
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iup.h>

// Cabeçalhos para o GTK Nativo
#ifdef __linux__
  #include <gtk/gtk.h>
  #include <gtk/gtkunixprint.h>
#endif

// --- Função: Gera um HTML simples e converte para PDF ---
void gerar_pdf_teste(const char *nome_pdf) {
    printf("1. Gerando HTML temporário...\n");
    FILE *f = fopen("temp.html", "w");
    fprintf(f, "<html><body><h1>Teste do Sistema</h1><p>Se voce esta lendo isso, o wkhtmltopdf funcionou!</p></body></html>");
    fclose(f);

    printf("2. Convertendo para PDF (Aguarde)...\n");
    char comando[1024];
    // Chama o binário que está na pasta redist
    sprintf(comando, "./redist/wkhtmltopdf -q temp.html \"%s\"", nome_pdf);
    
    if (system(comando) == 0) {
        printf("   PDF gerado com sucesso: %s\n", nome_pdf);
    } else {
        IupMessage("Erro", "Falha ao chamar wkhtmltopdf na pasta redist!");
    }
    remove("temp.html"); // Limpa o lixo
}

// --- Função: Chama a janela nativa de impressão do Linux ---
void imprimir_nativo(const char *caminho_pdf) {
#ifdef __linux__
    if (!gtk_init_check(NULL, NULL)) return;

    GtkWidget *dialog = gtk_print_unix_dialog_new("Selecione a Impressora", NULL);
    GtkPrintCapabilities cap = GTK_PRINT_CAPABILITY_COPIES;
    gtk_print_unix_dialog_set_manual_capabilities(GTK_PRINT_UNIX_DIALOG(dialog), cap);

    printf("3. Abrindo diálogo nativo GTK...\n");
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY) {
        GtkPrinter *printer = gtk_print_unix_dialog_get_selected_printer(GTK_PRINT_UNIX_DIALOG(dialog));
        if (printer) {
            const char *nome_imp = gtk_printer_get_name(printer);
            printf("   Usuario escolheu: %s\n", nome_imp);
            
            char cmd[1024];
            sprintf(cmd, "lpr -P \"%s\" \"%s\"", nome_imp, caminho_pdf);
            system(cmd);
            IupMessage("Sucesso", "Enviado para a impressora!");
        }
    }
    gtk_widget_destroy(dialog);
    while (gtk_events_pending()) gtk_main_iteration();
#endif
}

// --- Callback do Botão ---
int btn_teste_cb(Ihandle *self) {
    const char *pdf = "teste_saida.pdf";
    gerar_pdf_teste(pdf);
    imprimir_nativo(pdf);
    return IUP_DEFAULT;
}

int main(int argc, char **argv) {
    IupOpen(&argc, &argv);

    Ihandle *btn = IupButton("Gerar PDF e Imprimir", NULL);
    IupSetCallback(btn, "ACTION", (Icallback)btn_teste_cb);
    IupSetAttribute(btn, "PADDING", "20x20");

    Ihandle *vbox = IupVbox(IupLabel("Teste de Ambiente Linux"), btn, NULL);
    IupSetAttribute(vbox, "MARGIN", "50x50");
    IupSetAttribute(vbox, "GAP", "20");

    Ihandle *dlg = IupDialog(vbox);
    IupSetAttribute(dlg, "TITLE", "Setup Test");
    
    IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
    IupMainLoop();
    IupClose();
    return 0;
}