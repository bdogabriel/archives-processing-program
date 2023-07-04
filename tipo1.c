// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700

// esse .c contem as funcoes especificas do tipo1
#include "tipo1.h"
#include <stdio.h>
#include <stdlib.h>
#include "arvoreB.h"


int pesquisarComCamposTipo1(char *arqPesq, char **campos, char **valores, int qtdCampos)
{
    if (!campos || !valores || !arqPesq || qtdCampos <= 0)
        return -1;

    Registro_t *parametrosPesquisa = malloc(sizeof(Registro_t));
    inicializarRegistro(parametrosPesquisa);

    montarRegistroPesquisa(parametrosPesquisa, campos, valores, qtdCampos);

    FILE *arqBin = fopen(arqPesq, "rb");

    if (arqBin == NULL)
    {
        return -1;
    }

    int result = printarResultadosPesquisaTipo1(arqBin, parametrosPesquisa);

    free(parametrosPesquisa);

    fclose(arqBin);

    return result;
}

int printarResultadosPesquisaTipo1(FILE *arqBin, Registro_t *regPesquisar)
{
    if (!arqBin)
        return -1;

    char status;

    fread(&status, 1, 1, arqBin);

    if (status == '0')
    {
        return -1;
    }

    fseek(arqBin, 174, SEEK_SET);
    int maxRRN = 0;
    fread(&maxRRN, sizeof(int), 1, arqBin);

    if (maxRRN <= 0)
    {
        printf("Registro inexistente.");
        return 0;
    }

    fseek(arqBin, sizeof(int), SEEK_CUR);
    for (int i = 0; i < maxRRN - 1; ++i)
    { // percorre o arquivo inteiro pesquisando pelo resutado correto e printa se achar
        Registro_t *regProcurar = malloc(sizeof(Registro_t));
        inicializarRegistro(regProcurar);
        if (pegarProximoRegTipo1(arqBin, regProcurar) >= 0)
        {
            if (compararPesquisaRegistro(regProcurar, regPesquisar) == 0)
                printarStructRegistro(regProcurar);
        }
        desalocarRegistro(regProcurar);
    }

    return 0;
}

int printarArquivoBinInteiroTipo1(char *arq)
{
    FILE *arqBin = fopen(arq, "rb");
    if (!arqBin)
        return -1;

    char status;

    fread(&status, 1, 1, arqBin);

    if (status == '0')
    {
        return -1;
    }

    fseek(arqBin, 174, SEEK_SET);
    int maxRRN = 0;
    fread(&maxRRN, sizeof(int), 1, arqBin);

    if (maxRRN <= 0)
    {
        printf("Registro inexistente.");
        return 0;
    }

    fseek(arqBin, sizeof(int), SEEK_CUR);
    for (int i = 0; i < maxRRN; ++i)
    { // percorre o arquivo inteiro e printa todos os registros
        Registro_t *reg = malloc(sizeof(Registro_t));
        inicializarRegistro(reg);
        if (pegarProximoRegTipo1(arqBin, reg) >= 0)
        {
            printarStructRegistro(reg);
        }

        desalocarRegistro(reg);
    }

    fclose(arqBin);

    return 0;
}

int pegarProximoRegTipo1(FILE *arqBin, Registro_t *reg)
{
    if (reg == NULL){
        //printf("ruim 1\n");
        return -1;
    }

    if (!arqBin){
        //printf("ruim 2\n");
        return -1;
    }

    int tamanhoRestante = 97;
    char removido = '1';
    fread(&removido, sizeof(char), 1, arqBin);

    if (removido != '0')
    {
        fseek(arqBin, 96, SEEK_CUR);
        //fread(&removido, sizeof(char), 1, arqBin);
        //printf("ruim 3\n");
        return -1;
    }

    fseek(arqBin, sizeof(int), SEEK_CUR);

    fread(&reg->id, sizeof(int), 1, arqBin);
    fread(&reg->ano, sizeof(int), 1, arqBin);
    fread(&reg->qtt, sizeof(int), 1, arqBin);
    fread(&reg->sigla, sizeof(char) * 2, 1, arqBin);

    tamanhoRestante -= sizeof(char) + sizeof(int) * 4 + sizeof(char) * 2;

    reg->tamCidade = 0;
    reg->tamModelo = 0;
    reg->tamMarca = 0;

    while (1) // le os campos de tamanho variado
    {
        int tamanho = 0;
        fread(&tamanho, sizeof(int), 1, arqBin);

        char tipo = ' ';
        fread(&tipo, 1, 1, arqBin);
        tamanhoRestante -= 1 + sizeof(int);

        if (tamanho > 97 || tamanho < 0)
            goto fimWhile;

        switch (tipo)
        {
        case '0':
            reg->tamCidade = tamanho;
            if (tamanho > 0){
                reg->cidade = realloc(reg->cidade, reg->tamCidade + 1);
                fread(reg->cidade, reg->tamCidade, 1, arqBin);
                tamanhoRestante -= reg->tamCidade;
                reg->cidade[tamanho] = '\0';
            }
            else if (reg->cidade != NULL){
                free(reg->cidade);
            }
            break;
        case '1':
            reg->tamMarca = tamanho;
            if (tamanho > 0){
                reg->marca = realloc(reg->marca, reg->tamMarca + 1);
                fread(reg->marca, reg->tamMarca, 1, arqBin);
                tamanhoRestante -= reg->tamMarca;
                reg->marca[tamanho] = '\0';
            }

            else if (reg->marca != NULL){
                free(reg->marca);
            }

            break;
        case '2':
            reg->tamModelo = tamanho;
            if(tamanho > 0){
                reg->modelo = realloc(reg->modelo, reg->tamModelo + 1);
                fread(reg->modelo, reg->tamModelo, 1, arqBin);
                tamanhoRestante -= reg->tamModelo;
                reg->modelo[tamanho] = '\0';
            }

            else if (reg->modelo != NULL){
                free(reg->modelo);
            }

            break;
        default:
            goto fimWhile;
        }
    }
fimWhile:;

    fseek(arqBin, tamanhoRestante, SEEK_CUR); // coloca o ponteiro no proximo registro

    return 1;
}

int pesquisaRRN(int rrn, char *arqBin)
{
    if (!arqBin)
        return -1;

    FILE *arqPesq = fopen(arqBin, "rb");

    if (!arqPesq)
        return -1;

    char status;

    fread(&status, sizeof(char), 1, arqPesq);

    if (status == '0')
    {
        return -1;
    }

    fseek(arqPesq, 174, SEEK_SET);
    int maxRRN = 0;
    fread(&maxRRN, sizeof(int), 1, arqPesq);

    if (rrn >= maxRRN)
    {
        printf("Registro inexistente.");
        fclose(arqPesq);
        return 0;
    }

    int posInicialReg = (int)(sizeof(int) + (rrn * 97)); // pula o cabecalho e adiciona a posicao do rrn
    fseek(arqPesq, posInicialReg, SEEK_CUR);

    // pega o registro e o printa
    Registro_t *reg = malloc(sizeof(Registro_t));
    inicializarRegistro(reg);
    int result = pegarProximoRegTipo1(arqPesq, reg);

    fclose(arqPesq);

    if (result < 0)
    {
        printf("Registro inexistente.");
    }

    else
    {
        printarStructRegistro(reg);
    }

    desalocarRegistro(reg);

    return 0;
}

int atualizarRRNCabecalho(FILE *arqBin, int novoRRN) // atualiza o prox rrn disponivel e o coloca no cabecalho
{
    if (novoRRN < 0)
        return -1;
    fseek(arqBin, 174, SEEK_SET);
    fwrite(&novoRRN, sizeof(int), 1, arqBin);
    return 1;
}

int montarVetorIndicesTipo1(FILE *arqBin,FILE *arqInd, RegistroIndice_t **vetorIndices, int *qtdIndices)
{
    if (!arqBin)
        return -1;

    char status;
    fread(&status, sizeof(char), 1, arqBin);
    if (status != '1')
        return -1;

    fseek(arqBin, 174, SEEK_SET);

    int proxRRN;
    fread(&proxRRN, sizeof(int), 1, arqBin);
    fseek(arqBin, 4, SEEK_CUR);

    *qtdIndices = 0;

    int rrnAtual = 0; // acompanha em qual rrn esta no momento

    while (proxRRN > rrnAtual) // enquanto nao ter lido todos os rrns
    {
        int bytesRestantes = 97; // acompanha quantos bytes tera que pular no final para o proximo registro

        char removido;
        fread(&removido, sizeof(char), 1, arqBin);
        bytesRestantes--;

        if (removido != '1') // caso nao esteja removido
        {
            // pula o prox
            fseek(arqBin, 4, SEEK_CUR);
            bytesRestantes -= 4;

            // le o id
            int id;
            fread(&id, sizeof(int), 1, arqBin);
            bytesRestantes -= 4;

            // aloca e inicia o registro novo
            (*qtdIndices)++;
            vetorIndices = realloc(vetorIndices,sizeof(RegistroIndice_t *) * (*qtdIndices));
            vetorIndices[(*qtdIndices) - 1] = malloc(sizeof(RegistroIndice_t));
            inicializarRegistroIndice(vetorIndices[(*qtdIndices) - 1]);

            // coloca os valores no novo registro
            vetorIndices[(*qtdIndices) - 1]->indice = id;
            vetorIndices[(*qtdIndices) - 1]->rrn = rrnAtual;
        }
        fseek(arqBin, bytesRestantes, SEEK_CUR); // pula para o prox registro, possivelmente ira dar erro
        rrnAtual++;
    }

    ordenarVetorIndices(vetorIndices, (*qtdIndices)-1);               // ordena o vetor
    inserirIndice(arqInd, vetorIndices, *qtdIndices, 1); // insere o vetor no arquivo de indices

    for (int i = 0; i < *qtdIndices; ++i)
    {
        free(vetorIndices[i]);
    }
    free(vetorIndices);

    return 1;
}


int alterarValoresRegistroTipo1(FILE *arqDados, Registro_t *novosDados, int rrn)
{
    if(!arqDados || !novosDados || rrn < 0)
        return -1;

    fseek(arqDados,0,SEEK_SET);

    char status;
    fread(&status,sizeof(char),1,arqDados);
    if(status != '1')
        return -1;

    mudarStatus(arqDados,0);

    fseek(arqDados,(rrn * 97) + 182, SEEK_SET);

    Registro_t *regAntigo = malloc(sizeof(Registro_t));
    inicializarRegistro(regAntigo);

    pegarProximoRegTipo1(arqDados,regAntigo);
    alterarReg(regAntigo,novosDados);

    fseek(arqDados,-97, SEEK_CUR);
    inserirRegistroPosAtual(arqDados,regAntigo,97,1);

    desalocarRegistro(regAntigo);

    mudarStatus(arqDados,1);
    fseek(arqDados,((rrn + 1) * 97) + 182, SEEK_SET);

    return 1;
}

//insere um registro do tipo 1 no arquivo
int inserirRegistroTipo1(FILE* arqDados, FILE* arqInd, Registro_t* reg, int tipoIndice){
    fseek(arqDados, 0, SEEK_SET);

    char status;
    fread(&status, 1, 1, arqDados);

    if (status != '1'){
        return -1;
    }

    mudarStatus(arqDados, 0);

    int topo;
    fread(&topo, 4, 1, arqDados);

    RegistroIndice_t *regInd = malloc(sizeof(RegistroIndice_t));

    if (topo == -1){//caso nao exista nenhum espaco de registros excluidos, entao ele ira inserir no final
        fseek(arqDados, 0, SEEK_END);
        inserirRegistroPosAtual(arqDados, reg, 97, 1);
        fseek(arqDados, 174, SEEK_SET);

        int proxRRN;
        fread(&proxRRN, 4, 1, arqDados);
        proxRRN++;
        fseek(arqDados, 174, SEEK_SET);
        fwrite(&proxRRN, 4, 1, arqDados);

        inicializarRegistroIndice(regInd);
        regInd->rrn = proxRRN - 1;
    }
    else{//caso exista espaco, ele insere no topo e atualiza a pilha
        // acessa o rrn do topo
        fseek(arqDados, (topo * 97) + 182, SEEK_SET);
        int proxTopo;
        char removido;
        fread(&removido, 1, 1, arqDados);

        if (removido == '0'){
            return -1;
        }

        regInd->rrn = topo;

        // pega o proxTopo
        fread(&proxTopo, 4, 1, arqDados);

        // volta pro comeco do registro
        fseek(arqDados, -5, SEEK_CUR);
        // insere o novo registro
        inserirRegistroPosAtual(arqDados, reg, 97, 1);
        
        // atualiza o topo
        fseek(arqDados, 1, SEEK_SET);
        fwrite(&proxTopo, 4, 1, arqDados);
        fseek(arqDados, 178, SEEK_SET);

        // atualiza o nroRem
        int nroRem = 0;
        fread(&nroRem, 4, 1, arqDados);
        nroRem--;
        fseek(arqDados, -4, SEEK_CUR);
        fwrite(&nroRem, 4, 1, arqDados);
    }

    if(tipoIndice == 1)
    {
        regInd->indice = reg->id;
        RegistroIndice_t *vazio = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(vazio);

        atualizarIndices(arqInd, vazio, regInd, 1);

        free(vazio);

    }else
    {
        inserirValoresArvB(arqInd, reg->id,regInd->rrn,-1,1);
    }

    mudarStatus(arqDados, 1);
    free(regInd);

    return 0;
}

//funcao para atualizar um indice no arquivo de indices
int atualizarIndicesTipo1(FILE *arqInd, RegistroIndice_t *regAnt, RegistroIndice_t *regNovo, int tipoArq)
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
        }else
        {
            indices[i]->indice = regNovo->indice;
            if(tipoArq == 1)
                fread(&indices[i]->rrn, sizeof(int), 1, arqInd);
            else
                fread(&indices[i]->byteOffset, sizeof(long int), 1, arqInd);
        }
    }

    quickSort(indices,0,qtdIndices-1);
    fseek(arqInd,1,SEEK_SET);
    int adicionadoNovoIndice = 0;
    for (int i = 0; i < qtdIndices; ++i) {

        fwrite(&indices[i]->indice,sizeof(int),1,arqInd);
        if(tipoArq == 1)
            fwrite(&indices[i]->rrn,sizeof(int),1,arqInd);
        else
            fwrite(&indices[i]->byteOffset,sizeof(long int),1,arqInd);

        adicionadoNovoIndice++;

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