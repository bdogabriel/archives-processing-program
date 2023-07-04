// Gabriel Barbosa de Oliveira - 12543415 - Participacao : 100%
// Felipe Seiji Momma Valente - 12543700 - Participacao : 100%
//Entregue no dia 20/07/2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arq.h"
#include "tipo1.h"
#include "tipo2.h"
#include "funcoesFornecidas.h"
#include "arvoreB.h"

int problemaEncontrado(); // caso tenha achado algum problema na execucao do comando ele printao erro
int lerComando();         // le o comando e o executa

int main()
{
    int estado = lerComando(); // le o comando e o executa
    if (estado == -1)
        problemaEncontrado();
    return 0;
}

int problemaEncontrado()
{
    printf("Falha no processamento do arquivo.");
    return 0;
}

int lerComando()
{
    // le o comando
    int comando;
    int tipoArq;
    char *arqEnt = NULL;
    int aux;
    scanf("%d", &aux);

    comando = aux;
    if (comando < 1 || comando > 11)
        return -1;

    aux = getc(stdin);
    if (aux != ' ')
        return -1;
    //-----------------------

    // le o tipo do arquivo e o converte para int
    char *nomeTipo = readUntil(' ');

    if (strcmp(nomeTipo, "tipo1") == 0)
        tipoArq = 1;
    else if (strcmp(nomeTipo, "tipo2") == 0)
        tipoArq = 2;
    else
    {
        free(nomeTipo);
        return -1;
    }

    free(nomeTipo);
    //-----------------------

    // le o nome do primeiro arquivo
    arqEnt = readUntil(' ');

    if (comando == 1) // caso seja o de criar o binario
    {
        // le o nome do arquivo de saida
        char *arqSaida = readUntil('\n');

        if (criarArquivo(arqEnt, arqSaida, tipoArq) < 0) // executa
            problemaEncontrado();

        else
        {
            binarioNaTela(arqSaida);
            free(arqEnt);
        }

        free(arqSaida);

        return 0;
    }

    else if (comando == 2) // printar o arquivo inteiro
    {
        int estado = -1;

        if (tipoArq == 1)
            estado = printarArquivoBinInteiroTipo1(arqEnt);
        else
            estado = printarArquivoBinInteiroTipo2(arqEnt);

        if (estado < 0)
            problemaEncontrado();

        free(arqEnt);

        return 0;
    }

    else if (comando == 3) // pesquisa com parametros do arquivo binario
    {
        // le a quantidade de parametros que serao passados
        int qtdParametros;
        scanf("%d\n", &qtdParametros);
        //-----------------------

        char **campos = malloc(sizeof(char *) * qtdParametros);  // lista com todos os nomes dos parametros
        char **valores = malloc(sizeof(char *) * qtdParametros); // lista com todos os valroes dos parametros
        for (int i = 0; i < qtdParametros; ++i)
        {
            // le o nome do campo
            campos[i] = readUntil(' ');
            //-----------------------

            // le o valor do parametro
            valores[i] = readUntil('\n');
            //-----------------------
        }

        int estado;
        if (tipoArq == 1)
            estado = pesquisarComCamposTipo1(arqEnt, campos, valores, qtdParametros);
        else
            estado = pesquisarComCamposTipo2(arqEnt, campos, valores, qtdParametros);

        // libera a memoria dos valores de pesquisa
        for (int i = 0; i < qtdParametros; i++)
        {
            free(campos[i]);
            if (valores[i] != NULL)
            {
                free(valores[i]);
            }
        }

        free(campos);
        free(valores);
        free(arqEnt);
        //-----------------------

        getc(stdin);
        return estado;
    }

    else if (comando == 4) // printa o registro do rrn
    {
        // le o rrn a ser pesquisado
        int rrn;
        scanf("%d", &rrn);
        //-----------------------
        int estado = 0;

        if (tipoArq == 1)
        {
            estado = pesquisaRRN(rrn, arqEnt);
        }

        else if (estado < 0)
            problemaEncontrado();

        else
            problemaEncontrado();

        free(arqEnt);
        return estado;
    }
    else if (comando == 5)
    {
        // le o nome do arquivo de saida
        char *arqSaida = readUntil('\n');

        if (criarBinIndice(arqEnt, arqSaida, tipoArq) < 0){
            problemaEncontrado();
        } 

        binarioNaTela(arqSaida);

        free(arqEnt);
        free(arqSaida);

        return 0;
    }
    else if (comando == 6)
    {
        char *arqInd = readUntil(' ');//le o nome do arquivo de indices

        // le a quantidade de parametros que serao passados
        int qtdExclusoes;
        scanf("%d\n", &qtdExclusoes);

        int estado = 0;
        estado = excluirRegistrosEntrada(arqEnt,arqInd,qtdExclusoes,tipoArq);
        if(estado < 0)
            problemaEncontrado();

        binarioNaTela(arqEnt);
        binarioNaTela(arqInd);
        if(arqEnt)
            free(arqEnt);
        if(arqInd)
            free(arqInd);

        return 0;
    }
    else if (comando == 7)
    {
        char *arqInd = readUntil(' ');

        // numero de registros adicionados
        int n = 0;
        scanf("%d", &n);

        int success = -1;

        if(tipoArq == 2)
            success = inserirNovosRegTipo2(arqEnt,arqInd,n,1);
        
        else{
            FILE *entrada = fopen(arqEnt, "r+b");
            FILE *indice = fopen(arqInd, "r+b");

            for (int i = 0; i < n; i++)
            {
                Registro_t *reg = lerRegistro();
                success = inserirRegistroTipo1(entrada, indice, reg,1);

                if (success < 0){
                    problemaEncontrado();
                }

                desalocarRegistro(reg);
            }

            fclose(entrada);
            fclose(indice);
        }

        if (success >= 0){
            binarioNaTela(arqEnt);
            binarioNaTela(arqInd);
        }

        else{
            problemaEncontrado();
        }

        free(arqInd);
        free(arqEnt);

        return 0;
    }
    else if (comando == 8)
    {
        char *arqInd = readUntil(' ');

        // le a quantidade de parametros que serao passados
        int n = 0;
        scanf("%d", &n);

        //-----------------------

        for (int i = 0; i < n; ++i)
        {
            int qtdParametrosPesquisa;
            scanf("%d ", &qtdParametrosPesquisa);

            char **camposPesquisa = malloc(sizeof(char *) * qtdParametrosPesquisa);  // lista com todos os nomes dos parametros
            char **valoresPesquisa = malloc(sizeof(char *) * qtdParametrosPesquisa); // lista com todos os valroes dos parametros
            for (int k = 0; k < qtdParametrosPesquisa - 1; ++k)
            {
                // le o nome do campo
                camposPesquisa[k] = readUntil(' ');
                //-----------------------

                // le o valor do parametro
                valoresPesquisa[k] = readUntil(' ');
                if ((strcmp(camposPesquisa[k],"qtt") == 0 || strcmp(camposPesquisa[k], "ano") == 0) && (strcmp(valoresPesquisa[k], "NULO") == 0)){
                    free(valoresPesquisa[k]);
                    valoresPesquisa[k] = malloc(3);
                    valoresPesquisa[k][0] = '-';
                    valoresPesquisa[k][1] = '2';
                    valoresPesquisa[k][2] = '\0';
                }
                //-----------------------
            }
            // le o nome do campo
            camposPesquisa[qtdParametrosPesquisa - 1] = readUntil(' ');
            //-----------------------

            // le o valor do parametro
            valoresPesquisa[qtdParametrosPesquisa - 1] = readUntil('\n');
            if ((strcmp(camposPesquisa[qtdParametrosPesquisa - 1],"qtt") == 0 || strcmp(camposPesquisa[qtdParametrosPesquisa - 1], "ano") == 0) && (strcmp(valoresPesquisa[qtdParametrosPesquisa - 1], "NULO") == 0)){
                    free(valoresPesquisa[qtdParametrosPesquisa - 1]);
                    valoresPesquisa[qtdParametrosPesquisa - 1] = malloc(3);
                    valoresPesquisa[qtdParametrosPesquisa - 1][0] = '-';
                    valoresPesquisa[qtdParametrosPesquisa - 1][1] = '2';
                    valoresPesquisa[qtdParametrosPesquisa - 1][2] = '\0';
            }
            //-----------------------

            int qtdParametrosAtualiza;
            scanf("%d ", &qtdParametrosAtualiza);

            char **camposAtualiza = malloc(sizeof(char *) * qtdParametrosAtualiza);  // lista com todos os nomes dos parametros
            char **valoresAtualiza = malloc(sizeof(char *) * qtdParametrosAtualiza); // lista com todos os valroes dos parametros
            for (int k = 0; k < qtdParametrosAtualiza - 1; ++k)
            {
                // le o nome do campo
                camposAtualiza[k] = readUntil(' ');
                //-----------------------

                // le o valor do parametro
                valoresAtualiza[k] = readUntil(' ');
                if ((strcmp(camposAtualiza[k],"qtt") == 0 || strcmp(camposAtualiza[k], "ano") == 0) && (strcmp(valoresAtualiza[k], "NULO") == 0)){
                    free(valoresAtualiza[k]);
                    valoresAtualiza[k] = malloc(3);
                    valoresAtualiza[k][0] = '-';
                    valoresAtualiza[k][1] = '2';
                    valoresAtualiza[k][2] = '\0';
                }
                //-----------------------
            }
            // le o nome do campo
            camposAtualiza[qtdParametrosAtualiza - 1] = readUntil(' ');
            //-----------------------

            // le o valor do parametro
            valoresAtualiza[qtdParametrosAtualiza - 1] = readUntil('\n');
            if ((strcmp(camposAtualiza[qtdParametrosAtualiza - 1],"qtt") == 0 || strcmp(camposAtualiza[qtdParametrosAtualiza - 1], "ano") == 0) && (strcmp(valoresAtualiza[qtdParametrosAtualiza - 1], "NULO") == 0)){
                    free(valoresAtualiza[qtdParametrosAtualiza - 1]);
                    valoresAtualiza[qtdParametrosAtualiza - 1] = malloc(3);
                    valoresAtualiza[qtdParametrosAtualiza - 1][0] = '-';
                    valoresAtualiza[qtdParametrosAtualiza - 1][1] = '2';
                    valoresAtualiza[qtdParametrosAtualiza - 1][2] = '\0';
            }
            //-----------------------

            int estado;
            estado = atualizarDadosRegistro(arqEnt, arqInd, camposPesquisa, valoresPesquisa, qtdParametrosPesquisa, camposAtualiza, valoresAtualiza, qtdParametrosAtualiza, tipoArq);

            if (estado < 0){
                problemaEncontrado();
            }

            // libera a memoria dos valores de pesquisa
            for (int j = 0; j < qtdParametrosPesquisa; j++)
            {
                free(camposPesquisa[j]);
                if (valoresPesquisa[j] != NULL)
                {
                    free(valoresPesquisa[j]);
                }
            }

            free(camposPesquisa);
            free(valoresPesquisa);

            for (int j = 0; j < qtdParametrosAtualiza; j++)
            {
                free(camposAtualiza[j]);
                if (valoresAtualiza[j] != NULL)
                {
                    free(valoresAtualiza[j]);
                }
            }

            free(camposAtualiza);
            free(valoresAtualiza);
        }

        binarioNaTela(arqEnt);
        binarioNaTela(arqInd);
        free(arqEnt);
        free(arqInd);
        //-----------------------

        return 0;
    }
    else if (comando == 9)
    {
        char *arqIndArvB = readUntil('\n');//le o nome do arquivo da arvore b

        int estado = -1;
        estado = criarIndArvB(arqEnt,arqIndArvB,tipoArq);

        if(estado < 0)
            problemaEncontrado();

        binarioNaTela(arqIndArvB);

        if(arqEnt)
            free(arqEnt);
        if(arqIndArvB)
            free(arqIndArvB);

        return 0;
    }
    else if (comando == 10)
    {
        char *arqIndArvB = readUntil(' ');//le o nome do arquiva arvore b
        char *tmp = readUntil(' ');//le o nome do arquivo da arvore b

        // le a quantidade de parametros que serao passados
        int valorId;
        scanf("%d", &valorId);

        int estado = 0;
        estado = printBuscaIdArvoreB(arqEnt,arqIndArvB,valorId,tipoArq);
        if(estado < 0)
            problemaEncontrado();

        if(arqEnt)
            free(arqEnt);
        if(arqIndArvB)
            free(arqIndArvB);
        if(tmp)
            free(tmp);
        return 0;
    }
    else if (comando == 11)
    {
        char *arqInd = readUntil(' ');

        // numero de registros adicionados
        int n = 0;
        scanf("%d", &n);

        int success = -1;

        if(tipoArq == 2)
            success = inserirNovosRegTipo2(arqEnt,arqInd,n,2);
        else{
            FILE *entrada = fopen(arqEnt, "r+b");
            FILE *indice = fopen(arqInd, "r+b");

            for (int i = 0; i < n; i++)
            {
                Registro_t *reg = lerRegistro();
                success = inserirRegistroTipo1(entrada, indice, reg,2);

                if (success < 0){
                    problemaEncontrado();
                }

                desalocarRegistro(reg);
            }

            fclose(entrada);
            fclose(indice);
        }

        if (success >= 0){
            binarioNaTela(arqEnt);
            binarioNaTela(arqInd);
        }

        else{
            problemaEncontrado();
        }

        free(arqInd);
        free(arqEnt);

        return 0;
    }
    else
    {
        problemaEncontrado();
    }

    free(arqEnt);
    return -1;
}