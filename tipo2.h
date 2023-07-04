// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700


#include "auxiliar.h"
#include <stdio.h>

int printarArquivoBinInteiroTipo2(char *nomeArq);
int pesquisarComCamposTipo2(char *arqPesq, char **campos, char **valores, int qtdCampos);
int atualizarByteOffsetCabecalho(FILE *arqBin, long int byteOffset); // atualiza o proximo byteoffset disponivel e o coloca no cabecalho
int montarVetorIndicesTipo2(FILE *arqBin, FILE *arqInd,RegistroIndice_t **vetorIndices, int *qtdIndices);
int pegarProxRegistroTipo2(FILE *arqReg, Registro_t *reg);
int inserirRegistroTipo2(FILE *arqBin, FILE *arqBinInd, Registro_t *regInserir, int tamRegAnt,long int byteOffset, int tipoInd);
int alterarValoresRegistroTipo2(FILE *arqDados,FILE *arqInd ,Registro_t *novosDados, long int byteOffset);
int inserirNovosRegTipo2(char *strArqDados, char* strArqInd,int qtdNovosReg, int tipoInd);