#!/bin/bash

# ==============================================================================
# ATUALIZANDO MAIN_CLI.C (Versão Final Estável)
# ==============================================================================
cat << 'EOF' > main_cli.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "processamento.h"
#include "constantes.h"

// Função para limpar o lixo do teclado
void limpar_buffer() { 
    int c; 
    while ((c = getchar()) != '\n' && c != EOF); 
}

// A FUNÇÃO DE VISUALIZAÇÃO QUE ESTAVA FALTANDO/DANDO ERRO
void visualizar_pdf() {
    char caminho_completo[1024];
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        #ifdef _WIN32
            // Windows: Tenta o Sumatra (redist) ou o visualizador padrão
            sprintf(caminho_completo, "redist\\pdf_print.exe \"%s\\data\\tmp\\latest.pdf\"", cwd);
            if (system(caminho_completo) != 0) {
                sprintf(caminho_completo, "start \"\" \"%s\\data\\tmp\\latest.pdf\"", cwd);
                system(caminho_completo);
            }
        #else
            // Linux: Força o xdg-open com caminho absoluto para não ter erro
            sprintf(caminho_completo, "xdg-open \"%s/data/tmp/latest.pdf\" > /dev/null 2>&1", cwd);
            if (system(caminho_completo) != 0) {
                printf("\n[AVISO] PDF não abriu. Tentando abrir o HTML no navegador...\n");
                sprintf(caminho_completo, "xdg-open \"%s/data/tmp/print.html\" > /dev/null 2>&1", cwd);
                system(caminho_completo);
            }
        #endif
    } else {
        printf("[ERRO] Falha ao obter diretório atual para o xdg-open.\n");
    }
}

void cadastrar_aluno_cli(int id_existente) {
    Aluno a; 
    memset(&a, 0, sizeof(Aluno));

    if(id_existente > 0) {
        Aluno* antigo = (Aluno*)carregar_item(1, id_existente);
        if(antigo) { 
            a = *antigo; 
            free(antigo); 
            printf("\n--- EDITANDO ALUNO (ID %d) ---\n", id_existente); 
        }
    } else {
        printf("\n--- NOVO CADASTRO DE ALUNO ---\n");
    }

    printf("Nome Completo: "); fgets(a.nome, TAM_NOME, stdin); a.nome[strcspn(a.nome, "\n")] = 0;
    printf("Idade (anos): "); fgets(a.idade, 10, stdin); a.idade[strcspn(a.idade, "\n")] = 0;
    printf("Peso (kg): "); fgets(a.peso, 10, stdin); a.peso[strcspn(a.peso, "\n")] = 0;
    printf("Altura (cm): "); fgets(a.altura, 10, stdin); a.altura[strcspn(a.altura, "\n")] = 0;
    
    // Validade da Ficha (não salva na struct Aluno se preferir, mas aqui mantemos para o PDF)
    printf("Validade do Protocolo (ex: 18/02/26 - 18/05/26): "); 
    fgets(a.periodo, 64, stdin); a.periodo[strcspn(a.periodo, "\n")] = 0;

    printf("\n--- MEDIDAS ANTROPOMÉTRICAS (cm) ---\n");
    printf("Tórax: "); fgets(a.torax, 10, stdin); a.torax[strcspn(a.torax, "\n")] = 0;
    printf("Cintura: "); fgets(a.cintura, 10, stdin); a.cintura[strcspn(a.cintura, "\n")] = 0;
    printf("Abdome: "); fgets(a.abdome, 10, stdin); a.abdome[strcspn(a.abdome, "\n")] = 0;
    printf("Quadril: "); fgets(a.quadril, 10, stdin); a.quadril[strcspn(a.quadril, "\n")] = 0;

    int id = (id_existente > 0) ? id_existente : gerar_novo_id(PATH_INDEX_ALUNOS, sizeof(indice_alunos));
    salvar_universal(1, &a, id);
    
    if(id_existente == 0) {
        FILE *f = fopen(PATH_INDEX_ALUNOS, "ab");
        if(f) { 
            indice_alunos idx = {id, ""}; 
            strncpy(idx.nome, a.nome, TAM_NOME-1); 
            fwrite(&idx, sizeof(indice_alunos), 1, f); 
            fclose(f); 
        }
    }
    printf("\n[SUCESSO] Dados do Aluno ID %d atualizados!\n", id);
    printf("PRESSIONE ENTER..."); limpar_buffer();
}

void montar_treino_cli() {
    int id_alu, id_tre, dia;
    char o_d, o_t;

    printf("\nID do Aluno para montar ficha: "); 
    if(scanf("%d", &id_alu) != 1) { limpar_buffer(); return; }
    limpar_buffer();

    Aluno* a = (Aluno*)carregar_item(1, id_alu);
    if(!a) { printf("[ERRO] Aluno não encontrado!\n"); return; }

    do {
        printf("\n--- Seleção de Dia (0-Seg, 1-Ter, 2-Qua, 3-Qui, 4-Sex, 5-Sáb, 6-Dom) ---\n");
        printf("Escolha o dia: "); scanf("%d", &dia); limpar_buffer();
        if(dia < 0 || dia > 6) continue;

        do {
            printf("\n  ID do Exercício: "); scanf("%d", &id_tre); limpar_buffer();
            TreinoTemplate* t = (TreinoTemplate*)carregar_item(2, id_tre);
            
            if(t) {
                int s = -1;
                for(int i=0; i<20; i++) if(a->treinos_semanais[dia][i].id_template == 0) { s = i; break; }

                if(s != -1) {
                    a->treinos_semanais[dia][s].id_template = id_tre;
                    printf("    Séries: "); fgets(a->treinos_semanais[dia][s].series, 32, stdin); 
                    a->treinos_semanais[dia][s].series[strcspn(a->treinos_semanais[dia][s].series, "\n")] = 0;
                    
                    printf("    Repetições: "); fgets(a->treinos_semanais[dia][s].repeticoes, 32, stdin); 
                    a->treinos_semanais[dia][s].repeticoes[strcspn(a->treinos_semanais[dia][s].repeticoes, "\n")] = 0;
                    
                    printf("    Carga (kg): "); fgets(a->treinos_semanais[dia][s].carga, 32, stdin); 
                    a->treinos_semanais[dia][s].carga[strcspn(a->treinos_semanais[dia][s].carga, "\n")] = 0;
                    
                    printf("    Descanso: "); fgets(a->treinos_semanais[dia][s].tempo_descanso, 32, stdin); 
                    a->treinos_semanais[dia][s].tempo_descanso[strcspn(a->treinos_semanais[dia][s].tempo_descanso, "\n")] = 0;
                }
                free(t);
            } else printf("  [ERRO] Exercício não encontrado.\n");

            printf("  Adicionar outro exercício na %s? (s/n): ", (dia==0)?"Seg":(dia==1)?"Ter":(dia==2)?"Qua":(dia==3)?"Qui":(dia==4)?"Sex":(dia==5)?"Sáb":"Dom");
            scanf("%c", &o_t); limpar_buffer();
        } while(o_t == 's' || o_t == 'S');

        salvar_universal(1, a, id_alu);
        printf("\n[SALVO] Treinos de dia %d registrados.\n", dia);

        printf("Deseja editar outro dia? (s/n): ");
        scanf("%c", &o_d); limpar_buffer();
    } while(o_d == 's' || o_d == 'S');

    free(a);
}

void listar_geral() {
    FILE *f = fopen(PATH_INDEX_ALUNOS, "rb");
    if(f) {
        indice_alunos idx;
        printf("\n--- LISTA DE ALUNOS ---\n");
        while(fread(&idx, sizeof(indice_alunos), 1, f)) printf("ID #%d - %s\n", idx.id, idx.nome);
        fclose(f);
    }
    f = fopen(PATH_INDEX_TREINOS, "rb");
    if(f) {
        indice_treinos idx;
        printf("\n--- LISTA DE EXERCÍCIOS BASE ---\n");
        while(fread(&idx, sizeof(indice_treinos), 1, f)) printf("ID #%d - %s\n", idx.id, idx.nome);
        fclose(f);
    }
    printf("\nPRESSIONE ENTER..."); getchar(); limpar_buffer();
}

int main() {
    int o = -1;
    ambiente();
    
    while (o != 0) {
        printf("\n============================\n");
        printf("      UFS CLUB - ACADEMIA\n");
        printf("============================\n");
        printf("1. Novo Aluno\n");
        printf("2. Editar Aluno (por ID)\n");
        printf("3. Cadastrar Novo Exercício\n");
        printf("4. Listar Alunos / Exercícios\n");
        printf("5. Montar Ficha de Treino\n");
        printf("6. Gerar PDF e Visualizar\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        
        if (scanf("%d", &o) != 1) { limpar_buffer(); continue; }
        limpar_buffer();

        switch (o) {
            case 1: cadastrar_aluno_cli(0); break;
            case 2: {
                int id; printf("Digite o ID do Aluno: "); 
                scanf("%d", &id); limpar_buffer();
                cadastrar_aluno_cli(id); 
                break;
            }
            case 3: {
                TreinoTemplate t; memset(&t, 0, sizeof(t));
                printf("\nNome do Exercício: "); fgets(t.nome, TAM_NOME, stdin); t.nome[strcspn(t.nome, "\n")] = 0;
                printf("Instruções/Dicas: "); fgets(t.instrucoes, TAM_INSTRUCOES, stdin); t.instrucoes[strcspn(t.instrucoes, "\n")] = 0;
                int id = gerar_novo_id(PATH_INDEX_TREINOS, sizeof(indice_treinos));
                salvar_universal(2, &t, id);
                FILE *f = fopen(PATH_INDEX_TREINOS, "ab");
                if(f) { 
                    indice_treinos idx = {id, "", ""}; 
                    strncpy(idx.nome, t.nome, TAM_NOME-1); 
                    strncpy(idx.musculo, "Geral", 63);
                    fwrite(&idx, sizeof(indice_treinos), 1, f); 
                    fclose(f); 
                }
                printf("[OK] Exercício ID %d cadastrado!\n", id);
                break;
            }
            case 4: listar_geral(); break;
            case 5: montar_treino_cli(); break;
            case 6: {
                int id; printf("ID do Aluno: "); 
                scanf("%d", &id); limpar_buffer();
                Aluno* a = (Aluno*)carregar_item(1, id);
                if(a) { 
                    gerar_pdf(a); 
                    visualizar_pdf(); 
                    free(a); 
                }
                break;
            }
        }
    }
    return 0;
}
EOF

# Compilar e atualizar dist
make clean && make dist
