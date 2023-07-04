// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700


#include <stdio.h>

#ifndef PARSER_H // header guard (serve pra n definir a struc duas vezes)
#define PARSER_H
typedef struct registro
{
    char removido;
    int tamanhoRegistro;
    int prox;
    int id;
    int ano;
    int qtt;
    char sigla[2];
    int tamCidade;
    char codC5;
    char *cidade;
    int tamMarca;
    char codC6;
    char *marca;
    int tamModelo;
    char codC7;
    char *modelo;
} Registro_t; // struct com todos os dados de um registro do arquivo

typedef struct registroIndice
{
    int indice;
    int rrn;
    long int byteOffset;
} RegistroIndice_t; // struct dos indices, se for tipo1 o byteoffset sera -1 e vice versa

#endif

void inicializarRegistro(Registro_t *reg);
int desalocarRegistro(Registro_t *reg);
int montarRegistroPesquisa(Registro_t *parametrosPesquisa, char **campos, char **valores, int qtdCampos);
int tamString(char *string);
int compararPesquisaRegistro(Registro_t *regProcurar, Registro_t *regInformacoes);
int printarStructRegistro(Registro_t *registroPrintar);
int addBinarioRegistro(Registro_t *registroNovo, FILE *arqBin, int tipoArq);
int mudarStatus(FILE *arqBin, int novoStatus);
int montarCabecalho(FILE *saida, int tipoArq);
int lerCampo(FILE *fp, char **string);
void endLine(FILE *fp);
void quickSort(RegistroIndice_t **regs, int min, int max);
int ordenarVetorIndices(RegistroIndice_t **regs, int qtdRegs);
void inicializarRegistroIndice(RegistroIndice_t *reg);
int inserirIndice(FILE *arqIndice, RegistroIndice_t **regs, int qtdRegs, int tipoArq);
void copiarSemBarraZero(char *string, char *dest);
long int pesquisaBinariaIndice(FILE *arqInd, int id, RegistroIndice_t *result, int tipoArq);
int alterarReg(Registro_t *regAlterar, Registro_t *regNovosDados);
int atualizarIndices(FILE *arqInd, RegistroIndice_t *regAnt, RegistroIndice_t *regNovo, int tipoArq);
int inserirRegistroPosAtual(FILE *arqBin, Registro_t *regInserir, int tamRegAnt,int tipoArq);
void quickSortByteOffset(RegistroIndice_t **regs,int *tamanhoRegs ,int min, int max);
void quickSortByteOffset2(RegistroIndice_t **regs, int min, int max);
int pegarRegistroDadosByteOffset(FILE *arqBin, long int byteOffset,Registro_t *reg, int tipoArq);