// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700

#include <stdio.h>
#include "auxiliar.h"

int criarArquivo(char *arqCSV, char *arqSaida, int tipoArq); // le o csv e cria um arquivo binario com os registros inseridos nele, funciona para ambos os tipos
char *readUntil(char stop);                                  // le uma string da entrada padrao ate o caracter de parada
int criarBinIndice(char *nomeArqEnt, char *nomeArqInd, int tipoArq);
Registro_t *lerRegistro();
int excluirRegistrosEntrada(char *stringArqBin, char *stringArqInd, int qtdLinhas, int tipoArq);
int atualizarDadosRegistro(char *stringArqDados, char *stringArqInd, char **campos, char **valores, int qtdCampos, char **camposSub, char **valoresSub, int qtdCamposSub, int tipoArq);