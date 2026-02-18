#ifndef CONSTANTES_H
#define CONSTANTES_H

#define MAX_ALUNOS 1000
#define MAX_TEMPLATES 500
#define TAM_NOME 128
#define TAM_INSTRUCOES 512
#define TAM_MIOLO (512 * 1024)

#define PATH_INDEX_ALUNOS "data/alunos.idx"
#define PATH_INDEX_TREINOS "data/treinos.idx"

// Declarando como extern para que qualquer arquivo .c saiba que elas existem
extern int print_enable;
extern int impressao_disponivel;

#endif
