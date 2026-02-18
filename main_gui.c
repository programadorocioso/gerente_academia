#include <iup.h>
#include <string.h>
#include <stdlib.h>
#include "processamento.h"
#include "janela_aluno.h"
#include "janela_treino.h"

Ihandle *lista_alunos_main;

int cb_sair(Ihandle* self) { (void)self; return IUP_CLOSE; }

int cb_abrir_aluno(Ihandle* self) {
    (void)self;
    IupShow(criar_janela_aluno());
    return IUP_DEFAULT;
}

int cb_abrir_treino(Ihandle* self) {
    (void)self;
    IupShow(criar_janela_treino());
    return IUP_DEFAULT;
}

int cb_gerar_latest(Ihandle* self) {
    (void)self;
    Aluno a; memset(&a, 0, sizeof(a));
    strcpy(a.nome, "RELATORIO_AVULSO");
    gerar_pdf(&a);
    IupMessage("PDF Gerado", "O arquivo foi criado em:\ndata/tmp/latest.pdf");
    return IUP_DEFAULT;
}

int cb_imprimir(Ihandle* self) {
    (void)self;
    int ret = system("lpr data/tmp/latest.pdf 2>/dev/null || lp data/tmp/latest.pdf");
    if(ret == 0)
        IupMessage("Impressão", "Comando enviado para a impressora!");
    else
        IupMessage("Erro Impressão", "Falha ao enviar. Verifique se 'lpr' está instalado ou abra o PDF manualmente.");
    return IUP_DEFAULT;
}

int main(int argc, char* argv[]) {
    IupOpen(&argc, &argv);
    ambiente();

    IupSetFunction("cb_aluno", (Icallback)cb_abrir_aluno);
    IupSetFunction("cb_treino", (Icallback)cb_abrir_treino);
    IupSetFunction("cb_sair", (Icallback)cb_sair);

    lista_alunos_main = IupList(NULL);
    IupSetAttribute(lista_alunos_main, "EXPAND", "YES");

    Ihandle *dlg = IupDialog(IupVbox(
        IupLabel("UFS CLUB - DASHBOARD"),
        lista_alunos_main,
        IupHbox(IupButton("Novo Aluno", "cb_aluno"), IupButton("Exercícios", "cb_treino"), NULL),
        IupHbox(IupButton("Gerar Protocolo PDF", (Icallback)cb_gerar_latest), 
                IupButton("Imprimir Relatório", (Icallback)cb_imprimir), NULL),
        NULL
    ));

    IupSetAttribute(dlg, "TITLE", "Dashboard");
    IupSetAttribute(dlg, "MARGIN", "20x20");
    IupSetAttribute(dlg, "SIZE", "300x200");
    IupShow(dlg);
    IupMainLoop();
    IupClose();
    return 0;
}
