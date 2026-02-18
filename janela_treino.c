#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iup.h>
#include "janela_treino.h"
#include "processamento.h"
#include "constantes.h"

void atualizar_lista_local(Ihandle* lista) {
    IupSetAttribute(lista, "REMOVEITEM", "ALL");
    FILE *f = fopen(PATH_INDEX_TREINOS, "rb");
    if (!f) return;
    indice_treinos reg;
    int i = 1;
    while(fread(&reg, sizeof(indice_treinos), 1, f)) {
        char buffer[256];
        sprintf(buffer, "[%s] %s", reg.musculo, reg.nome);
        IupSetAttributeId(lista, "", i, buffer);
        i++;
    }
    fclose(f);
}

const char* pegar_val(Ihandle* h) {
    char* s = IupGetAttribute(h, "VALUE");
    return s ? s : "";
}

int btn_salvar_treino_cb(Ihandle* self) {
    Ihandle *nome = (Ihandle*)IupGetAttribute(self, "REF_NOME");
    Ihandle *musc = (Ihandle*)IupGetAttribute(self, "REF_MUSC");
    Ihandle *inst = (Ihandle*)IupGetAttribute(self, "REF_INST");
    Ihandle *lista = (Ihandle*)IupGetAttribute(self, "REF_LISTA");

    TreinoTemplate t;
    memset(&t, 0, sizeof(TreinoTemplate));
    
    strncpy(t.nome, pegar_val(nome), TAM_NOME-1);
    strncpy(t.musculo, pegar_val(musc), 63);
    strncpy(t.instrucoes, pegar_val(inst), TAM_INSTRUCOES-1);

    if(strlen(t.nome) == 0) {
        IupMessage("Erro", "O nome do exercício é obrigatório.");
        return IUP_DEFAULT;
    }

    int id = (int)time(NULL) % 5000;
    salvar_universal(2, &t, id);

    FILE *f = fopen(PATH_INDEX_TREINOS, "ab");
    if(f) {
        indice_treinos idx = {id, "", ""};
        strncpy(idx.nome, t.nome, TAM_NOME-1);
        strncpy(idx.musculo, t.musculo, 63);
        fwrite(&idx, sizeof(indice_treinos), 1, f);
        fclose(f);
    }

    atualizar_lista_local(lista);
    IupMessage("Sucesso", "Exercício cadastrado!");
    return IUP_DEFAULT;
}

Ihandle* criar_janela_treino() {
    // Campos
    Ihandle *txt_nome = IupText(NULL);
    IupSetAttribute(txt_nome, "EXPAND", "HORIZONTAL");
    
    Ihandle *txt_musculo = IupText(NULL); 
    IupSetAttribute(txt_musculo, "EXPAND", "HORIZONTAL");

    // Instruções dentro de um FRAME para ter borda e título (A "Caixinha")
    Ihandle *txt_instru = IupMultiLine(NULL);
    IupSetAttribute(txt_instru, "EXPAND", "YES");
    IupSetAttribute(txt_instru, "SIZE", "x80"); // Altura fixa boa
    IupSetAttribute(txt_instru, "BORDER", "YES"); 
    
    Ihandle *frame_inst = IupFrame(txt_instru);
    IupSetAttribute(frame_inst, "TITLE", "Instruções / Observações");

    // Lista
    Ihandle *lst_treinos = IupList(NULL);
    IupSetAttribute(lst_treinos, "EXPAND", "HORIZONTAL");
    IupSetAttribute(lst_treinos, "VISIBLELINES", "6");
    IupSetAttribute(lst_treinos, "DROPDOWN", "NO"); // Lista aberta

    // Botões
    Ihandle *btn_salvar = IupButton("Salvar Treino", NULL);
    Ihandle *btn_fechar = IupButton("Fechar", "cb_sair");
    
    // Conectando referências
    IupSetAttribute(btn_salvar, "REF_NOME", (char*)txt_nome);
    IupSetAttribute(btn_salvar, "REF_MUSC", (char*)txt_musculo);
    IupSetAttribute(btn_salvar, "REF_INST", (char*)txt_instru);
    IupSetAttribute(btn_salvar, "REF_LISTA", (char*)lst_treinos);
    IupSetCallback(btn_salvar, "ACTION", (Icallback)btn_salvar_treino_cb);

    Ihandle *box_botoes = IupHbox(IupFill(), btn_salvar, btn_fechar, NULL);
    IupSetAttribute(box_botoes, "GAP", "10");

    // Layout Principal
    Ihandle *layout = IupVbox(
        IupLabel("Nome do Exercício:"), txt_nome,
        IupLabel("Grupamento (ex: Peito):"), txt_musculo,
        frame_inst,     // A caixa de instruções entra aqui
        box_botoes,     // Botões logo abaixo das instruções
        IupLabel("--------------------------------"),
        IupLabel("Biblioteca Atual:"),
        lst_treinos,
        NULL
    );

    IupSetAttribute(layout, "MARGIN", "15x15");
    IupSetAttribute(layout, "GAP", "10");

    Ihandle *dlg = IupDialog(layout);
    IupSetAttribute(dlg, "TITLE", "Cadastro de Exercícios");
    IupSetAttribute(dlg, "SIZE", "350x550"); 

    atualizar_lista_local(lst_treinos);
    return dlg;
}
