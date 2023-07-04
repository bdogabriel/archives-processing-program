
// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700

// esse .c contem as funcoes especificas do tipo2
#include "tipo2.h"
#include "arq.h"
#include <stdio.h>
#include <stdlib.h>
#include "arvoreB.h"

long int buscaWorstFitTamanho(FILE *arqBin, int tamanho);
int tamanhoRegistroTipo2(Registro_t *reg);

int printarArquivoBinInteiroTipo2(char *nomeArq)
{
    FILE *fp = fopen(nomeArq, "rb");

    char status;

    if (fp == NULL)
    {
        return -1;
    }

    fread(&status, 1, 1, fp);

    if (status == '0')
    {
        return -1;
    }

    // pula o cabeçalho
    fseek(fp, 169 + sizeof(int) + 2 * sizeof(long int), SEEK_CUR);

    int lido = 1;

    while (lido != 0)
    {
        char rem;

        lido = fread(&rem, 1, 1, fp);

        if (lido == 0)
        {
            break;
        }

        if (rem == '0')
        {
            int tam = 0;
            int tamLido = 0;

            fread(&tam, sizeof(int), 1, fp);
            Registro_t *reg = NULL;

            if (tam < (int)sizeof(Registro_t))
            {
                reg = malloc(sizeof(Registro_t));
            }
            else
            {
                reg = malloc(tam);
            }

            reg->tamanhoRegistro = tam;

            inicializarRegistro(reg);

            // pula o 'prox'
            fseek(fp, sizeof(long int), SEEK_CUR);

            // lendo os campos
            fread(&reg->id, sizeof(int), 1, fp);
            fread(&reg->ano, sizeof(int), 1, fp);
            fread(&reg->qtt, sizeof(int), 1, fp);

            fread(reg->sigla, 2, 1, fp);

            tamLido += sizeof(long int) + sizeof(int) * 3 + 2;

            while (tamLido != tam)
            {
                char codigo = 'a';
                int tamCampo = 0;

                fread(&tamCampo, sizeof(int), 1, fp);
                fread(&codigo, 1, 1, fp);

                switch (codigo)
                {
                case '0':
                    reg->tamCidade = tamCampo;
                    reg->cidade = malloc(tamCampo + 1);
                    fread(reg->cidade, tamCampo, 1, fp);
                    reg->cidade[reg->tamCidade] = '\0';
                    break;

                case '1':
                    reg->tamMarca = tamCampo;
                    reg->marca = malloc(tamCampo + 1);
                    fread(reg->marca, tamCampo, 1, fp);
                    reg->marca[reg->tamMarca] = '\0';
                    break;

                default:
                    reg->tamModelo = tamCampo;
                    reg->modelo = malloc(tamCampo + 1);
                    fread(reg->modelo, tamCampo, 1, fp);
                    reg->modelo[reg->tamModelo] = '\0';
                    break;
                }

                tamLido += tamCampo + sizeof(int) + 1;
            }

            if (reg != NULL)
            {
                printarStructRegistro(reg);
                desalocarRegistro(reg);
            }
        }

        else
        {
            int tam;
            fread(&tam, sizeof(int), 1, fp);

            fseek(fp, tam, SEEK_CUR);
        }
    }

    fclose(fp);

    return 0;
}

// 'n' eh o numero de parametros para a busca
int pesquisarComCamposTipo2(char *arqPesq, char **campos, char **valores, int qtdCampos)
{
    int encontrados = 0;

    FILE *fp = fopen(arqPesq, "rb");

    if (fp == NULL)
    {
        return -1;
    }

    Registro_t *regPesq = malloc(sizeof(Registro_t));
    inicializarRegistro(regPesq);
    montarRegistroPesquisa(regPesq, campos, valores, qtdCampos);

    char status;

    fread(&status, 1, 1, fp);

    if (status == '0')
    {
        return -1;
    }

    // pula o cabeçalho
    fseek(fp, 177, SEEK_CUR);

    int totalBytes;
    fread(&totalBytes, sizeof(long int), 1, fp);
    fseek(fp, 4, SEEK_CUR);

    int lido = 1;

    while (lido != 0)
    {
        char rem;

        lido = fread(&rem, sizeof(char), 1, fp);

        if (lido == 0)
        {
            break;
        }

        if (rem == '0')
        {
            int tam = 0;
            int tamLido = 0;

            fread(&tam, sizeof(int), 1, fp);
            Registro_t *reg = malloc(sizeof(Registro_t));

            reg->tamanhoRegistro = tam;

            inicializarRegistro(reg);

            // pula o 'prox'
            fseek(fp, sizeof(long int), SEEK_CUR);

            // lendo os campos
            fread(&reg->id, sizeof(int), 1, fp);
            fread(&reg->ano, sizeof(int), 1, fp);
            fread(&reg->qtt, sizeof(int), 1, fp);

            fread(reg->sigla, 2, 1, fp);

            tamLido += sizeof(long int) + sizeof(int) * 3 + 2;

            while (tamLido != tam)
            {
                char codigo = 'a';
                int tamCampo = 0;

                fread(&tamCampo, sizeof(int), 1, fp);
                fread(&codigo, 1, 1, fp);

                switch (codigo)
                {
                case '0':
                    reg->tamCidade = tamCampo;
                    reg->cidade = malloc(tamCampo + 1);
                    fread(reg->cidade, tamCampo, 1, fp);
                    reg->cidade[reg->tamCidade] = '\0';
                    break;

                case '1':
                    reg->tamMarca = tamCampo;
                    reg->marca = malloc(tamCampo + 1);
                    fread(reg->marca, tamCampo, 1, fp);
                    reg->marca[reg->tamMarca] = '\0';
                    break;

                default:
                    reg->tamModelo = tamCampo;
                    reg->modelo = malloc(tamCampo + 1);
                    fread(reg->modelo, tamCampo, 1, fp);
                    reg->modelo[reg->tamModelo] = '\0';
                    break;
                }

                tamLido += tamCampo + sizeof(int) + 1;
            }

            if (reg != NULL && compararPesquisaRegistro(reg, regPesq) == 0)
            {
                printarStructRegistro(reg);
                encontrados++;
            }

            if (reg != NULL)
            {
                desalocarRegistro(reg);
            }
        }
        else
        {
            int tam;
            fread(&tam, sizeof(int), 1, fp);
            fseek(fp, tam, SEEK_CUR);
        }
    }
    free(regPesq);
    fclose(fp);

    if (encontrados == 0)
    {
        printf("Registro Inexistente.");
    }

    return 0;
}

int atualizarByteOffsetCabecalho(FILE *arqBin, long int byteOffset) // atualiza o proximo byteoffset disponivel e o coloca no cabecalho
{
    if (byteOffset < 0)
        return -1;

    fseek(arqBin, 178, SEEK_SET);
    fwrite(&byteOffset, sizeof(long int), 1, arqBin);
    return 1;
}

int montarVetorIndicesTipo2(FILE *arqBin, FILE *arqInd,RegistroIndice_t **vetorIndices, int *qtdIndices)
{
    if (!arqBin)
        return -1;

    char status;
    fread(&status, sizeof(char), 1, arqBin);

    if (status != '1')
        return -1;

    fseek(arqBin, 178, SEEK_SET); // pula o cabecalho

    long int proxByteOffset;
    fread(&proxByteOffset, sizeof(long int), 1, arqBin);
    fseek(arqBin, 4, SEEK_CUR);

    *qtdIndices = 0;
    long int byteAtual = 190; // acompanha aonde o ponteiro esta no arquivo

    while (proxByteOffset - 1 > byteAtual) // enquanto nao chegar ao fim do arquivo
    {
        char removido;
        fread(&removido, sizeof(char), 1, arqBin);

        int tamanhoRegistro = 0;
        fread(&tamanhoRegistro, sizeof(int), 1, arqBin);

        int proxReg = tamanhoRegistro; // calcula a distancia do proximo registro
        if (removido != '1')                                          // caso nao esteja removido
        {
            fseek(arqBin, sizeof(long int), SEEK_CUR); // pula o prox

            int id;
            fread(&id, sizeof(int), 1, arqBin); // le o id do registro

            // aloca e inicia o registro novo
            (*qtdIndices)++;
            vetorIndices = realloc(vetorIndices, sizeof(RegistroIndice_t *) * *qtdIndices);
            vetorIndices[(*qtdIndices) - 1] = malloc(sizeof(RegistroIndice_t));
            inicializarRegistroIndice(vetorIndices[(*qtdIndices) - 1]);

            // coloca os valores no novo registro
            vetorIndices[(*qtdIndices) - 1]->indice = id;
            vetorIndices[(*qtdIndices) - 1]->byteOffset = byteAtual;


            proxReg -= sizeof(long int) + sizeof(int); // retira os campos ja lido
        }
        byteAtual += tamanhoRegistro + sizeof(int) + sizeof(char);              // adiciona o registro na quantidade de bytes lidos
        fseek(arqBin, proxReg, SEEK_CUR); // pula para o prox registro
    }

    ordenarVetorIndices(vetorIndices, (*qtdIndices)-1);               // ordena o vetor
    inserirIndice(arqInd, vetorIndices, *qtdIndices, 2); // insere o vetor no arquivo de indices

    for (int i = 0; i < *qtdIndices; ++i)
    {
        free(vetorIndices[i]);
    }
    free(vetorIndices);

    return 1;
}

int updateListaTopoTipo2(FILE *arqBin, long int novoByteOffset, long int topo, int tamReg)
{
    if(novoByteOffset <= 0)
        return -1;

    fseek(arqBin,0,SEEK_SET);
    if(topo == -1)//caso nao exista a lista ainda, entao comeca a lista com o registro excluido
    {
        fseek(arqBin,1,SEEK_CUR);
        fwrite(&novoByteOffset, sizeof(long int), 1, arqBin);
        return 1;
    }
    int tamRemovidoAtual = 0;
    long int anteriorByteOffset = -1;
    long int atualByteOffset = -1;
    long int proxByteOffset = topo;

    //verifica o caso do novo registro excluido ser o novo topo e caso seja, entao atualiza os valores e retorna
    atualByteOffset = proxByteOffset;
    fseek(arqBin,proxByteOffset + sizeof(char),SEEK_SET);//vai ate o registro excluido
    fread(&tamRemovidoAtual,sizeof(int),1,arqBin);//le o tamanho do registro excluido
    fread(&proxByteOffset,sizeof(long int),1,arqBin);//le o proximo registro excluido da lista
    if(tamRemovidoAtual < tamReg)
    {
        //atualiza o topo e o registro que foi excluido
        fseek(arqBin,sizeof(char),SEEK_SET);
        fwrite(&novoByteOffset, sizeof(long int), 1, arqBin);
        fseek(arqBin,novoByteOffset + sizeof(int) - sizeof(long int),SEEK_CUR);
        fwrite(&atualByteOffset, sizeof(long int), 1, arqBin);
        return 1;
    }

    //procura pelo lugar correto na lista
    while(tamReg < tamRemovidoAtual && atualByteOffset != -1)
    {
        anteriorByteOffset = atualByteOffset;
        atualByteOffset = proxByteOffset;

        if(proxByteOffset != -1)
        {
            fseek(arqBin, proxByteOffset + 1, SEEK_SET);
            fread(&tamRemovidoAtual, sizeof(int), 1, arqBin);
            fread(&proxByteOffset, sizeof(long int), 1, arqBin);
        }

    }
    //insere o registro excluido e atualiza os valores
    fseek(arqBin,anteriorByteOffset + sizeof(char) + sizeof (int),SEEK_SET);
    fwrite(&novoByteOffset, sizeof(long int), 1, arqBin);
    fseek(arqBin,novoByteOffset + sizeof(char) + sizeof (int),SEEK_CUR);
    fwrite(&atualByteOffset, sizeof(long int), 1, arqBin);

    return 1;

}

int pegarProxRegistroTipo2(FILE *arqReg, Registro_t *reg)
{
    if(!arqReg)
        return -1;

    if(!reg)
    {
        reg = malloc(sizeof(Registro_t));
        inicializarRegistro(reg);
    }

    fread(&reg->removido, sizeof(char), 1, arqReg);

    fread(&reg->tamanhoRegistro, sizeof(int), 1, arqReg);
    fseek(arqReg,8,SEEK_CUR);
    fread(&reg->id, sizeof(int), 1, arqReg);
    fread(&reg->ano, sizeof(int), 1, arqReg);
    fread(&reg->qtt, sizeof(int), 1, arqReg);
    fread(&reg->sigla[0], sizeof(char), 1, arqReg);
    fread(&reg->sigla[1], sizeof(char), 1, arqReg);

    int tamLido =  sizeof(char) * 2 + 8 + sizeof(int) * 3;
    int qtdCamposLidos = 0;

    while((tamLido + 5) <= reg->tamanhoRegistro && qtdCamposLidos < 3)
    {
        long int j = ftell(arqReg);
        int tamProxCampo;
        fread(&tamProxCampo, sizeof(int), 1, arqReg);
        char codProxCampo;
        fread(&codProxCampo, sizeof(char), 1, arqReg);
        tamLido += sizeof(int) + sizeof(char);

        if(codProxCampo == '0' && (tamLido + tamProxCampo) <= reg->tamanhoRegistro )
        {
            reg->tamCidade = tamProxCampo;
            reg->cidade = malloc(sizeof(char) * tamProxCampo+1);
            fread(reg->cidade, sizeof(char)*tamProxCampo, 1, arqReg);
            reg->cidade[tamProxCampo] = '\0';
            tamLido += reg->tamCidade;
            qtdCamposLidos++;
        }else if(codProxCampo == '1' && (tamLido + tamProxCampo) <= reg->tamanhoRegistro )
        {
            reg->tamMarca = tamProxCampo;
            reg->marca = malloc(sizeof(char) * tamProxCampo+1);
            fread(reg->marca, sizeof(char)*tamProxCampo, 1, arqReg);
            reg->marca[tamProxCampo] = '\0';
            tamLido += reg->tamMarca;
            qtdCamposLidos++;

        } else if(codProxCampo == '2' && (tamLido + tamProxCampo) <= reg->tamanhoRegistro )
        {
            reg->tamModelo = tamProxCampo;
            reg->modelo = malloc(sizeof(char) * tamProxCampo+1);
            fread(reg->modelo, sizeof(char)*tamProxCampo, 1, arqReg);
            reg->modelo[tamProxCampo] = '\0';
            tamLido += reg->tamModelo;
            qtdCamposLidos++;
        }else
            break;
    }
    int byteSobrou = reg->tamanhoRegistro-tamLido;
    if(byteSobrou > 0)
        fseek(arqReg,(reg->tamanhoRegistro-tamLido),SEEK_CUR);
    if(reg->removido != '0')
        return -1;

    return 1;
}

int alterarValoresRegistroTipo2(FILE *arqDados,FILE *arqInd ,Registro_t *novosDados, long int byteOffset)
{
    if(!arqDados || !novosDados || byteOffset <= 0)
        return -1;

    fseek(arqDados, 0 ,SEEK_SET);
    char status;
    fread(&status,sizeof(char),1,arqDados);
    if(status == '0')
        return -1;


    Registro_t *regAntigo = malloc(sizeof(Registro_t));
    inicializarRegistro(regAntigo);

    mudarStatus(arqDados,0);
    fseek(arqDados, byteOffset + sizeof(char),SEEK_SET);

    int tamRegAnt = 0;
    fread(&tamRegAnt,sizeof(int),1,arqDados);

    fseek(arqDados, -5,SEEK_CUR);

    pegarProxRegistroTipo2(arqDados,regAntigo);
    RegistroIndice_t *indAntigo = malloc(sizeof(RegistroIndice_t));

    inicializarRegistroIndice(indAntigo);
    indAntigo->indice = regAntigo->id;
    indAntigo->byteOffset = byteOffset;

    alterarReg(regAntigo,novosDados);

    int tamanhoRegAtt = sizeof(int) * 4 + sizeof(char) * 3 + sizeof(long int);

    if(regAntigo->tamCidade > 0)
        tamanhoRegAtt += regAntigo->tamCidade + sizeof(int) + sizeof(char);

    if(regAntigo->tamMarca > 0)
        tamanhoRegAtt += regAntigo->tamMarca + sizeof(int) + sizeof(char);

    if(regAntigo->tamModelo > 0)
        tamanhoRegAtt += regAntigo->tamModelo + sizeof(int) + sizeof(char);



    if(tamRegAnt >= tamanhoRegAtt)
    {
        fseek(arqDados, -1 * tamRegAnt,SEEK_CUR);
        inserirRegistroPosAtual(arqDados,regAntigo,tamRegAnt,2);
        if(novosDados->id > 0)
        {
            RegistroIndice_t *indNovo = malloc(sizeof(RegistroIndice_t));
            inicializarRegistroIndice(indNovo);
            indNovo->indice = novosDados->id;
            indNovo->byteOffset = byteOffset;
            atualizarIndices(arqInd,indAntigo,indNovo,2);
            free(indNovo);
        }

    }else
    {
        //excluirRegistroTipo2ByteOffset(arqDados,byteOffset,-1);
        long int byteOffsetInserir;
        byteOffsetInserir = buscaWorstFitTamanho(arqDados, tamanhoRegAtt);
        inserirRegistroTipo2(arqDados,arqInd, regAntigo,tamanhoRegAtt,byteOffsetInserir,1);
    }

    desalocarRegistro(regAntigo);
    free(regAntigo);
    free(indAntigo);

    return 1;
}

int inserirRegistroTipo2(FILE *arqBin, FILE *arqBinInd, Registro_t *regInserir, int tamRegAnt,long int byteOffset, int tipoInd)
{
    if(!arqBin || !regInserir || tamRegAnt <= 0)
        return -1;

    fseek(arqBin,0,SEEK_SET);

    char status;
    fread(&status,sizeof(char),1,arqBin);
    if(status != '1')
        return -1;

    mudarStatus(arqBin,0);
    if(byteOffset < 0)
    {
        fseek(arqBin,178,SEEK_SET);
        fread(&byteOffset,sizeof(long int),1,arqBin);
        tamRegAnt = regInserir->tamanhoRegistro - 5;
    }else
    {
        fseek(arqBin,byteOffset + sizeof(char) + sizeof(int),SEEK_SET);
        long int novoTopo;
        fread(&novoTopo,sizeof(long int),1,arqBin);

        fseek(arqBin,1,SEEK_SET);
        fwrite(&novoTopo,sizeof(long int),1,arqBin);

        fseek(arqBin,186,SEEK_SET);
        int nroRegRem;
        fread(&nroRegRem,sizeof(int),1,arqBin);

        nroRegRem--;
        fseek(arqBin,-4,SEEK_CUR);
        fwrite(&nroRegRem,sizeof(int),1,arqBin);
    }

    fseek(arqBin,byteOffset,SEEK_SET);
    inserirRegistroPosAtual(arqBin,regInserir,tamRegAnt,2);
    mudarStatus(arqBin,1);

    if(tipoInd == 1)
    {
        RegistroIndice_t *regAnt = malloc(sizeof(RegistroIndice_t));
        RegistroIndice_t *regNovo = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(regAnt);
        inicializarRegistroIndice(regNovo);
        regNovo->indice = regInserir->id;
        regNovo->byteOffset = byteOffset;

        atualizarIndices(arqBinInd,regAnt,regNovo,2);

        free(regNovo);
        free(regAnt);
    }else
    {
        inserirValoresArvB(arqBinInd,regInserir->id,-1,byteOffset,2);
    }

    return 1;
}

long int buscaWorstFitTamanho(FILE *arqBin, int tamanho)
{
    if(!arqBin || tamanho <= 0)
        return -1;

    fseek(arqBin,0,SEEK_SET);
    char status;
    fread(&status,sizeof(char),1,arqBin);
    if(status == '0')
        return -1;

    fseek(arqBin,1,SEEK_SET);


    long int byteOffsetAtual = 0;
    fread(&byteOffsetAtual,sizeof(long int),1,arqBin);
    if(byteOffsetAtual <= 0)
        return -1;

    fseek(arqBin,byteOffsetAtual + sizeof(char),SEEK_CUR);

    int tamanhoRegAtual;
    fread(&tamanhoRegAtual,sizeof(int),1,arqBin);
    if(tamanhoRegAtual >= tamanho)
    {
        return byteOffsetAtual;
    }else
            return -1;

}


int inserirNovosRegTipo2(char *strArqDados, char* strArqInd,int qtdNovosReg, int tipoInd){
    if(!strArqDados || !strArqInd)
        return -1;

    FILE *arqDados = fopen(strArqDados,"r+b");
    FILE *arqInd = fopen(strArqInd,"r+b");

    for (int i = 0; i < qtdNovosReg; ++i) {
        Registro_t *novoReg = lerRegistro();
        int tamReg = tamanhoRegistroTipo2(novoReg);
        novoReg->tamanhoRegistro = tamReg;
        long int byteOffset = buscaWorstFitTamanho(arqDados,tamReg);
        int tamRegAnt;
        fseek(arqDados, 1 + byteOffset, SEEK_SET);
        fread(&tamRegAnt, sizeof(int), 1, arqDados);

        if (tamRegAnt < tamReg){
            inserirRegistroTipo2(arqDados, arqInd, novoReg, tamReg, -1,tipoInd);
            fseek(arqDados, 0, SEEK_END);
            long int proxByteOffset = ftell(arqDados);
            fseek(arqDados, 178, SEEK_SET);
            fwrite(&proxByteOffset, sizeof(long int), 1, arqDados);
        }
        else if(tamReg > 0)
            inserirRegistroTipo2(arqDados,arqInd,novoReg,tamRegAnt,byteOffset,tipoInd);

        desalocarRegistro(novoReg);
    }

    fclose(arqDados);
    fclose(arqInd);
    return 1;
}

//retorna o tamanho de um registro do tipo  2
int tamanhoRegistroTipo2(Registro_t *reg)
{
    if(!reg)
        return -1;

    int tamanhoReg = sizeof(int) * 4 + sizeof(char) * 3 + sizeof(long int);

    if(reg->tamCidade > 0)
        tamanhoReg += reg->tamCidade + sizeof(int) + sizeof(char);

    if(reg->tamMarca > 0)
        tamanhoReg += reg->tamMarca + sizeof(int) + sizeof(char);

    if(reg->tamModelo > 0)
        tamanhoReg += reg->tamModelo + sizeof(int) + sizeof(char);

    return tamanhoReg;

}