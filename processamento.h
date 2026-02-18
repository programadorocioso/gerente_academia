#ifndef PROCESSAMENTO_H
#define PROCESSAMENTO_H

#include "constantes.h"
#include <stddef.h>

extern int print_enable;
extern int impressao_disponivel;

typedef struct { int id; char nome[TAM_NOME]; } indice_alunos;
typedef struct { int id; char nome[TAM_NOME]; char musculo[64]; } indice_treinos;

typedef struct {
    char nome[TAM_NOME];
    char musculo[64];
    char instrucoes[TAM_INSTRUCOES];
} TreinoTemplate;

typedef struct {
    int id_template; 
    char series[32];
    char repeticoes[32];
    char tempo_descanso[32];
    char espera[32];
    char carga[32];
} TreinoAplicado;

typedef struct {
    char nome[TAM_NOME];
    char idade[10], peso[10], altura[10], bf_estimado[10];
    char periodo[64]; // Campo de Texto Livre

    char pescoco[10], ombro[10], torax[10], cintura[10], abdome[10], quadril[10];
    char braco_dir[10], braco_esq[10], antebraco_dir[10], antebraco_esq[10];
    char coxa_dir[10], coxa_esq[10], panturrilha_dir[10], panturrilha_esq[10];
    char dobra_tricipital[10], dobra_subescapular[10], dobra_suprailiaca[10], dobra_abdominal[10];
    TreinoAplicado treinos_semanais[7][20]; 
} Aluno;

void ambiente();
void* carregar_item(int tipo, int id);
void salvar_universal(int tipo, void* dado, int id);
void gerar_pdf(Aluno* alu);
int gerar_novo_id(const char* path_index, size_t size_struct);

#endif
