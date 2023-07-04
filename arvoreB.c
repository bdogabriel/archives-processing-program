// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700

#include "stdio.h"
#include "stdlib.h"
#include "arvoreB.h"
#include "auxiliar.h"
#include "tipo1.h"
#include "tipo2.h"

int recursaoIdArvoreB(FILE *arvB, int idPesq, int *rrnResult, long int *byteOffsetResult, int tipoArq);
long int buscaIdArvoreB(FILE *arvB, int idPesq, int tipoArq);
int lerProximoRegArvB(FILE *arqIndArvB, RegIndiceArvoreB_t *reg, int tipoArq);
int escreverProxRegArvB(FILE *arqIndArvB, RegIndiceArvoreB_t *reg, int tipoArq);
int splitRegistros(FILE *arqIndArvB, RegIndiceArvoreB_t *regSplit, RegIndiceArvoreB_t *regReceber, RegIndiceArvoreB_t *regDir, int tipoArq);
int inserirNovoValorReg(RegIndiceArvoreB_t *reg, int idInserir, int rrnInserir, long int byteOffsetInserir, int tipoArq);
int funcaoRecursivaInserir(FILE *arqIndArvB,int idInserir, int rrnInserir, long int byteOffsetInserir,RegIndiceArvoreB_t *regAnt, int tipoArq);
int escreverCabecalhoArvB(FILE *arvB,int tipoArq);


void inicializarRegistroArvoreB(RegIndiceArvoreB_t *reg)//inicializa o registro com valores iniciais
{
    if(reg != NULL)
    {
        reg->tipoNo = '3';
        reg->nroChaves = -1;
        reg->endReg = -1;
        for (int i = 0; i < 4; ++i) {
            reg->byteOffset[i] = -1;
            reg->rrn[i] = -1;
            reg->chaves[i] = -1;
            reg->prox[i] = -1;
        }
        reg->prox[4] = -1;
    }
}

//procura pelo valor correto na arvore b e printa esse valor de acordo com o registro no arquivo de valores
int printBuscaIdArvoreB(char *nomeArqBin, char *nomeArqIndArvB, int id, int tipoArq)
{
    if(!nomeArqBin || !nomeArqIndArvB || id < 0 || tipoArq > 2 || tipoArq < 1)
        return -1;

    FILE *arqBin = fopen(nomeArqBin, "rb");
    FILE *arqBinIndArvB = fopen(nomeArqIndArvB, "rb");
    if(!arqBin || !arqBinIndArvB)
        return -1;

    long int resultPesq = buscaIdArvoreB(arqBinIndArvB,id,tipoArq);//faz a pesquisa do rrn ou byteOffset

    if(resultPesq < 0)//caso nao encontre
    {
        printf("Registro inexistente.\n");
        return 0;
    }

    Registro_t *reg = malloc(sizeof(Registro_t));
    inicializarRegistro(reg);

    long int byteOffset = resultPesq * 97 + 182;
    if(tipoArq == 2)
        byteOffset = resultPesq;

    if (pegarRegistroDadosByteOffset(arqBin,byteOffset,reg,tipoArq) < 0)//pega o registro com os valores a serem printados
        return -1;

    printarStructRegistro(reg);//printa os dados

    desalocarRegistro(reg);
    fclose(arqBin);
    fclose(arqBinIndArvB);
    return 1;
}

long int buscaIdArvoreB(FILE *arvB, int idPesq, int tipoArq)//busca pelo id e retorna a posicao no arquivo de dados
{
    //verifica o status do aquivo de indices
    fseek(arvB,0,SEEK_SET);
    char status = '0';
    fread(&status,1,1,arvB);
    if(status != '1')
        return -1;

    //tamanho da pagina dependendo do tipo do arq
    int tamPag = 45;
    if(tipoArq == 2)
        tamPag = 57;

    //le a raiz e vai para a pagina do mesmo
    int raiz = -1;
    fread(&raiz,4,1,arvB);
    long int endRaiz = (raiz+1) * tamPag;
    fseek(arvB,endRaiz,SEEK_SET);

    //faz a pesquisa recursiva pelo id
    int rrnResult = -1;
    long int byteOffsetResult = -1;
    recursaoIdArvoreB(arvB,idPesq,&rrnResult,&byteOffsetResult,tipoArq);

    //retorna o resultado encontrado
    if(rrnResult == -1 && byteOffsetResult == -1)
        return -1;

    if(tipoArq == 1)
        return rrnResult;

    return byteOffsetResult;
}

//pesquisa recursiva pelo id, no comeco da funcao o ponteiro tem de estar no comeco de um no
int recursaoIdArvoreB(FILE *arvB, int idPesq, int *rrnResult, long int *byteOffsetResult, int tipoArq)
{
    RegIndiceArvoreB_t *reg = malloc(sizeof(RegIndiceArvoreB_t));
    inicializarRegistroArvoreB(reg);
    lerProximoRegArvB(arvB,reg,tipoArq);

    if(tipoArq < 1 || tipoArq > 2){
        free(reg);
        return -1;
    }

    if(reg->nroChaves < 1){
        free(reg);
        return -1;
    }

    int posCorreta = 0;
    //procura pela posicao correta do proximo passo da pesquisa
    while(posCorreta < 4 && (reg->nroChaves) >= posCorreta && idPesq > reg->chaves[posCorreta] && reg->chaves[posCorreta] != -1)
        posCorreta++;

    if(reg->chaves[posCorreta] == idPesq)//caso tenha encontrado
    {
        if(tipoArq == 1)
            *rrnResult = reg->rrn[posCorreta];
        else
            *byteOffsetResult = reg->byteOffset[posCorreta];
        
        free(reg);
        return 1;
    }

    if(reg->tipoNo == '2'){//caso tenha chego em uma folha e nao tenha encontrado, entao quer dizer que nao existe esse valor
        free(reg);
        return -1;
    }

    //vai para o proximo registro
    long int proxByteOffset = (reg->prox[posCorreta]+1) * 45;
    if(tipoArq == 2)
        proxByteOffset = (reg->prox[posCorreta]+1) * 57;

    //vai para o proximo registro
    long int byteAtual = 0;
    byteAtual = ftell(arvB);
    fseek(arvB,proxByteOffset - byteAtual,SEEK_CUR);

    free(reg);

    return recursaoIdArvoreB(arvB,idPesq,rrnResult,byteOffsetResult,tipoArq);
}

int lerProximoRegArvB(FILE *arqIndArvB, RegIndiceArvoreB_t *reg, int tipoArq)//le o proximo registro da arvoreB apartir do ponteiro atual do aquivo
{
    if(!arqIndArvB || !reg)
        return -1;

    inicializarRegistroArvoreB(reg);
    reg->endReg = ftell(arqIndArvB);
    fread(&reg->tipoNo,1,1,arqIndArvB);
    fread(&reg->nroChaves,4,1,arqIndArvB);

    for (int i = 0; i < 3; ++i) {
        fread(&reg->chaves[i],4,1,arqIndArvB);
        if(tipoArq == 1)
            fread(&reg->rrn[i],4,1,arqIndArvB);
        else
            fread(&reg->byteOffset[i],8,1,arqIndArvB);
    }

    for (int i = 0; i < 4; ++i) {
        fread(&reg->prox[i],4,1,arqIndArvB);
    }
    return 1;

}

int escreverProxRegArvB(FILE *arqIndArvB, RegIndiceArvoreB_t *reg, int tipoArq)//escre o registro na arvoreB apartir do ponteiro atual do arquivo
{
    if(!arqIndArvB || !reg)
        return -1;

    fwrite(&reg->tipoNo,1,1,arqIndArvB);
    fwrite(&reg->nroChaves,4,1,arqIndArvB);

    for (int i = 0; i < 3; ++i) {
        fwrite(&reg->chaves[i],4,1,arqIndArvB);
        if(tipoArq == 1)
            fwrite(&reg->rrn[i],4,1,arqIndArvB);
        else
            fwrite(&reg->byteOffset[i],8,1,arqIndArvB);
    }

    for (int i = 0; i < 4; ++i) {
        fwrite(&reg->prox[i],4,1,arqIndArvB);
    }
    return 1;
}

//procura recusivamente a posicao correta e insere o novo valor
int funcaoRecursivaInserir(FILE *arqIndArvB,int idInserir, int rrnInserir, long int byteOffsetInserir,RegIndiceArvoreB_t *regAnt, int tipoArq)
{
    RegIndiceArvoreB_t *regNovo = malloc(sizeof(RegIndiceArvoreB_t));
    lerProximoRegArvB(arqIndArvB,regNovo,tipoArq);

    int posCorreta = 0;

    while(posCorreta < 4 && (regNovo->nroChaves) >= posCorreta && idInserir > regNovo->chaves[posCorreta] && regNovo->chaves[posCorreta] != -1)
        posCorreta++;

    int tamanhoNo = 45;
    if (tipoArq == 2)
        tamanhoNo = 57;

    long int proxNo = (regNovo->prox[posCorreta] + 1) * tamanhoNo;
    fseek(arqIndArvB, proxNo - ftell(arqIndArvB), SEEK_CUR);

    int qtdNosAtual = regNovo->nroChaves;

    if(regNovo->prox[0] != -1)//caso nao tenha encontrado a posicao correta para inserir ainda, ele chama novamente a funcao
    {
        funcaoRecursivaInserir(arqIndArvB,idInserir,rrnInserir,byteOffsetInserir,regNovo,tipoArq);
    }else//caso tenha chego em uma folha e ja possa inserir
    {
        if(regNovo->nroChaves <= 2) {//caso tenha especo no registro para somente inserir o valor novo
            inserirNovoValorReg(regNovo,idInserir,rrnInserir,byteOffsetInserir,tipoArq);
            fseek(arqIndArvB, regNovo->endReg - ftell(arqIndArvB), SEEK_CUR);
            escreverProxRegArvB(arqIndArvB, regNovo, tipoArq);
            free(regNovo);
            return 0;
        }

        //caso nao tenha espaco, entao sobre um valor para o registro acima
        RegIndiceArvoreB_t *regDir = malloc(sizeof(RegIndiceArvoreB_t));
        inicializarRegistroArvoreB(regDir);

        inserirNovoValorReg(regNovo,idInserir,rrnInserir,byteOffsetInserir,tipoArq);
        int estado = -1;
        estado = splitRegistros(arqIndArvB,regNovo,regAnt,regDir,tipoArq);//faz o split do registro em 2 e sobe o valor

        free(regNovo);
        free(regDir);

        if(estado < 0)
            return -1;

        return estado;//caso o estado seja diferente de 0 entao quer dizer que tem uma nova raiz
    }

    if(regNovo->tipoNo == '0' && regNovo->nroChaves > 3)//caso a raiz tenha mais do que o numero maximo, entao tem que criar uma nova
    {
        RegIndiceArvoreB_t *regNovaRaiz = malloc(sizeof(RegIndiceArvoreB_t));
        inicializarRegistroArvoreB(regNovaRaiz);
        RegIndiceArvoreB_t *regDir = malloc(sizeof(RegIndiceArvoreB_t));
        inicializarRegistroArvoreB(regDir);

        int estado = 0;
        estado = splitRegistros(arqIndArvB,regNovo,regNovaRaiz,regDir,tipoArq);

        free(regNovo);
        free(regDir);
        free(regNovaRaiz);

        if(estado < 0)
            return -1;

        return estado;
    }

    //caso o registro abaixo tenha colocado um novo valor no atual e o atual ultrapasse o numero maximo, entao realiza o split no atual
    if(regNovo->nroChaves > 3)
    {
        RegIndiceArvoreB_t *regDir = malloc(sizeof(RegIndiceArvoreB_t));
        inicializarRegistroArvoreB(regDir);

        splitRegistros(arqIndArvB,regNovo,regAnt,regDir,tipoArq);

        free(regNovo);
        free(regDir);
        return 0;
    }

    if(qtdNosAtual != regNovo->nroChaves)//caso tenha ocorrido alguma alteracao no registro, entao atualiza o valor dele no arquivo
    {
        fseek(arqIndArvB, regNovo->endReg - ftell(arqIndArvB), SEEK_CUR);
        escreverProxRegArvB(arqIndArvB, regNovo, tipoArq);
    }

    free(regNovo);
    return 0;
}

//insere um valor no registro, sem mudar os valores dos proximos registros(prox)
int inserirNovoValorReg(RegIndiceArvoreB_t *reg, int idInserir, int rrnInserir, long int byteOffsetInserir, int tipoArq)
{
    if(!reg || reg->nroChaves > 3)
        return -1;

    //procura o lugar correto para inserir
    int posCorreta = 0;
    while(posCorreta < 4 && (reg->nroChaves) >= posCorreta && idInserir > reg->chaves[posCorreta] && reg->chaves[posCorreta] != -1)
        posCorreta++;

    if(reg->chaves[posCorreta] == idInserir)//caso o valor ja exista no registro
        return -1;

    reg->nroChaves++;

    int aux = reg->chaves[posCorreta];
    int rrnAux = reg->rrn[posCorreta];
    long int byteOffsetAux = reg->byteOffset[posCorreta];

    reg->chaves[posCorreta] = idInserir;
    if (tipoArq == 1)
        reg->rrn[posCorreta] = rrnInserir;
    else
        reg->byteOffset[posCorreta] = byteOffsetInserir;

    for (int i = posCorreta + 1; i < reg->nroChaves; ++i) {//insere o valor do id no local correto e arruma o restante
        int aux2 = reg->chaves[i];
        int rrnAux2 = reg->rrn[i];
        long int byteOffsetAux2 = reg->byteOffset[i];

        reg->chaves[i] = aux;
        if (tipoArq == 1)
            reg->rrn[i] = rrnAux;
        else
            reg->byteOffset[i] = byteOffsetAux;

        aux = aux2;
        rrnAux = rrnAux2;
        byteOffsetAux = byteOffsetAux2;
    }
    return posCorreta;//retorna a posicao no qual foi inserido o valor
}

//faz o split de um registro em 2 e passa um valor para cima
int splitRegistros(FILE *arqIndArvB, RegIndiceArvoreB_t *regSplit, RegIndiceArvoreB_t *regReceber, RegIndiceArvoreB_t *regDir, int tipoArq)
{
    if(!regDir || !regReceber || !regSplit || !arqIndArvB)
        return -1;

    if(regSplit->nroChaves != 4 || regReceber->nroChaves > 3)
        return -1;

    inicializarRegistroArvoreB(regDir);

    if(tipoArq == 1 || tipoArq == 2)
    {
        //cria o valor registro da direta e coloca os valores nele
        regDir->nroChaves = 1;

        regDir->chaves[0] = regSplit->chaves[3];
        regDir->rrn[0] = regSplit->rrn[3];
        regDir->byteOffset[0] = regSplit->byteOffset[3];
        regDir->prox[0] = regSplit->prox[3];
        regDir->prox[1] = regSplit->prox[4];

        regSplit->chaves[3] = -1;
        regSplit->rrn[3] = -1;
        regSplit->byteOffset[3] = -1;
        regSplit->prox[3] = -1;
        regSplit->prox[4] = -1;

        if(regDir->prox[0] == -1)
            regDir->tipoNo = '2';
        else
            regDir->tipoNo = '1';

        //escreve o registro da direita
        fseek(arqIndArvB,0,SEEK_END);
        regDir->endReg = ftell(arqIndArvB);
        escreverProxRegArvB(arqIndArvB,regDir,tipoArq);

        int tamanhoReg = 45;
        if (tipoArq == 2)
            tamanhoReg = 57;

        //insere o valor no registro acima
        int posCorreta = -1;
        posCorreta = inserirNovoValorReg(regReceber,regSplit->chaves[2],regSplit->rrn[2],regSplit->byteOffset[2],tipoArq);

        if(posCorreta < 0)
            return -1;

        //insere os enderecos dos registro da esquerda e o do original
        int auxProx,auxProx2;
        auxProx = regReceber->prox[posCorreta+1];
        regReceber->prox[posCorreta+1] = ((int)regDir->endReg/tamanhoReg) - 1;
        for (int i = posCorreta+2; i < 5; ++i) {
            if(auxProx == -1)
                i = 5;
            else
            {
                auxProx2 = regReceber->prox[i];
                regReceber->prox[i] = auxProx;
                auxProx = auxProx2;
            }
        }

        regSplit->chaves[2] = -1;
        regSplit->rrn[2] = -1;
        regSplit->byteOffset[2] = -1;
        regSplit->nroChaves = 2;

        if(regReceber->nroChaves == -1 || regReceber->nroChaves == 0 || regSplit->tipoNo == '0')
            if(regSplit->prox[0] == -1)
                regSplit->tipoNo = '2';
            else
                regSplit->tipoNo = '1';

        //atualiza o valor do registro atual no arquivo
        fseek(arqIndArvB, regSplit->endReg - ftell(arqIndArvB),SEEK_CUR);
        escreverProxRegArvB(arqIndArvB,regSplit,tipoArq);

        if(regReceber->nroChaves == -1 || regReceber->nroChaves == 0)//caso o registro ainda nao esteja no arquivo
        {
            regReceber->nroChaves = 1;
            regReceber->tipoNo = '0';
            fseek(arqIndArvB,0,SEEK_END);
            long int valorFim = ftell(arqIndArvB);
            regReceber->prox[0] = ((int)regSplit->endReg/tamanhoReg) - 1;
            regReceber->prox[1] = ((int)regDir->endReg/tamanhoReg) - 1;

            escreverProxRegArvB(arqIndArvB,regReceber,tipoArq);
            return ((int)valorFim/tamanhoReg) - 1;//retorna o endereco
        }
    }
    return 0;
}

//insere um valor na arvore b
int inserirValoresArvB(FILE *arqIndArvB,int idInserir ,int rrnInserir, long int byteOffsetInserir, int tipoArq)
{
    if(!arqIndArvB || (rrnInserir < 0 && byteOffsetInserir < 0) || tipoArq > 2 || tipoArq < 1)
        return -1;

    mudarStatus(arqIndArvB,0);
    fseek(arqIndArvB,1,SEEK_SET);

    int posNoRaiz = -1;
    fread(&posNoRaiz,4,1,arqIndArvB);

    int tamanhoRegInd = 45;
    if(tipoArq == 2)
        tamanhoRegInd = 57;

    RegIndiceArvoreB_t *regAux = malloc(sizeof(RegIndiceArvoreB_t));
    inicializarRegistroArvoreB(regAux);
    int estado = -1;

    if(posNoRaiz >= 0)//caso seja o primeiro valor
    {
        long int posByteOffset = (1 + posNoRaiz) * tamanhoRegInd;
        fseek(arqIndArvB,posByteOffset,SEEK_SET);
        estado = funcaoRecursivaInserir(arqIndArvB,idInserir,rrnInserir,byteOffsetInserir,regAux,tipoArq);
        mudarStatus(arqIndArvB,1);
    }else
    {
        fseek(arqIndArvB,tamanhoRegInd,SEEK_SET);

        regAux->chaves[0] = idInserir;
        if(tipoArq == 1)
            regAux->rrn[0] = rrnInserir;
        else
            regAux->byteOffset[0] = byteOffsetInserir;

        regAux->tipoNo = '0';
        regAux->nroChaves = 1;
        escreverProxRegArvB(arqIndArvB,regAux,tipoArq);

        estado = 0;
        fseek(arqIndArvB,1,SEEK_SET);
        fwrite(&estado,4,1,arqIndArvB);
    }

    if(estado < 0)
        return -1;

    //atualiza os valores do cabecalho
    fseek(arqIndArvB,0,SEEK_END);
    long int proxByteOffset = ftell(arqIndArvB);
    int proxRRN = (int)proxByteOffset/tamanhoRegInd - 1;

    fseek(arqIndArvB,1,SEEK_SET);

    if(estado > 0)
        fwrite(&estado,4,1,arqIndArvB);

    fseek(arqIndArvB,5,SEEK_SET);
    fwrite(&proxRRN,4,1,arqIndArvB);
    fwrite(&proxRRN,4,1,arqIndArvB);

    free(regAux);
    return 1;
}

//funcao chamada para criar o indice de arvore b
int criarIndArvB(char *nomeArqBin, char *nomeArqArvB, int tipoArq)
{
    if(!nomeArqBin || !nomeArqArvB || tipoArq > 2 || tipoArq < 1)
        return -1;

    FILE *arqBin = fopen(nomeArqBin, "rb");

    if (arqBin == NULL){
        return -1;
    }

    FILE *arqArvB = fopen(nomeArqArvB, "wb");
    fclose(arqArvB);
    arqArvB = fopen(nomeArqArvB,"r+b");

    char status = '2';
    fread(&status,1,1,arqBin);
    if(status != '1')
        return -1;

    int proxRRN = -1;
    long int proxByteOffset = -1;

    if(tipoArq == 1)
    {
        fseek(arqBin,174,SEEK_SET);
        fread(&proxRRN,4,1,arqBin);
        escreverCabecalhoArvB(arqArvB,1);
    }
    else
    {
        fseek(arqBin,178,SEEK_SET);
        fread(&proxByteOffset,8,1,arqBin);
        escreverCabecalhoArvB(arqArvB,2);
    }

    fseek(arqBin,4,SEEK_CUR);

    int rrnAtual = 0;
    long int byteOffsetAtual = 190;

    //percore o arquivo e adiciona os valores na arvore b
    while((tipoArq == 1 && proxRRN > rrnAtual) || (tipoArq == 2 && proxByteOffset - 1 > byteOffsetAtual))
    {
        Registro_t *regAux = malloc(sizeof(Registro_t));
        inicializarRegistro(regAux);

        if(tipoArq == 1)
        {
            while (pegarProximoRegTipo1(arqBin,regAux) < 0)
                rrnAtual++;
            inserirValoresArvB(arqArvB,regAux->id,rrnAtual,-1,1);
        }else
        {
            pegarProxRegistroTipo2(arqBin,regAux);
            while (regAux->removido == 0)
                byteOffsetAtual += regAux->tamanhoRegistro + 5;
            inserirValoresArvB(arqArvB,regAux->id,-1,byteOffsetAtual,2);
        }
        if(tipoArq == 1)
            rrnAtual++;
        else
            byteOffsetAtual += regAux->tamanhoRegistro + 5;

        desalocarRegistro(regAux);
    }

    mudarStatus(arqArvB,1);
    fclose(arqBin);
    fclose(arqArvB);
    return 1;
}

//escreve o cabecalho da arvore B
int escreverCabecalhoArvB(FILE *arvB,int tipoArq)
{
    if(!arvB || tipoArq > 2 || tipoArq < 1)
        return -1;

    fseek(arvB,0,SEEK_SET);

    int tamanhoReg = 45;
    if(tipoArq == 2)
        tamanhoReg = 57;

    char status = '0';
    fwrite(&status,1,1,arvB);
    tamanhoReg--;
    int intAux = -1;
    fwrite(&intAux,4,1,arvB);
    tamanhoReg -= 4;
    intAux = 0;
    fwrite(&intAux,4,1,arvB);
    tamanhoReg -= 4;
    fwrite(&intAux,4,1,arvB);
    tamanhoReg -= 4;

    while(tamanhoReg > 0)
    {
        char aux = '$';
        fwrite(&aux,1,1,arvB);
        tamanhoReg--;
    }
    return 1;
}