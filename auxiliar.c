// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700

// esse .c contem funcoes auxiliares
#include "auxiliar.h"
#include "tipo1.h"
#include "tipo2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pegarRegistroIndice(FILE *arqInd,RegistroIndice_t *result ,long int byteOffset, int tipoArq);

void inicializarRegistroIndice(RegistroIndice_t *reg)
{
    if (reg != NULL)
    {
        reg->rrn = -1;
        reg->indice = -1;
        reg->byteOffset = -1;
    }
}

void inicializarRegistro(Registro_t *reg)
{
    reg->removido = '0';
    reg->tamanhoRegistro = -1;
    reg->prox = -1;
    reg->id = -1;
    reg->ano = -1;
    reg->qtt = -1;
    reg->sigla[0] = '$';
    reg->sigla[1] = '$';
    reg->tamMarca = 0;
    reg->tamModelo = 0;
    reg->tamCidade = 0;
    reg->codC5 = '0';
    reg->codC6 = '0';
    reg->codC7 = '0';
    reg->marca = NULL;
    reg->cidade = NULL;
    reg->modelo = NULL;
}

int desalocarRegistro(Registro_t *reg)
{
    if (!reg)
        return -1;

    if (reg->tamMarca > 0 && reg->marca)
        free(reg->marca);

    if (reg->tamModelo > 0 && reg->modelo)
        free(reg->modelo);

    if (reg->tamCidade > 0 && reg->cidade)
        free(reg->cidade);
    free(reg);

    return 1;
}

int montarRegistroPesquisa(Registro_t *parametrosPesquisa, char **campos, char **valores, int qtdCampos)
{
    if (!parametrosPesquisa || !campos || !valores || qtdCampos < 1)
        return -1;

    for (int i = 0; i < qtdCampos; ++i)
    { // monta o registro com as informacoes a serem pesquisadas
        if (strcmp(campos[i], "id") == 0 && parametrosPesquisa->id < 0)
        {
            parametrosPesquisa->id = atoi(valores[i]);
        }
        else if (strcmp(campos[i], "ano") == 0 && parametrosPesquisa->ano < 0 && parametrosPesquisa->ano != -2)
        {
            parametrosPesquisa->ano = atoi(valores[i]);
        }
        else if (strcmp(campos[i], "qtt") == 0 && parametrosPesquisa->qtt < 0 && parametrosPesquisa->qtt != -2)
        {
            parametrosPesquisa->qtt = atoi(valores[i]);
        }
        else if (strcmp(campos[i], "sigla") == 0 && parametrosPesquisa->sigla[0] == '$')
        {
            parametrosPesquisa->sigla[0] = valores[i][0];
            parametrosPesquisa->sigla[1] = valores[i][1];
        }

        else if (strcmp(campos[i], "cidade") == 0 && parametrosPesquisa->tamCidade <= 0)
        {
            if (parametrosPesquisa->cidade != NULL && strcmp(parametrosPesquisa->cidade, "NULO") == 0){
                parametrosPesquisa->tamCidade = -2;
                free(parametrosPesquisa->cidade);
            }

            else{
                parametrosPesquisa->tamCidade = tamString(valores[i]);
                parametrosPesquisa->cidade = malloc(parametrosPesquisa->tamCidade);
                copiarSemBarraZero(valores[i], parametrosPesquisa->cidade);
                parametrosPesquisa->cidade[parametrosPesquisa->tamCidade - 1] = '\0';
            }
        }

        else if (strcmp(campos[i], "marca") == 0 && parametrosPesquisa->tamMarca <= 0)
        {
            if (parametrosPesquisa->marca != NULL && strcmp(parametrosPesquisa->marca, "NULO") == 0){
                parametrosPesquisa->tamMarca = -2;
                free(parametrosPesquisa->marca);
            }

            else{
                parametrosPesquisa->tamMarca = tamString(valores[i]);
                parametrosPesquisa->marca = malloc(parametrosPesquisa->tamMarca);
                copiarSemBarraZero(valores[i], parametrosPesquisa->marca);
                parametrosPesquisa->marca[parametrosPesquisa->tamMarca - 1] = '\0';
            }
        }
        else if (strcmp(campos[i], "modelo") == 0 && parametrosPesquisa->tamModelo <= 0)
        {
            if (parametrosPesquisa->modelo != NULL && strcmp(parametrosPesquisa->modelo, "NULO") == 0){
                parametrosPesquisa->tamModelo = -2;
                free(parametrosPesquisa->modelo);
            }

            else{
                parametrosPesquisa->tamModelo = tamString(valores[i]);
                parametrosPesquisa->modelo = malloc(parametrosPesquisa->tamModelo);
                copiarSemBarraZero(valores[i], parametrosPesquisa->modelo);
                parametrosPesquisa->modelo[parametrosPesquisa->tamModelo - 1] = '\0';
            }
        }
    }

    return 1;
}

int tamString(char *string)
{
    if (!string)
        return -1;

    int i = 0;
    while (string[i] != '\0')
        i++;
    i++;

    return i;
}

int compararPesquisaRegistro(Registro_t *regProcurar, Registro_t *regInformacoes)
{
    // percorre campo a campo e verifica se sao iguais
    if (regInformacoes->id >= 0 && regProcurar->id != regInformacoes->id)
        return -1;

    if (((regInformacoes->ano == -2 && regProcurar->ano > 0) || (regInformacoes->ano >= 0 && regProcurar->ano != regInformacoes->ano)))
        return -1;

    if (((regInformacoes->qtt == -2 && regProcurar->qtt > 0) || (regInformacoes->qtt >= 0 && regProcurar->qtt != regInformacoes->qtt)))
        return -1;

    if ((regInformacoes->sigla[0] == 'N' && regProcurar->sigla[0] != '$') || (regInformacoes->sigla[0] != '$' && (regProcurar->sigla[0] != regInformacoes->sigla[0] || regProcurar->sigla[1] != regInformacoes->sigla[1]))){
        return -1;
    }

    if ((regInformacoes->tamCidade == -2 && regProcurar->tamCidade > 0) || regInformacoes->tamCidade > 0)
    {
        if (regInformacoes->tamCidade == -2 && regProcurar->cidade != NULL)
            return -1;

        else if (regProcurar->tamCidade <= 0)
            return -1;

        else if (strcmp(regProcurar->cidade, regInformacoes->cidade) != 0)
            return -1;

    }

    if ((regInformacoes->tamMarca == -2 && regProcurar->tamMarca > 0) || regInformacoes->tamMarca > 0)
    {
        if (regInformacoes->tamMarca == -2 && regProcurar->marca != NULL)
            return -1;

        else if (regProcurar->tamMarca <= 0)
            return -1;

        else if (strcmp(regProcurar->marca, regInformacoes->marca) != 0)
            return -1;
    }

    if ((regInformacoes->tamModelo == -2 && regProcurar->tamModelo > 0) || regInformacoes->tamModelo > 0)
    {
        if (regInformacoes->tamModelo == -2 && regProcurar->modelo != NULL)
            return -1;

        else if (regProcurar->tamModelo <= 0){
            return -1;
        }

        else if (strcmp(regProcurar->modelo, regInformacoes->modelo) != 0)
            return -1;
    }

    return 0;
}


char* addBarraZero(char* string, int size){
    char *s = malloc(size + 1);

    for (int i = 0; i < size; i++){
        s[i] = string[i];
    }

    s[size] = '\0';

    return s;
}

int printarStructRegistro(Registro_t *registroPrintar)
{
    // printa uma struct do registro, tanto faz o tipo
    if (registroPrintar == NULL)
        return -1;

    if (registroPrintar->tamMarca > 0){
        char *string = addBarraZero(registroPrintar->marca, registroPrintar->tamMarca);
        printf("MARCA DO VEICULO: %s\n", string);
        free(string);
    }
    else
        printf("MARCA DO VEICULO: NAO PREENCHIDO\n");

    if (registroPrintar->tamModelo > 0){
        char *string = addBarraZero(registroPrintar->modelo, registroPrintar->tamModelo);
        printf("MODELO DO VEICULO: %s\n", string);
        free(string);
    }
    else
        printf("MODELO DO VEICULO: NAO PREENCHIDO\n");

    if (registroPrintar->ano >= 0)
        printf("ANO DE FABRICACAO: %d\n", registroPrintar->ano);
    else
        printf("ANO DE FABRICACAO: NAO PREENCHIDO\n");

    if (registroPrintar->tamCidade > 0){
        char *string = addBarraZero(registroPrintar->cidade, registroPrintar->tamCidade);
        printf("NOME DA CIDADE: %s\n", string);
        free(string);
    }
    else
        printf("NOME DA CIDADE: NAO PREENCHIDO\n");

    if (registroPrintar->qtt >= 0)
        printf("QUANTIDADE DE VEICULOS: %d\n", registroPrintar->qtt);
    else
        printf("QUANTIDADE DE VEICULOS: NAO PREENCHIDO\n");

    printf("\n");

    return 1;
}

int addBinarioRegistro(Registro_t *registroNovo, FILE *arqBin, int tipoArq)
{
    if (tipoArq != 1 && tipoArq != 2)
        return -1;

    int tamanho = 97 - sizeof(char) * 3 - sizeof(int) * 4; // tamanho total registroTipo1 menos os campos fixos

    if (tipoArq == 2) // caso seja tipo 2, entao calculamos o tamanho do registro antes para poder inserir no comeco do mesmo
    {
        tamanho = 0;
        tamanho += sizeof(int) * 3 + sizeof(long int) + 2; // tamanho dos campos fixos de registros tipo 2

        // calcular o tamanho final do registro de tipo com 2, com os campos nao fixos
        if (registroNovo->tamCidade > 0)
            tamanho += registroNovo->tamCidade + (int)sizeof(int) + (int)sizeof(char);
        if (registroNovo->tamMarca > 0)
            tamanho += registroNovo->tamMarca + (int)sizeof(int) + (int)sizeof(char);
        if (registroNovo->tamModelo > 0)
            tamanho += registroNovo->tamModelo + (int)sizeof(int) + (int)sizeof(char);
    }

    char removido = '0';
    fwrite(&removido, sizeof(char), 1, arqBin);

    if (tipoArq == 2) // se for tipo 2 tem o tamanho e o prox tem tamanho de long int
    {
        fwrite(&tamanho, sizeof(int), 1, arqBin);
        long int prox = -1;
        fwrite(&prox, sizeof(long int), 1, arqBin);
    }
    else
    {
        int prox = -1;
        fwrite(&prox, sizeof(int), 1, arqBin);
    }

    // adiciona os campos fixos no arquivo
    fwrite(&registroNovo->id, sizeof(int), 1, arqBin);
    fwrite(&registroNovo->ano, sizeof(int), 1, arqBin);
    fwrite(&registroNovo->qtt, sizeof(int), 1, arqBin);
    fwrite(&registroNovo->sigla, sizeof(char) * 2, 1, arqBin);

    // adiciona os campos variaveis no arquivo
    if (registroNovo->tamCidade > 0 && (tamanho > 5 || tipoArq == 2)) // verifica se o campo existe e se tem espaco para colocar o mesmo, caso seja o 2 nao importa o espaco
    {
        tamanho -= sizeof(int) + sizeof(char); // somente e utilizado pelo tipo 1, para o tipo 2 ele nao fara nada com isso
        int tamCidade = registroNovo->tamCidade;
        if (tamanho >= registroNovo->tamCidade)
            fwrite(&tamCidade, sizeof(int), 1, arqBin);
        else
            fwrite(&tamanho, sizeof(int), 1, arqBin);

        registroNovo->codC5 = '0';
        fwrite(&registroNovo->codC5, sizeof(char), 1, arqBin);

        if (tamanho >= registroNovo->tamCidade)
            fwrite(registroNovo->cidade, tamCidade, 1, arqBin);
        else
            fwrite(registroNovo->cidade, tamanho, 1, arqBin);

        tamanho -= registroNovo->tamCidade;
    }

    if (registroNovo->tamMarca > 0 && (tamanho > 5 || tipoArq == 2))
    {
        tamanho -= sizeof(int) + sizeof(char);

        int tamMarca = registroNovo->tamMarca;

        if (tamanho >= registroNovo->tamMarca)
            fwrite(&tamMarca, sizeof(int), 1, arqBin);
        else
            fwrite(&tamanho, sizeof(int), 1, arqBin);

        registroNovo->codC6 = '1';
        fwrite(&registroNovo->codC6, sizeof(char), 1, arqBin);

        if (tamanho >= registroNovo->tamMarca)
            fwrite(registroNovo->marca, tamMarca, 1, arqBin);
        else
            fwrite(registroNovo->marca, tamanho, 1, arqBin);

        tamanho -= registroNovo->tamMarca;
    }

    if (registroNovo->tamModelo > 0 && (tamanho > 5 || tipoArq == 2))
    {
        tamanho -= sizeof(int) + sizeof(char);

        int tamModelo = registroNovo->tamModelo;
        if (tamanho >= registroNovo->tamModelo)
            fwrite(&tamModelo, sizeof(int), 1, arqBin);
        else
            fwrite(&tamanho, sizeof(int), 1, arqBin);

        registroNovo->codC7 = '2';
        fwrite(&registroNovo->codC7, sizeof(char), 1, arqBin);

        if (tamanho >= registroNovo->tamModelo)
            fwrite(registroNovo->modelo, tamModelo, 1, arqBin);
        else
            fwrite(registroNovo->modelo, tamanho, 1, arqBin);

        tamanho -= registroNovo->tamModelo;
    }

    while (tamanho > 0 && tipoArq == 1) // caso seja o tipo 1 coloca a quantidade necessaria de lixo no final do registro
    {
        char lixo = '$';
        fwrite(&lixo, sizeof(char), 1, arqBin);
        tamanho--;
    }

    return 0;
}

int mudarStatus(FILE *arqBin, int novoStatus)
{
    if (novoStatus != 1 && novoStatus != 0)
        return -1;

    fseek(arqBin, 0, SEEK_SET);
    char status = (char)(novoStatus + 48);
    fwrite(&status, sizeof(char), 1, arqBin);
    return 1;
}

int montarCabecalho(FILE *saida, int tipoArq)
{
    if (!saida)
        return -1;

    if (tipoArq != 1 && tipoArq != 2)
        return -1;

    char status = '0';
    fwrite(&status, sizeof(char), 1, saida);

    if (tipoArq == 1) // caso seja tipo 2 o tamanho do topo fica como long int
    {
        int topo = -1;
        fwrite(&topo, sizeof(int), 1, saida);
    }
    else
    {
        long int topo = -1;
        fwrite(&topo, sizeof(long int), 1, saida);
    }

    char descricao[40] = "LISTAGEM DA FROTA DOS VEICULOS NO BRASIL";

    fwrite(descricao, sizeof(char) * 40, 1, saida);

    char desC1[22] = "CODIGO IDENTIFICADOR: ";
    fwrite(desC1, sizeof(char) * 22, 1, saida);

    char desC2[19] = "ANO DE FABRICACAO: ";
    fwrite(desC2, sizeof(char) * 19, 1, saida);

    char desC3[24] = "QUANTIDADE DE VEICULOS: ";
    fwrite(desC3, sizeof(char) * 24, 1, saida);

    char desC4[8] = "ESTADO: ";
    fwrite(desC4, sizeof(char) * 8, 1, saida);

    char codC5 = '0';
    fwrite(&codC5, sizeof(char), 1, saida);
    char desC5[16] = "NOME DA CIDADE: ";
    fwrite(desC5, sizeof(char) * 16, 1, saida);

    char codC6 = '1';
    fwrite(&codC6, sizeof(char), 1, saida);
    char desC6[18] = "MARCA DO VEICULO: ";
    fwrite(desC6, sizeof(char) * 18, 1, saida);

    char codC7 = '2';
    fwrite(&codC7, sizeof(char), 1, saida);
    char desC7[19] = "MODELO DO VEICULO: ";
    fwrite(desC7, sizeof(char) * 19, 1, saida);

    if (tipoArq == 1) // caso seja tipo 2 o proxRRN fica long int
    {
        int proxRRN = 0;
        fwrite(&proxRRN, sizeof(int), 1, saida);
    }
    else
    {
        long int proxByteOffset = 0;
        fwrite(&proxByteOffset, sizeof(long int), 1, saida);
    }

    int nroRegRem = 0;
    fwrite(&nroRegRem, sizeof(int), 1, saida);

    return 1;
}

void endLine(FILE *fp)
{
    getc(fp);
    getc(fp);
}

int lerCampo(FILE *fp, char **string)
{
    char c;

    int n = 1;

    do
    {
        c = getc(fp);

        if (c == '\r' || c == EOF || c == ',')
        {
            if (c == '\r')
            {
                c = getc(fp); // \n
            }
            break;
        }

        else
        {
            *string = realloc(*string, sizeof(char) * n);
            (*string)[n - 1] = c;
            n++;
        }

    } while (c != '\n' && c != '\r' && c != EOF && c != ',');

    if (n <= 1)
    {
        return 0;
    }

    else
    {
        return n - 1;
    }
}

//organiza o vetor de indices para ser inserido em ordem
int ordenarVetorIndices(RegistroIndice_t **regs, int qtdRegs)
{
    if (!regs)
        return -1;

    quickSort(regs, 0, qtdRegs);
    return 1;
}

//quicksort que organiza de acordo com o indice
void quickSort(RegistroIndice_t **regs, int min, int max)
{
    int i, j, x;

    RegistroIndice_t *aux = malloc(sizeof(RegistroIndice_t));
    inicializarRegistroIndice(aux);

    i = min;
    j = max;
    x = regs[(min + max) / 2]->indice;

    while (i <= j)
    {
        while (regs[i]->indice < x && i < max)
        {
            i++;
        }
        while (regs[j]->indice > x && j > min)
        {
            j--;
        }
        if (i <= j)
        {

            aux->indice = regs[i]->indice;
            aux->rrn = regs[i]->rrn;
            aux->byteOffset = regs[i]->byteOffset;

            regs[i]->indice = regs[j]->indice;
            regs[i]->rrn = regs[j]->rrn;
            regs[i]->byteOffset = regs[j]->byteOffset;

            regs[j]->indice = aux->indice;
            regs[j]->rrn = aux->rrn;
            regs[j]->byteOffset = aux->byteOffset;

            inicializarRegistroIndice(aux);
            i++;
            j--;
        }
    }

    if (j > min)
    {
        quickSort(regs, min, j);
    }
    if (i < max)
    {
        quickSort(regs, i, max);
    }
    if(aux)
        free(aux);
}

//quicksort que organiza de acordo com o byteOffset
void quickSortByteOffset2(RegistroIndice_t **regs, int min, int max)
{
    int i, j, x;

    RegistroIndice_t *aux = malloc(sizeof(RegistroIndice_t));
    inicializarRegistroIndice(aux);

    i = min;
    j = max;
    x = regs[(min + max) / 2]->byteOffset;

    while (i <= j)
    {
        while (regs[i]->byteOffset < x && i < max)
        {
            i++;
        }
        while (regs[j]->byteOffset > x && j > min)
        {
            j--;
        }
        if (i <= j)
        {

            aux->indice = regs[i]->indice;
            aux->rrn = regs[i]->rrn;
            aux->byteOffset = regs[i]->byteOffset;

            regs[i]->indice = regs[j]->indice;
            regs[i]->rrn = regs[j]->rrn;
            regs[i]->byteOffset = regs[j]->byteOffset;

            regs[j]->indice = aux->indice;
            regs[j]->rrn = aux->rrn;
            regs[j]->byteOffset = aux->byteOffset;

            if(aux)
                free(aux);
            i++;
            j--;
        }
    }

    if (j > min)
    {
        quickSort(regs, min, j);
    }
    if (i < max)
    {
        quickSort(regs, i, max);
    }
}

//quicksort que organiza de acordo com o tamanho
void quickSortByteOffset(RegistroIndice_t **regs,int *tamanhoRegs ,int min, int max)
{
    int i, j, x;

    RegistroIndice_t *aux = malloc(sizeof(RegistroIndice_t));
    inicializarRegistroIndice(aux);
    int auxInt;

    i = min;
    j = max;
    x = tamanhoRegs[(min + max) / 2];

    while (i <= j)
    {
        while (tamanhoRegs[i] < x && i < max)
        {
            i++;
        }
        while (tamanhoRegs[j] > x && j > min)
        {
            j--;
        }
        if (i <= j)
        {

            aux->indice = regs[i]->indice;
            aux->rrn = regs[i]->rrn;
            aux->byteOffset = regs[i]->byteOffset;
            auxInt = tamanhoRegs[i];

            regs[i]->indice = regs[j]->indice;
            regs[i]->rrn = regs[j]->rrn;
            regs[i]->byteOffset = regs[j]->byteOffset;
            tamanhoRegs[i] = tamanhoRegs[j];

            regs[j]->indice = aux->indice;
            regs[j]->rrn = aux->rrn;
            regs[j]->byteOffset = aux->byteOffset;
            tamanhoRegs[j] = auxInt;

            if(aux)
                free(aux);
            i++;
            j--;
        }
    }

    if (j > min)
    {
        quickSortByteOffset(regs, tamanhoRegs,min, j);
    }
    if (i < max)
    {
        quickSortByteOffset(regs, tamanhoRegs,i, max);
    }
}

//insere indices no arquivo de indices
int inserirIndice(FILE *arqIndice, RegistroIndice_t **regs, int qtdRegs, int tipoArq)
{
    if (!arqIndice || !regs || tipoArq > 2 || tipoArq < 1)
        return -1;

    char status = '0';
    fwrite(&status, sizeof(char), 1, arqIndice);

    int regInseridos = 0;
    while (regInseridos < qtdRegs)
    {
        fwrite(&regs[regInseridos]->indice, sizeof(int), 1, arqIndice);

        if (tipoArq == 1)
            fwrite(&regs[regInseridos]->rrn, sizeof(int), 1, arqIndice);
        else
            fwrite(&regs[regInseridos]->byteOffset, sizeof(long int), 1, arqIndice);

        regInseridos++;
    }

    mudarStatus(arqIndice, 1);

    return 1;
}

void copiarSemBarraZero(char *string, char *dest)
{
    int n = strlen(string);

    for (int i = 0; i < n; i++)
    {
        dest[i] = string[i];
    }
}

//faz uma pesquisa binaria do arquivo de indices procurando pelo id passado, e caso nao encontre, retorna -1
long int pesquisaBinariaIndice(FILE *arqInd, int id, RegistroIndice_t *result, int tipoArq)
{
    if(!arqInd || id <= 0 || tipoArq > 2 || tipoArq < 1){
        return -1;
    }

    fseek(arqInd,0,SEEK_SET);
    char status;
    fread(&status, sizeof(char), 1, arqInd);

    if(status != '1')
        return -1;

    if(!result)
    {
        result = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(result);
    }
    int tamRegInd = 0;

    if (tipoArq == 1)
        tamRegInd = sizeof(int) * 2;
    else
        tamRegInd = 8 + sizeof(int);

    fseek(arqInd, 0, SEEK_END);
    long int tamArqInd = ftell(arqInd);
    long int qtdRegsInd = (tamArqInd - 1) / tamRegInd;

    long int max = qtdRegsInd;
    long int min = 0;
    long int posicaoRegistro = -1;
    RegistroIndice_t  *regAtual = malloc(sizeof(RegistroIndice_t));
    inicializarRegistroIndice(regAtual);

    long int arqIndPonteiro = 0;
    fseek(arqInd, 0, SEEK_SET);

    while (min <= max) {//pesquisa binaria
        int m = min + (max - min) / 2;
        long int byteOffset = (m * tamRegInd) + 1;
        pegarRegistroIndice(arqInd,regAtual,byteOffset,tipoArq);

        if (regAtual->indice == id)
        {
            posicaoRegistro =  m;
            break;
        }

        if (regAtual->indice < id)
            min = m + 1;
        else
            max = m - 1;

        inicializarRegistroIndice(regAtual);
    }
    result->byteOffset = regAtual->byteOffset;
    result->rrn = regAtual->rrn;
    result->indice = regAtual->indice;

    free(regAtual);


    return posicaoRegistro;
}

//pega o registro de indice que esta presente no byteOffset, o registro estara no result
int pegarRegistroIndice(FILE *arqInd,RegistroIndice_t *result,long int byteOffset, int tipoArq)
{
    if(!arqInd || byteOffset < 0 || tipoArq > 2 || tipoArq < 1)
        return -1;

    if(!result)
    {
        result = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(result);
    }

    fseek(arqInd,byteOffset - ftell(arqInd),SEEK_CUR);
    fread(&result->indice, sizeof(int), 1, arqInd);

    if(tipoArq == 1)
        fread(&result->rrn, sizeof(int), 1, arqInd);
    else
        fread(&result->byteOffset, sizeof(long int), 1, arqInd);

    return 1;
}

//recebe um registro antigo e as atualizacoes que queira fazer e retorna no registro antigo o novo registro com as mudancas
int alterarReg(Registro_t *regAlterar, Registro_t *regNovosDados)
{
    if(!regAlterar || !regNovosDados)
        return -1;

    if(regNovosDados->prox >= 0)
        regAlterar->prox = regNovosDados->prox;
    if(regNovosDados->id >= 0)
        regAlterar->id = regNovosDados->id;
    if(regNovosDados->ano >= 0)
        regAlterar->ano = regNovosDados->ano;
    if(regNovosDados->qtt >= 0)
        regAlterar->qtt = regNovosDados->qtt;
    if(regNovosDados->sigla[0] != '$')
    {
        regAlterar->sigla[0] = regNovosDados->sigla[0];
        regAlterar->sigla[1] = regNovosDados->sigla[1];
    }
    if(regNovosDados->tamCidade > 0)
    {
        if (regAlterar->tamCidade > 0)
            free(regAlterar->cidade);

        if (strcmp(regNovosDados->cidade, "NULO") == 0){
            regAlterar->tamCidade = 0;
        }

        else{
            regAlterar->tamCidade = regNovosDados->tamCidade;
            regAlterar->cidade = malloc(sizeof (char) * (regAlterar->tamCidade+1));
            strcpy(regAlterar->cidade,regNovosDados->cidade);
        }
    }
    if(regNovosDados->tamMarca > 0)
    {
        if (regAlterar->tamMarca > 0)
            free(regAlterar->marca);

        if (strcmp(regNovosDados->marca, "NULO") == 0){
            regAlterar->tamMarca = 0;
        }

        else{
            regAlterar->tamMarca = regNovosDados->tamMarca;
            regAlterar->marca = malloc(sizeof (char) * (regAlterar->tamMarca+1));
            strcpy(regAlterar->marca,regNovosDados->marca);
        }
    }
    if(regNovosDados->tamModelo > 0)
    {
        if (regAlterar->tamModelo > 0)
            free(regAlterar->modelo);

        if (strcmp(regNovosDados->modelo, "NULO") == 0){
            regAlterar->tamModelo = 0;
        }

        else{
            regAlterar->tamModelo = regNovosDados->tamModelo;
            regAlterar->modelo = malloc(sizeof (char) * (regAlterar->tamModelo+1));
            strcpy(regAlterar->modelo,regNovosDados->modelo);
        }
    }
    return 1;
}

//insere um indice novo no arquivo de indices, funciona para ambos os tipos
int atualizarIndices(FILE *arqInd, RegistroIndice_t *regAnt, RegistroIndice_t *regNovo, int tipoArq)
{
    if(!arqInd || !regNovo)
        return -1;

    char status;
    fseek(arqInd,0,SEEK_SET);
    fread(&status, sizeof(char), 1, arqInd);
    if(status != '1')
        return -1;

    mudarStatus(arqInd,0);

    int tamanhoIndice = 0;
    if(tipoArq == 1)
        tamanhoIndice = sizeof(int) * 2;
    else
        tamanhoIndice = sizeof(int) + sizeof(long int);

    fseek(arqInd,0,SEEK_END);
    long int tamahoArqInd = ftell(arqInd);
    long int qtdIndices = (tamahoArqInd - 1)/tamanhoIndice;
    fseek(arqInd,1,SEEK_SET);

    RegistroIndice_t **indices = malloc(sizeof(RegistroIndice_t *)*qtdIndices);

    int indexIndice = 0;

    for (int i = 0; i < qtdIndices; ++i) {
        indices[i] = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(indices[i]);
        fread(&indices[i]->indice, sizeof(int), 1, arqInd);

        if(indices[i]->indice != regAnt->indice)
        {
            if(tipoArq == 1)
                fread(&indices[i]->rrn, sizeof(int), 1, arqInd);
            else
                fread(&indices[i]->byteOffset, sizeof(long int), 1, arqInd);
        }
    }

    fseek(arqInd,1,SEEK_SET);
    int adicionadoNovoIndice = 0;
    for (int i = 0; i < qtdIndices; ++i) {
        if(indices[i]->indice < regNovo->indice)
        {
            if(indices[i]->indice >= 0)
                fseek(arqInd,tamanhoIndice,SEEK_CUR);

        }else{
            if(adicionadoNovoIndice == 0)
            {
                fwrite(&regNovo->indice,sizeof(int),1,arqInd);
                if(tipoArq == 1)
                    fwrite(&regNovo->rrn,sizeof(int),1,arqInd);
                else
                    fwrite(&regNovo->byteOffset,sizeof(long int),1,arqInd);

                adicionadoNovoIndice++;
            }

            fwrite(&indices[i]->indice,sizeof(int),1,arqInd);
            if(tipoArq == 1)
                fwrite(&indices[i]->rrn,sizeof(int),1,arqInd);
            else
                fwrite(&indices[i]->byteOffset,sizeof(long int),1,arqInd);
        }
    }

    if(adicionadoNovoIndice == 0)
    {
        fwrite(&regNovo->indice,sizeof(int),1,arqInd);
        if(tipoArq == 1)
            fwrite(&regNovo->rrn,sizeof(int),1,arqInd);
        else
            fwrite(&regNovo->byteOffset,sizeof(long int),1,arqInd);
        adicionadoNovoIndice++;
    }

    mudarStatus(arqInd,1);

    for (int i = 0; i < qtdIndices; ++i) {
        free(indices[i]);
    }

    free(indices);

    return 1;
}

int inserirRegistroPosAtual(FILE *arqBin, Registro_t *regInserir, int tamRegAnt,int tipoArq)//insere um registro na posicao atual do ponteiro do arquivo, nao verifica nada somente insere
{
    if (!arqBin)
        return -1;

    regInserir->removido = '0';
    fwrite(&regInserir->removido, sizeof(char), 1, arqBin);

    int tamanhoRestante = tamRegAnt - (3 * sizeof(char)) - (sizeof(int) * 4);

    if (tipoArq == 1) {
        int prox = -1;
        fwrite(&prox, sizeof(int), 1, arqBin);
    } else {
        fwrite(&tamRegAnt, sizeof(int), 1, arqBin);
        long int prox = -1;
        fwrite(&prox, sizeof(long int), 1, arqBin);
        tamanhoRestante -= sizeof(long int);
        tamanhoRestante += 5;
    }

    fwrite(&regInserir->id, sizeof(int), 1, arqBin);
    fwrite(&regInserir->ano, sizeof(int), 1, arqBin);
    fwrite(&regInserir->qtt, sizeof(int), 1, arqBin);
    fwrite(&regInserir->sigla, sizeof(char), 2, arqBin);

    if (regInserir->tamCidade > 0 && (tamanhoRestante > 5 || tipoArq == 2)) {
        regInserir->codC5 = '0';
        fwrite(&regInserir->tamCidade, sizeof(int), 1, arqBin);
        fwrite(&regInserir->codC5, sizeof(char), 1, arqBin);
        tamanhoRestante -= (sizeof(char) + sizeof(int));

        if (tamanhoRestante >= regInserir->tamCidade)
            fwrite(regInserir->cidade, 1, regInserir->tamCidade, arqBin);
        else
            fwrite(regInserir->cidade, 1, tamanhoRestante, arqBin);
        tamanhoRestante -= regInserir->tamCidade;
    }

    if (regInserir->tamMarca > 0 && (tamanhoRestante > 5 || tipoArq == 2)) {
        regInserir->codC6 = '1';
        fwrite(&regInserir->tamMarca, sizeof(int), 1, arqBin);
        fwrite(&regInserir->codC6, sizeof(char), 1, arqBin);
        tamanhoRestante -= (sizeof(char) + sizeof(int));

        if (tamanhoRestante >= regInserir->tamMarca)
            fwrite(regInserir->marca, 1, regInserir->tamMarca, arqBin);
        else
            fwrite(regInserir->marca, 1, tamanhoRestante, arqBin);
        tamanhoRestante -= regInserir->tamMarca;

    }

    if (regInserir->tamModelo > 0 && (tamanhoRestante > 5 || tipoArq == 2)) {
        regInserir->codC7 = '2';
        fwrite(&regInserir->tamModelo, sizeof(int), 1, arqBin);
        fwrite(&regInserir->codC7, sizeof(char), 1, arqBin);
        tamanhoRestante -= (sizeof(char) + sizeof(int));

        if (tamanhoRestante >= regInserir->tamModelo)
            fwrite(regInserir->modelo, 1, regInserir->tamModelo, arqBin);
        else
            fwrite(regInserir->modelo, 1, tamanhoRestante, arqBin);
        tamanhoRestante -= regInserir->tamModelo;

    }

    while (tamanhoRestante > 0){
        char *lixo = "$";
        fwrite(lixo, 1, 1, arqBin);
        tamanhoRestante--;
    }

    return 1;
}

//pega o registro apartir do byte passado
int pegarRegistroDadosByteOffset(FILE *arqBin, long int byteOffset,Registro_t *reg, int tipoArq)
{
    if(!arqBin || !reg)
        return -1;

    fseek(arqBin,0,SEEK_SET);

    char status = '0';
    fread(&status,1,1,arqBin);
    if(status != '1')
        return -1;

    fseek(arqBin,byteOffset,SEEK_SET);
    int result = 1;

    if(tipoArq == 1)
        result = pegarProximoRegTipo1(arqBin,reg);
    else
        result = pegarProxRegistroTipo2(arqBin,reg);

    if(result < 0)
        return -1;

    return 1;

}