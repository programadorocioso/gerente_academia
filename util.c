#include "util.h"
#include <stdlib.h>
#include <stdio.h>

void limpar_tela() {
    #ifdef _WIN32
        system("cls"); // Comando Windows
    #else
        system("clear"); // Comando Linux
    #endif
}

// Útil para pausar o CLI de forma cross-platform
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    #ifdef _WIN32
        getchar(); // No Windows o buffer costuma precisar de um segundo getchar
    #endif
}