#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iup.h>
#include "janela_aluno.h"
#include "processamento.h"
#include "constantes.h"

typedef struct {
    Ihandle *nome, *idade, *peso, *altura, *bf;
    Ihandle *pesc, *ombr, *torax, *cint, *abdo, *quad;
    Ihandle *brac_d, *brac_e, *ant_d, *ant_e;
    Ihandle *coxa_d, *coxa_e, *pant_d, *pant_e;
    Ihandle *tri, *sub, *supra, *abdo_d;
} CamposAluno;

const char* safe_get(Ihandle* h) {
    char* s = IupGetAttribute(h, "VALUE");
    return s ? s : "";
}

int cb_salvar_aluno(Ihandle* self) {
    CamposAluno* c = (CamposAluno*)IupGetAttribute(self, "CAMPOS_REF");
    if(!c) return IUP_ERROR;
    Aluno a; memset(&a, 0, sizeof(Aluno));

    strncpy(a.nome, safe_get(c->nome), TAM_NOME-1);
    if(strlen(a.nome) == 0) {
        IupMessage("Erro", "Nome é obrigatório!");
        return IUP_DEFAULT;
    }

    // Copia todos os campos de forma segura
    strncpy(a.idade, safe_get(c->idade), 9);
    strncpy(a.peso, safe_get(c->peso), 9);
    strncpy(a.altura, safe_get(c->altura), 9);
    strncpy(a.bf_estimado, safe_get(c->bf), 9);
    strncpy(a.pescoco, safe_get(c->pesc), 9);
    strncpy(a.ombro, safe_get(c->ombr), 9);
    strncpy(a.torax, safe_get(c->torax), 9);
    strncpy(a.cintura, safe_get(c->cint), 9);
    strncpy(a.abdome, safe_get(c->abdo), 9);
    strncpy(a.quadril, safe_get(c->quad), 9);
    strncpy(a.braco_dir, safe_get(c->brac_d), 9);
    strncpy(a.braco_esq, safe_get(c->brac_e), 9);
    strncpy(a.antebraco_dir, safe_get(c->ant_d), 9);
    strncpy(a.antebraco_esq, safe_get(c->ant_e), 9);
    strncpy(a.coxa_dir, safe_get(c->coxa_d), 9);
    strncpy(a.coxa_esq, safe_get(c->coxa_e), 9);
    strncpy(a.panturrilha_dir, safe_get(c->pant_d), 9);
    strncpy(a.panturrilha_esq, safe_get(c->pant_e), 9);
    strncpy(a.dobra_tricipital, safe_get(c->tri), 9);
    strncpy(a.dobra_subescapular, safe_get(c->sub), 9);
    strncpy(a.dobra_suprailiaca, safe_get(c->supra), 9);
    strncpy(a.dobra_abdominal, safe_get(c->abdo_d), 9);

    int id = (int)time(NULL) % 10000;
    salvar_universal(1, &a, id);

    FILE *f = fopen(PATH_INDEX_ALUNOS, "ab");
    if(f) {
        indice_alunos idx = {id, ""};
        strncpy(idx.nome, a.nome, TAM_NOME-1);
        fwrite(&idx, sizeof(indice_alunos), 1, f);
        fclose(f);
    }
    IupMessage("Sucesso", "Aluno Salvo!");
    return IUP_CLOSE;
}

// Cria um "bloco" vertical pequeno: Label + Input
Ihandle* bloco_med(const char* label, Ihandle** campo) {
    Ihandle* lbl = IupLabel(label);
    IupSetAttribute(lbl, "FONT", "HELVETICA_NORMAL_8"); // Fonte menor pra caber
    *campo = IupText(NULL);
    IupSetAttribute(*campo, "SIZE", "40x"); // Largura fixa compacta
    return IupVbox(lbl, *campo, NULL);
}

Ihandle* criar_janela_aluno() {
    CamposAluno* c = malloc(sizeof(CamposAluno));
    
    // 1. Dados Básicos (Linha única)
    Ihandle* box_basicos = IupHbox(
        bloco_med("IDADE", &c->idade),
        bloco_med("PESO", &c->peso),
        bloco_med("ALTURA", &c->altura),
        bloco_med("% GORD", &c->bf),
        NULL
    );
    IupSetAttribute(box_basicos, "GAP", "15");

    // 2. Grid de Perímetros (3 Colunas organizadas)
    Ihandle* grid_perim = IupGridBox(
        bloco_med("Pescoço", &c->pesc), bloco_med("Ombro", &c->ombr), bloco_med("Tórax", &c->torax),
        bloco_med("Cintura", &c->cint), bloco_med("Abdome", &c->abdo), bloco_med("Quadril", &c->quad),
        bloco_med("Braço D", &c->brac_d), bloco_med("Braço E", &c->brac_e), bloco_med("Anteb. D", &c->ant_d),
        bloco_med("Anteb. E", &c->ant_e), bloco_med("Coxa D", &c->coxa_d), bloco_med("Coxa E", &c->coxa_e),
        bloco_med("Pantur. D", &c->pant_d), bloco_med("Pantur. E", &c->pant_e), IupLabel(""), // Label vazio pra fechar grid
        NULL
    );
    IupSetAttribute(grid_perim, "NUMDIV", "3"); // 3 Colunas
    IupSetAttribute(grid_perim, "GAPCOL", "20");
    IupSetAttribute(grid_perim, "GAPLIN", "10");

    // 3. Grid de Dobras (4 Colunas)
    Ihandle* box_dobras = IupHbox(
        bloco_med("Tricipital", &c->tri), bloco_med("Subescap.", &c->sub),
        bloco_med("Suprail.", &c->supra), bloco_med("Abdom.", &c->abdo_d),
        NULL
    );
    IupSetAttribute(box_dobras, "GAP", "15");

    // CONTEÚDO ROLÁVEL (Apenas os campos)
    Ihandle* conteudo_scroll = IupVbox(
        IupLabel("DADOS PESSOAIS:"), (c->nome = IupText(NULL)), box_basicos,
        IupLabel("----------------------------------"),
        IupLabel("PERÍMETROS:"), grid_perim,
        IupLabel("----------------------------------"),
        IupLabel("DOBRAS:"), box_dobras,
        NULL
    );
    IupSetAttribute(c->nome, "EXPAND", "HORIZONTAL");
    IupSetAttribute(conteudo_scroll, "MARGIN", "10x10");
    IupSetAttribute(conteudo_scroll, "GAP", "8");
    
    Ihandle* scroll = IupScrollBox(conteudo_scroll);

    // BARRA DE AÇÃO FIXA (Isso garante que o botão apareça!)
    Ihandle* btn_salvar = IupButton("Salvar Ficha", NULL);
    IupSetAttribute(btn_salvar, "CAMPOS_REF", (char*)c);
    IupSetAttribute(btn_salvar, "PADDING", "10x5");
    IupSetCallback(btn_salvar, "ACTION", (Icallback)cb_salvar_aluno);

    Ihandle* barra_botoes = IupHbox(IupFill(), btn_salvar, IupButton("Cancelar", "cb_sair"), NULL);
    IupSetAttribute(barra_botoes, "MARGIN", "10x10");
    IupSetAttribute(barra_botoes, "GAP", "10");

    // Layout Final: Scroll em cima + Botões fixos embaixo
    Ihandle* layout_janela = IupVbox(scroll, IupLabel(""), barra_botoes, NULL);

    Ihandle* dlg = IupDialog(layout_janela);
    IupSetAttribute(dlg, "TITLE", "Ficha Antropométrica");
    IupSetAttribute(dlg, "SIZE", "420x550"); // Tamanho vertical maior
    return dlg;
}
