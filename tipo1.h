// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700


#include <stdio.h>
#include "auxiliar.h"

int pesquisarComCamposTipo1(char *arqPesq, char **campos, char **valores, int qtdCampos);
int printarResultadosPesquisaTipo1(FILE *arqBin, Registro_t *regPesquisar);
int printarArquivoBinInteiroTipo1(char *arq);
int pegarProximoRegTipo1(FILE *arqBin, Registro_t *reg);
int pesquisaRRN(int rrn, char *arqBin);
int atualizarRRNCabecalho(FILE *arqBin, int novoRRN); // atualiza o prox rrn disponivel e o coloca no cabecalho
int montarVetorIndicesTipo1(FILE *arqBin,FILE *arqInd, RegistroIndice_t **vetorIndices, int *qtdIndices);
int alterarValoresRegistroTipo1(FILE *arqDados, Registro_t *novosDados, int rrn);
int inserirRegistroTipo1(FILE* arqDados, FILE* arqInd, Registro_t* reg, int tipoIndice);
int atualizarIndicesTipo1(FILE *arqInd, RegistroIndice_t *regAnt, RegistroIndice_t *regNovo, int tipoArq);