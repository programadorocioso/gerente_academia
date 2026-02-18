#include "processamento.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int print_enable = 1;
int impressao_disponivel = 1;

void ambiente() {
    const char *dirs[] = {"data","data/alunos","data/templates","data/tmp","redist"};
    for(int i=0; i<5; i++) {
        #ifdef _WIN32
            mkdir(dirs[i]);
        #else
            mkdir(dirs[i], 0777);
        #endif
    }
}

int gerar_novo_id(const char* path_index, size_t size_struct) {
    FILE *f = fopen(path_index, "rb");
    if (!f) return 1; 
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return (int)(size / size_struct) + 1;
}

void replace_all(char **str, const char *old, const char *new_val) {
    char *p;
    if (!(p = strstr(*str, old))) return;
    size_t new_len = strlen(*str) - strlen(old) + strlen(new_val) + 1;
    char *new_str = malloc(new_len);
    if (!new_str) return;
    size_t prefix_len = p - *str;
    memcpy(new_str, *str, prefix_len);
    strcpy(new_str + prefix_len, new_val);
    strcat(new_str, p + strlen(old));
    free(*str);
    *str = new_str;
    replace_all(str, old, new_val);
}

int tem_valor(const char* s) {
    return (s && strlen(s) > 0 && strcmp(s, "N/A") != 0 && strcmp(s, "\n") != 0);
}

void gerar_pdf(Aluno* a) {
    FILE *ft = fopen("data/template.html", "r");
    if(!ft) return;
    fseek(ft, 0, SEEK_END); long fsize = ftell(ft); fseek(ft, 0, SEEK_SET);
    char *html = malloc(fsize + TAM_MIOLO);
    size_t l = fread(html, 1, fsize, ft); html[l] = '\0'; fclose(ft);

    // Injeção de caminho absoluto para a fonte (Obrigatório para o PDF Build)
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        char f_url[2048];
        #ifdef _WIN32
            sprintf(f_url, "url('file:///%s/data/OpenSans-Regular.ttf')", cwd);
            for(int i=0; f_url[i]; i++) if(f_url[i] == '\\') f_url[i] = '/';
        #else
            sprintf(f_url, "url('file://%s/data/OpenSans-Regular.ttf')", cwd);
        #endif
        replace_all(&html, "{FONTE_URL}", f_url);
    }

    if(a) {
        replace_all(&html, "{NOME_ALUNO}", a->nome);
        replace_all(&html, "{PERIODO}", tem_valor(a->periodo) ? a->periodo : "A definir");
        
        char *miolo = malloc(TAM_MIOLO);
        miolo[0] = '\0';
        const char* dias[] = {"SEGUNDA-FEIRA", "TERÇA-FEIRA", "QUARTA-FEIRA", "QUINTA-FEIRA", "SEXTA-FEIRA", "SÁBADO", "DOMINGO"};
        
        for(int d=0; d<7; d++) {
            char ex_dia[15000] = ""; // Aumentado para segurança
            int d_t = 0;
            for(int i=0; i<20; i++) {
                if(a->treinos_semanais[d][i].id_template > 0) {
                    d_t = 1;
                    TreinoTemplate* temp = carregar_item(2, a->treinos_semanais[d][i].id_template);
                    if(temp) {
                        TreinoAplicado *app = &a->treinos_semanais[d][i];
                        char det[512] = "";
                        if(tem_valor(app->series)) sprintf(det + strlen(det), "<b>Séries:</b> %s | ", app->series);
                        if(tem_valor(app->repeticoes)) sprintf(det + strlen(det), "<b>Reps:</b> %s | ", app->repeticoes);
                        if(tem_valor(app->carga)) sprintf(det + strlen(det), "<b>Carga:</b> %s", app->carga);
                        
                        char bloco_ex[4096];
                        sprintf(bloco_ex, "<div class='exercicio-bloco'><div class='exercicio-nome'>%s</div><div class='exercicio-detalhes'>%s</div>%s</div>",
                            temp->nome, det, tem_valor(temp->instrucoes) ? (char[]){sprintf((char[2048]){}, "<div class='instrucoes-box'>%s</div>", temp->instrucoes), 0} : "");
                        strcat(ex_dia, bloco_ex); free(temp);
                    }
                }
            }
            if(d_t) {
                // CORREÇÃO DO OVERFLOW: Concatenando diretamente
                strcat(miolo, "<div class='dia-semana'>");
                strcat(miolo, dias[d]);
                strcat(miolo, "</div>");
                strcat(miolo, ex_dia);
            }
        }
        replace_all(&html, "{CONTEUDO_TREINO}", miolo); free(miolo);
    }

    FILE *fout = fopen("data/tmp/print.html", "w");
    if(fout) { fprintf(fout, "%s", html); fclose(fout); }
    free(html);
    
    #ifdef _WIN32
        system("redist\\pdf_build.exe --quiet --enable-local-file-access data\\tmp\\print.html data\\tmp\\latest.pdf");
    #else
        system("chmod +x redist/pdf_build 2>/dev/null");
        system("./redist/pdf_build --quiet --enable-local-file-access data/tmp/print.html data/tmp/latest.pdf");
    #endif
}

void salvar_universal(int t, void* d, int id) {
    char p[256]; sprintf(p, (t == 1) ? "data/alunos/%d.bin" : "data/templates/%d.bin", id);
    FILE *f = fopen(p, "wb"); if(f) { fwrite(d, (t == 1) ? sizeof(Aluno) : sizeof(TreinoTemplate), 1, f); fclose(f); }
}

void* carregar_item(int t, int id) {
    char p[256]; sprintf(p, (t == 1) ? "data/alunos/%d.bin" : "data/templates/%d.bin", id);
    FILE *f = fopen(p, "rb"); if(!f) return NULL;
    size_t s = (t == 1) ? sizeof(Aluno) : sizeof(TreinoTemplate);
    void* d = malloc(s); fread(d, s, 1, f); fclose(f); return d;
}
