// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700

// esse .c contem as funcoes comuns aos dois tipos de arquivo
#include "arq.h"
#include "auxiliar.h"
#include "tipo1.h"
#include "tipo2.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int criarBin(FILE *fp, FILE *nomeBin, int tipoArq, long int *byteOffset);
int atualizarTopo(FILE *arqBin, FILE *arqInd,char *nomeArqInd,RegistroIndice_t **regsIndExcluidos, int *tamRegExcluidos, int qtdExcluidos, int tipoArq);
int excluirIndices(FILE *arqInd,char *stringArqInd ,RegistroIndice_t **regsExcluir, int qtdRegsExcluir, int tipoArq);
int excluirRegsArqBin(FILE *arqBin,FILE *arqInd,Registro_t **regsPesq,int *qtdParamRegsPesq,int qtdregsPes,char *nomeArqInd ,int tipoArq);
int pegarFilaTopo(FILE *arqBin,RegistroIndice_t **regsIndExc, int *tamnRegsIndExc ,int qtdExc, int comecoReg);
int intercalonamentoRegExcluir(RegistroIndice_t **regs,int *tamanhoRegs, int maxRegs);

int criarArquivo(char *arqCSV, char *arqSaida, int tipoArq)
{
    // abre ambos os arquivos
    FILE *csv = fopen(arqCSV, "r");

    if (csv == NULL)
        return -1;

    FILE *binSaida = fopen(arqSaida, "wb");

    if (montarCabecalho(binSaida, tipoArq) < 0) // monta o cabecalho
        return -1;

    long int byteOffset = 0;
    int rrn = criarBin(csv, binSaida, tipoArq, &byteOffset); // le o csv e cria o arquivo binario

    fclose(csv);

    if (tipoArq == 1)
        atualizarRRNCabecalho(binSaida, rrn); // atualiza o rrn do cabecalho no tipo 1

    else
    {
        atualizarByteOffsetCabecalho(binSaida, byteOffset);
    }

    mudarStatus(binSaida, 1); // muda o estado do arquivo para completo

    fclose(binSaida);
    return 0;
}

int criarBin(FILE *fp, FILE *nomeBin, int tipoArq, long int *byteOffset)
{
    // ler cabecalho
    char cabecalho[60];
    fscanf(fp, "%s", cabecalho);

    endLine(fp);

    char c = 'a';
    int qtdReg = 0;

    while (c != EOF)
    {
        Registro_t *reg = malloc(sizeof(Registro_t));

        inicializarRegistro(reg);

        int nCampo = 1;

        while (nCampo <= 7)
        {
            c = getc(fp);

            if (c == ',' || c == EOF || c == '\r')
            {
                if (c == '\r')
                {
                    getc(fp); // \n
                }
                nCampo++;
                continue;
            }

            else
            {
                ungetc(c, fp);

                switch (nCampo)
                {
                case 1:
                    fscanf(fp, "%d", &reg->id);
                    break;

                case 2:
                    fscanf(fp, "%d", &reg->ano);
                    break;

                case 3:
                    reg->tamCidade = lerCampo(fp, &reg->cidade);
                    nCampo++;
                    break;

                case 4:
                    fscanf(fp, "%d", &reg->qtt);
                    break;

                case 5:
                    fscanf(fp, "%c", &reg->sigla[0]);
                    fscanf(fp, "%c", &reg->sigla[1]);
                    getc(fp); // pega a virgula
                    nCampo++;
                    break;

                case 6:
                    reg->tamMarca = lerCampo(fp, &reg->marca);
                    nCampo++;
                    break;

                default:
                    reg->tamModelo = lerCampo(fp, &reg->modelo);
                    nCampo++;
                }
            }
        }

        if (c != EOF)
        {
            addBinarioRegistro(reg, nomeBin, tipoArq);
            qtdReg++;
        }

        desalocarRegistro(reg);
    }

    *byteOffset = ftell(nomeBin);

    return qtdReg;
}

int criarBinIndice(char *nomeArqEnt, char *nomeArqInd, int tipoArq)
{
    if (!nomeArqEnt || !nomeArqInd || tipoArq > 2 || tipoArq < 1)
        return -1;

    FILE *arqEnt = fopen(nomeArqEnt, "rb");
    FILE *arqIndice = fopen(nomeArqInd, "wb");

    if (!arqEnt || !arqIndice)
        return -1;

    RegistroIndice_t **vetorIndices = malloc(sizeof(RegistroIndice_t *));
    vetorIndices[0] = NULL;
    int qtdIndices = 0;

    // le os arquivos e monta um vetor de struct com os dados necessarios para criar o arquivo de indices(sem estar em ordem)
    if (tipoArq == 1)
    {
        montarVetorIndicesTipo1(arqEnt,arqIndice, vetorIndices, &qtdIndices);
    }
    else
    {
        montarVetorIndicesTipo2(arqEnt, arqIndice,vetorIndices, &qtdIndices);
    }

    // libera as memorias alocadas na funcao e fecha os arquivos abertos

    fclose(arqEnt);
    fclose(arqIndice);

    return 1;
}

// le uma string da entrada padrao ate o caracter de parada
char *readUntil(char stop)
{
    char *string = malloc(sizeof(char));

    int n = 0;

    char c;

    do
    {
        c = getc(stdin);

        if (c != stop && c != '\r' && c != EOF && c != '\n')
        {
            if (c != '"')
            {
                string[n] = c;
                n++;
                string = realloc(string, (n + 1) * sizeof(char));
            }
        }

    } while (c != stop && c != EOF && c != '\n');

    string[n] = '\0';

    return string;
}

Registro_t *lerRegistro()
{
    Registro_t *reg = malloc(sizeof(Registro_t));
    inicializarRegistro(reg);

    scanf("%d", &reg->id);
    getchar(); // espaço

    for (int i = 0; i < 6; i++)
    {
        char c = getc(stdin);

        if (c == 'N')
        {
            char *string = readUntil(' ');
            if(string)
                free(string);
            continue;
        }

        else if (c != '\"')
        {
            ungetc(c, stdin);

            switch (i)
            {
            case 0:
                scanf("%d", &reg->ano);
                getchar(); // espaço
                break;

            case 1:
                scanf("%d", &reg->qtt);
                getchar(); // espaço
                break;

            default:
                break;
            }
        }

        else
        {
            char *string = readUntil('\"');
            getchar(); // espaço ou \n

            switch (i)
            {

            case 2:
                reg->sigla[0] = string[0];
                reg->sigla[1] = string[1];
                if(string)
                    free(string);
                break;

            case 3:
                reg->tamCidade = strlen(string);
                reg = realloc(reg, sizeof(Registro_t) + reg->tamCidade);
                reg->cidade = malloc(reg->tamCidade);
                copiarSemBarraZero(string, reg->cidade);
                if(string)
                    free(string);
                break;

            case 4:
                reg->tamMarca = strlen(string);
                reg = realloc(reg, sizeof(Registro_t) + reg->tamCidade + reg->tamMarca);
                reg->marca = malloc(reg->tamMarca);
                copiarSemBarraZero(string, reg->marca);
                if(string)
                    free(string);
                break;

            case 5:
                reg->tamModelo = strlen(string);
                reg = realloc(reg, sizeof(Registro_t) + reg->tamCidade + reg->tamMarca + reg->tamModelo);
                reg->modelo = malloc(reg->tamModelo);
                copiarSemBarraZero(string, reg->modelo);
                if(string)
                    free(string);
                break;

            default:
                if(string)
                    free(string);
                break;
            }
        }
    }

    return reg;
}

int atualizarDadosRegistro(char *stringArqDados, char *stringArqInd, char **campos, char **valores, int qtdCampos, char **camposSub, char **valoresSub, int qtdCamposSub, int tipoArq)
{
    if(!stringArqDados || !stringArqInd || !campos || !valores || qtdCampos <= 0 || !camposSub || !valoresSub || qtdCamposSub <=0 || tipoArq > 2 || tipoArq < 1){
        return -1;
    }

    Registro_t *regPesquisa = malloc(sizeof(Registro_t));
    Registro_t *regSub = malloc(sizeof(Registro_t));

    inicializarRegistro(regPesquisa);
    inicializarRegistro(regSub);

    FILE *arqInd = fopen(stringArqInd, "r+b");
    FILE *arqDados = fopen(stringArqDados, "r+b");

    montarRegistroPesquisa(regPesquisa,campos,valores,qtdCampos);
    montarRegistroPesquisa(regSub,camposSub,valoresSub,qtdCamposSub);

    if(regPesquisa->id >= 0)
    {
        RegistroIndice_t *regIndicePesquisa = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(regIndicePesquisa);

        if(pesquisaBinariaIndice(arqInd,regSub->id,regIndicePesquisa,tipoArq) >= 0)
            return -1;

        inicializarRegistroIndice(regIndicePesquisa);
        if(pesquisaBinariaIndice(arqInd,regPesquisa->id,regIndicePesquisa,tipoArq) < 0){
            return -1;
        }

        if(tipoArq == 1)
        {
            alterarValoresRegistroTipo1(arqDados,regSub,regIndicePesquisa->rrn);

            if(regSub->id >= 0)
            {
                RegistroIndice_t *regIndNovo = malloc(sizeof(RegistroIndice_t));
                inicializarRegistroIndice(regIndNovo);
                regIndNovo->rrn = regIndicePesquisa->rrn;
                regIndNovo->indice = regSub->id;
                atualizarIndicesTipo1(arqInd,regIndicePesquisa,regIndNovo,tipoArq);
                free(regIndNovo);
            }
        }else
        {
            alterarValoresRegistroTipo2(arqDados,arqInd ,regSub, regIndicePesquisa->byteOffset);
        }
        if(regIndicePesquisa)
            free(regIndicePesquisa);

    }else
    {
        fseek(arqDados, 0, SEEK_SET);
        char status = '0';
        fread(&status, sizeof(char), 1, arqDados);
        if (status != '1')
            return -1;

        if(tipoArq == 1)
        {
            // caso o maxrrn seja realmente o rrn maximo
            fseek(arqDados, 174, SEEK_SET);
            int maxRRN = 0;
            fread(&maxRRN, sizeof(int), 1, arqDados);

            fseek(arqDados, 182, SEEK_SET);

            for (int i = 0; i < maxRRN - 1; ++i)
            {
                Registro_t *regProcurar = malloc(sizeof(Registro_t));
                inicializarRegistro(regProcurar);
                if (pegarProximoRegTipo1(arqDados, regProcurar) >= 0)
                {
                    if (compararPesquisaRegistro(regProcurar, regPesquisa) == 0)
                    {
                        alterarValoresRegistroTipo1(arqDados,regSub,i);

                        if(regSub->id >= 0)
                        {
                            RegistroIndice_t *regIndNovo = malloc(sizeof(RegistroIndice_t));
                            inicializarRegistroIndice(regIndNovo);
                            regIndNovo->rrn = i;
                            regIndNovo->indice = regSub->id;

                            RegistroIndice_t *regIndicePesquisa = malloc(sizeof(RegistroIndice_t));
                            inicializarRegistroIndice(regIndicePesquisa);
                            regIndicePesquisa->rrn = i;
                            regIndicePesquisa->indice = regProcurar->id;
                            atualizarIndicesTipo1(arqInd,regIndicePesquisa,regIndNovo,tipoArq);
                            free(regIndNovo);
                            free(regIndicePesquisa);
                        }
                    }
                }
                desalocarRegistro(regProcurar);
            }
        }
        else
        {
            fseek(arqDados, 177, SEEK_CUR);
            long int byteAtual = 190;
            long int proxByteOffset = 0;
            fread(&proxByteOffset, sizeof(long int), 1, arqDados);

            while(byteAtual < proxByteOffset)
            {
                Registro_t *regProcurar = malloc(sizeof(Registro_t));
                inicializarRegistro(regProcurar);
                if (pegarProxRegistroTipo2(arqDados, regProcurar) >= 0)
                {
                    if (compararPesquisaRegistro(regProcurar, regPesquisa) == 0)
                    {
                        alterarValoresRegistroTipo2(arqDados,arqInd ,regSub, byteAtual);
                    }
                    printarStructRegistro(regProcurar);
                    byteAtual += regProcurar->tamanhoRegistro;

                }
                desalocarRegistro(regProcurar);
            }
        }
    }

    desalocarRegistro(regPesquisa);
    desalocarRegistro(regSub);
    fclose(arqInd);
    fclose(arqDados);
    return 1;
}


int excluirRegistrosEntrada(char *stringArqBin, char *stringArqInd, int qtdLinhas, int tipoArq)//exclui registros passados como a funcionalidade 6
{
    Registro_t **regsExcluirPesq = malloc(sizeof(Registro_t*)*qtdLinhas);//vetor com os valores que querem ser excluidos
    int *qtdParamRegsPesq = malloc(sizeof(int)*qtdLinhas);//quantidade de parametros para cada exclusao
    char ***campos = malloc(sizeof(char **) * qtdLinhas);  // matriz com os campos de cada exclusao
    char ***valores = malloc(sizeof(char **) * qtdLinhas);// matriz com o valor dos campos de cada exclusao

    for (int i = 0; i < qtdLinhas; ++i)//le os campos e valores, qtdLinhas eh a quantidade  de linhas serao passadas para exclusao
    {
        int qtdParametros;
        scanf("%d ", &qtdParametros);//quantidade de campos e valores que serao passados nessa linha

        campos[i] = malloc(sizeof(char *) * qtdParametros);  // lista com todos os nomes dos parametros
        valores[i]= malloc(sizeof(char *) * qtdParametros); // lista com todos os valroes dos parametros
        for (int k = 0; k < qtdParametros-1; ++k)//le os campos e valores
        {
            // le o nome do campo
            campos[i][k] = readUntil(' ');
            //-----------------------

            // le o valor do parametro
            valores[i][k] = readUntil(' ');
            //-----------------------
        }
        // le o nome do campo
        campos[i][qtdParametros - 1] = readUntil(' ');
        //-----------------------

        // le o valor do parametro
        valores[i][qtdParametros - 1] = readUntil('\n');
        //-----------------------


        qtdParamRegsPesq[i] = qtdParametros;//salva a quantidade de parametros passados para essa linha

        //monta o registro de pesquisa com as informacoes lidos
        regsExcluirPesq[i] = malloc(sizeof(Registro_t));
        inicializarRegistro(regsExcluirPesq[i]);
        montarRegistroPesquisa(regsExcluirPesq[i],campos[i],valores[i],qtdParametros);
    }

    //abre ambos os arquivos
    FILE *arqBin = fopen(stringArqBin,"r+b");
    FILE *arqInd = fopen(stringArqInd,"r+b");


    int estado = 0;//valor de retorno da funcao, para saber se tudo ocorreu como o esperado
    estado = excluirRegsArqBin(arqBin,arqInd,regsExcluirPesq,qtdParamRegsPesq,qtdLinhas,stringArqInd,tipoArq);//realiza a exclusao

    //libera a memoria de campos e valores
    for (int i = 0; i < qtdLinhas; ++i) {
        for (int j = 0; j < qtdParamRegsPesq[i]; j++)
        {
            if(campos[i][j])
                free(campos[i][j]);
            if (valores[i][j] != NULL)
            {
                if(valores[i][j])
                    free(valores[i][j]);
            }
        }
        if(campos[i])
            free(campos[i]);
        if(valores[i])
            free(valores[i]);
    }
    if(campos)
        free(campos);
    if(valores)
        free(valores);

    //libera a memoria dos registros de pesquisa
    for (int i = 0; i < qtdLinhas; ++i) {
        desalocarRegistro(regsExcluirPesq[i]);

    }
    if(regsExcluirPesq)
        free(regsExcluirPesq);
    if(qtdParamRegsPesq)
        free(qtdParamRegsPesq);

    //fecha os arquivos e renomeia o novo arquivo de indices para o nome antigo
    fclose(arqInd);
    if (remove(stringArqInd) < 0)
        return -1;
    if (rename("auxInd.bin", stringArqInd) < 0)
        return -1;
    if(arqBin)
        fclose(arqBin);

    //se tudo ocorreu corretamente retorna 1, ao contrario -1
    if(estado > 0)
        return 1;
    else
        return -1;
}


/* essa funcao ira percorrer o arquivo(caso tenha o id, ira diretamente) e se precisar remover o registro, ela ira marcar o registro e salvar o indice para
 * que depois possa ser alterado o arquivo de indices, essa funcao executa todas as linhas de exclusao de uma vez, economizando assim a quantidade de fseek
 * utilizados
*/
int excluirRegsArqBin(FILE *arqBin,FILE *arqInd,Registro_t **regsPesq,int *qtdParamRegsPesq,int qtdregsPes,char *nomeArqInd ,int tipoArq)
{
    //verifica os valores passados como parametros
    if (!arqBin || !regsPesq || tipoArq < 1 || tipoArq > 2)
        return -1;

    char status;
    fread(&status, sizeof(char), 1, arqBin);
    if (status != '1')
        return -1;

    //le o numero de registros que ja estao removidos no arquivo
    int nroRegRem = 0;
    if(tipoArq == 2)
    {
        fseek(arqBin,186,SEEK_SET);
        fread(&nroRegRem,sizeof(int),1,arqBin);
    }else
    {
        fseek(arqBin,178,SEEK_SET);
        fread(&nroRegRem,sizeof(int),1,arqBin);
    }

    mudarStatus(arqBin,0);//marca que o arquivo esta em alteracao

    /* a logica aqui eh que cada valor passado para exclusao pode excluir X registros, entao eh necessario um vetor para saber quais serao excluidos,
     * mas como aqui temos Y valores de exclusao, entao precisamos de uma matrix Y por Z para guardar todas as informacoes para uso posterior
     */

    //aloca o basico de memoria para as variaves que serao utilizadas no geral da funcao
    RegistroIndice_t ***indsRegExcluir = malloc(sizeof (RegistroIndice_t **)*qtdregsPes);//matriz de indices que tem que serem removidos
    int **tamRegExcluir = malloc(sizeof(int*) * qtdregsPes);//matriz do tamanho dos registros que serao removidos(utilizado para o tipo 2)
    for (int i = 0; i < qtdregsPes; ++i) {//aloca o basico de memoria para cada um dos vetores da matriz
        indsRegExcluir[i] = malloc(0);
        tamRegExcluir[i] = malloc(0);
    }
    int *qtdExcluidos = calloc(sizeof(int), qtdregsPes);//quantidade de exclusao para cada set de valores
    int *regsPesqCompletados = calloc(qtdregsPes,sizeof(int));//marca se o set de exclusao ja acabou de ser pesquisados

    //percorre pesquisando sobre o id
    for (int i = 0; i < qtdregsPes; ++i) {
        if(regsPesq[i]->id >= 0)//caso tenha id na pesquisa, entao pode ter no maximo 1 registro excluido, pois ele eh unico
        {
            regsPesqCompletados[i] = 1;//marca que ja pesquisou tudo sobre esse valor

            //faz a pesquisa binaria no arquivo de indices para ver se o id existe
            RegistroIndice_t *regAux = malloc(sizeof(RegistroIndice_t));
            inicializarRegistroIndice(regAux);
            pesquisaBinariaIndice(arqInd, regsPesq[i]->id, regAux, tipoArq);

            if(regAux->byteOffset > 0 || regAux->rrn > 0)//caso o id exista
            {
                //byteOffset do registro que pode ter o valor a ser excluido
                long int byteOffset;
                if(tipoArq == 1)
                    byteOffset = regAux->rrn * 97 + 182;
                else
                    byteOffset = regAux->byteOffset;

                long int qtdByteVoltar = -1;
                if(qtdParamRegsPesq[i] > 1)//caso na pesquisa tenha mais do que somente o id
                {
                    //vai ate o registro e pega as outras informacoes dele
                    fseek(arqBin,byteOffset,SEEK_SET);
                    Registro_t *regProcurar = malloc(sizeof(Registro_t));
                    inicializarRegistro(regProcurar);
                    if(tipoArq == 1)
                        pegarProximoRegTipo1(arqBin,regProcurar);
                    else
                        pegarProxRegistroTipo2(arqBin,regProcurar);

                    if(compararPesquisaRegistro(regProcurar,regsPesq[i]) != 0)//compara com os valores de pesquisa
                    {
                        desalocarRegistro(regProcurar);
                        goto sairIfs;
                    }

                    //volta para o comeco desse registro
                    qtdByteVoltar = 97;
                    if(tipoArq == 2)
                        qtdByteVoltar =  -1 * (regProcurar->tamanhoRegistro + 5);
                    fseek(arqBin,qtdByteVoltar,SEEK_CUR);

                    desalocarRegistro(regProcurar);
                }else//caso seja somente o id, entao pode excluir esse registro
                    fseek(arqBin,byteOffset,SEEK_SET);

                //marca o registro como removido
                char removido = '1';
                fwrite(&removido,1,1,arqBin);
                long int test = ftell(arqBin);

                //aumenta o tamanho das matrizes e vetores
                qtdExcluidos[i]++;
                indsRegExcluir[i] = realloc(indsRegExcluir[i],sizeof(RegistroIndice_t *) * (qtdExcluidos[i]));
                indsRegExcluir[i][qtdExcluidos[i]-1] = malloc(sizeof(RegistroIndice_t));
                inicializarRegistroIndice(indsRegExcluir[i][qtdExcluidos[i]-1]);
                tamRegExcluir[i] = realloc(tamRegExcluir[i],sizeof(int) * qtdExcluidos[i]);

                //guarda os valores do registro que serao utilizados depois
                if(qtdByteVoltar == -1)
                {
                    int tamTest = 0;
                    fseek(arqBin,test,SEEK_SET);
                    fread(&tamTest,4,1,arqBin);
                    tamRegExcluir[i][qtdExcluidos[i]-1] = tamTest;

                }
                else
                    tamRegExcluir[i][qtdExcluidos[i]-1] = -1 * qtdByteVoltar;
                if(tipoArq == 1)
                    tamRegExcluir[i][qtdExcluidos[i]-1] = 97;
                indsRegExcluir[i][qtdExcluidos[i]-1]->byteOffset = regAux->byteOffset;
                indsRegExcluir[i][qtdExcluidos[i]-1]->rrn = regAux->rrn;
                indsRegExcluir[i][qtdExcluidos[i]-1]->indice = regAux->indice;

            }
            sairIfs://caso nao exista esse registro pesquisadow
            if(regAux)
                free(regAux);
        }
    }

    //conta quanto ja foi pesquisado
    int totalPesquisados = 0;
    for (int i = 0; i < qtdregsPes; ++i) {
        totalPesquisados += regsPesqCompletados[i];
    }

    if(totalPesquisados == qtdregsPes)//caso se somente pesquisando o id, ja foi tudo
        goto pesquisaCompleta;

    //caso ainda falte pesquisar, entao faz uma pesquisa sequencial pelo arquivo
    long int byteAtual;//posicao atual
    long int byteOffset = 174;
    int qtdRRN = 0;
    fseek(arqBin, 1, SEEK_SET);
    if(tipoArq == 2)
        byteOffset = 178;

    //le o byte max
    fseek(arqBin, byteOffset, SEEK_SET);
    long int maxByteOffset = 190;//byte maximo do arquivoss
    if(tipoArq == 1)
    {
        fread(&qtdRRN,sizeof (int),1,arqBin);
        maxByteOffset = qtdRRN * 97 + 182;
    }else
        fread(&maxByteOffset,8,1,arqBin);

    //deixa o ponteiro no primeiro registro e atualiza o byteAual
    fseek(arqBin,sizeof(int),SEEK_CUR);
    byteAtual = ftell(arqBin);

    int rrn = 0;//qual registro esta no momento
    while(byteAtual < maxByteOffset)//le os registros ate o fim do arquivo
    {
        //le o proximo registro
        Registro_t *regProcurar = malloc(sizeof(Registro_t));
        inicializarRegistro(regProcurar);
        if (tipoArq == 1 && pegarProximoRegTipo1(arqBin, regProcurar) <= 0)
        {
            goto fimWhile;

        }else if(tipoArq == 2 && pegarProxRegistroTipo2(arqBin,regProcurar) <= 0)
        {
            goto fimWhile;
        }

        //verifica esse registro precisa ser excluido
        int excluir = -1;//qual valor que queria excluir esse registro
        for (int i = 0; i < qtdregsPes; ++i) {
            if(compararPesquisaRegistro(regProcurar,regsPesq[i]) == 0)
            {
                excluir = i;
                i = qtdregsPes;//para sair do for
            }
        }

        if(excluir == -1)//caso nao precisa excluir, entao vai para o proximo registro direto
            goto fimWhile;

        //volta para o comeco do registro
        long int qtdByteVoltar = -97;
        if(tipoArq == 2)
            qtdByteVoltar =  -1 * (regProcurar->tamanhoRegistro + 5);
        fseek(arqBin,qtdByteVoltar,SEEK_CUR);

        //marca o registro como removido e volta para o proximo registro
        char removido = '1';
        fwrite(&removido,1,1,arqBin);
        fseek(arqBin,(qtdByteVoltar + 1)*-1,SEEK_CUR);

        //aumenta o tamanho das matrizes e vetores
        qtdExcluidos[excluir]++;
        indsRegExcluir[excluir] = realloc(indsRegExcluir[excluir],sizeof(RegistroIndice_t *) * qtdExcluidos[excluir]);
        indsRegExcluir[excluir][qtdExcluidos[excluir]-1] = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(indsRegExcluir[excluir][qtdExcluidos[excluir]-1]);
        tamRegExcluir[excluir] = realloc(tamRegExcluir[excluir],sizeof(int) * qtdExcluidos[excluir]);

        //guarda os valores do registro que serao utilizados depois
        if(tipoArq == 1)
            tamRegExcluir[excluir][qtdExcluidos[excluir]-1] = 97;
        else
            tamRegExcluir[excluir][qtdExcluidos[excluir]-1] = qtdByteVoltar * -1;

        if(tipoArq == 2)
            indsRegExcluir[excluir][qtdExcluidos[excluir]-1]->byteOffset = ftell(arqBin) + qtdByteVoltar;
        else
            indsRegExcluir[excluir][qtdExcluidos[excluir]-1]->rrn = rrn;
        indsRegExcluir[excluir][qtdExcluidos[excluir]-1]->indice = regProcurar->id;

        fimWhile://atualiza o valor do byteAtual e reinicia as variaveis para a proxima leitura
        if(tipoArq == 1)
            byteAtual += 97;
        else
            byteAtual += regProcurar->tamanhoRegistro + 5;
        desalocarRegistro(regProcurar);
        byteAtual = ftell(arqBin);
        rrn++;
    }
    pesquisaCompleta:;

    //atualiza a quantidade de vetor excluidos
    int qtdRegsExcluir = 0;
    for (int i = 0; i < qtdregsPes; ++i) {
        qtdRegsExcluir += qtdExcluidos[i];
    }

    //transforma a matriz de registros que foram excluido em um vetor na ordem que devem ser removidos no indice
    RegistroIndice_t **indsRegExcluirUnico = malloc(sizeof(RegistroIndice_t*) * qtdRegsExcluir);
    int *tamRegExcluirUnico = malloc(sizeof(int) * qtdRegsExcluir);
    for (int i = 0; i < qtdRegsExcluir; ++i) {
        indsRegExcluirUnico[i] = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(indsRegExcluirUnico[i]);
    }
    int indice = 0;
    for (int i = 0; i < qtdregsPes; ++i) {
        for (int j = 0; j < qtdExcluidos[i]; ++j) {
            indsRegExcluirUnico[indice]->indice = indsRegExcluir[i][j]->indice;
            indsRegExcluirUnico[indice]->byteOffset = indsRegExcluir[i][j]->byteOffset;
            indsRegExcluirUnico[indice]->rrn = indsRegExcluir[i][j]->rrn;
            tamRegExcluirUnico[indice] = tamRegExcluir[i][j];
            if(indsRegExcluir[i][j])
                free(indsRegExcluir[i][j]);
            indice++;
        }
        if(tamRegExcluir[i])
            free(tamRegExcluir[i]);
        if(indsRegExcluir[i])
            free(indsRegExcluir[i]);
    }
    if(tamRegExcluir)
        free(tamRegExcluir);
    if(indsRegExcluir)
        free(indsRegExcluir);
    //-----------------------------------------------------------------------

    //caso precisa dos registro que ja estavam removidos para o indice, somente no caso tipo 2
    if(nroRegRem > 0 && tipoArq == 2) {
        indsRegExcluirUnico = realloc(indsRegExcluirUnico, sizeof(RegistroIndice_t *) * (qtdRegsExcluir + nroRegRem));
        tamRegExcluirUnico = realloc(tamRegExcluirUnico, sizeof(int) * (qtdRegsExcluir + nroRegRem));
        for (int i = qtdRegsExcluir; i < nroRegRem + qtdRegsExcluir; ++i) {
            indsRegExcluirUnico[i] = malloc(sizeof(RegistroIndice_t));
            inicializarRegistroIndice(indsRegExcluirUnico[i]);
        }
    }

    //atualiza a pilha/fila de removidos e atualiza o arquivo de indices
    int estado = 0;
    estado = atualizarTopo(arqBin,arqInd,nomeArqInd,indsRegExcluirUnico,tamRegExcluirUnico,qtdRegsExcluir,tipoArq);
    mudarStatus(arqBin,1);

    //desaloca as memorias
    free(tamRegExcluirUnico);
    free(qtdExcluidos);
    free(regsPesqCompletados);

    if (tipoArq == 1)
        nroRegRem = 0;

    for (int i = 0; i < qtdRegsExcluir + nroRegRem; i++){
        free(indsRegExcluirUnico[i]);
    }

    free(indsRegExcluirUnico);

    if(estado < 0)
        return -1;
    return 1;
}

//funcao para atualizar a pilha ou fila de valores removidos
int atualizarTopo(FILE *arqBin, FILE *arqInd,char *nomeArqInd,RegistroIndice_t **regsIndExcluidos, int *tamRegExcluidos, int qtdExcluidos, int tipoArq)
{
    if(!arqBin || !regsIndExcluidos || !tamRegExcluidos || qtdExcluidos <= 0 || tipoArq > 2 || tipoArq < 1)
        return -1;

    fseek(arqBin,0,SEEK_SET);

    //pega a quantidade de valores removidos
    int nroRegRem = 0;
    if(tipoArq == 1)
    {
        fseek(arqBin,178,SEEK_CUR);
        fread(&nroRegRem,sizeof(int),1,arqBin);
    }else
    {
        fseek(arqBin,186,SEEK_CUR);
        fread(&nroRegRem,sizeof(int),1,arqBin);
    }
    //atualiza o valores de removidos e escreve o valor atualizado
    nroRegRem += qtdExcluidos;
    fseek(arqBin,-4,SEEK_CUR);
    fwrite(&nroRegRem, sizeof(int),1,arqBin);
    nroRegRem -= qtdExcluidos;

    if(tipoArq == 1)
    {
        int topoAtual;
        fseek(arqBin,1,SEEK_SET);
        fread(&topoAtual,sizeof(int),1,arqBin);
        fseek(arqBin,1,SEEK_SET);
        fwrite(&regsIndExcluidos[qtdExcluidos-1]->rrn, sizeof(int),1,arqBin);
        for (int i = qtdExcluidos-2; i >= 0 ; --i) {
            long int byteOffset = regsIndExcluidos[i+1]->rrn * 97 + 182;
            fseek(arqBin,byteOffset + 1, SEEK_SET);
            fwrite(&regsIndExcluidos[i]->rrn,sizeof(int),1,arqBin);
        }
        long int byteOffset = regsIndExcluidos[0]->rrn * 97 + 182;
        fseek(arqBin,byteOffset + 1, SEEK_SET);
        fwrite(&topoAtual,sizeof(int),1,arqBin);
    }else
    {
        if(nroRegRem > 0) {
            pegarFilaTopo(arqBin, regsIndExcluidos, tamRegExcluidos, nroRegRem, qtdExcluidos);

        }

        intercalonamentoRegExcluir(regsIndExcluidos,tamRegExcluidos,(qtdExcluidos + nroRegRem));

        fseek(arqBin,1,SEEK_SET);
        fwrite(&regsIndExcluidos[qtdExcluidos + nroRegRem-1]->byteOffset, 8,1,arqBin);
        for (int i = qtdExcluidos + nroRegRem-1; i > 0 ; --i) {
            fseek(arqBin,regsIndExcluidos[i]->byteOffset + 5, SEEK_SET);
            fwrite(&regsIndExcluidos[i-1]->byteOffset,8,1,arqBin);
        }

    }

    if(tipoArq == 2)
        excluirIndices(arqInd,nomeArqInd,regsIndExcluidos,(qtdExcluidos + nroRegRem),tipoArq);
    else
        excluirIndices(arqInd,nomeArqInd,regsIndExcluidos,qtdExcluidos,tipoArq);

    return 1;

}

//organiza a lista com os enderecos ja removidos e com a serem removidos, para estar em ordem crescente para a lista de excluidos
int intercalonamentoRegExcluir(RegistroIndice_t **regs,int *tamanhoRegs, int maxRegs)
{
    int k, j, intAux;
    RegistroIndice_t *regAux = malloc(sizeof(RegistroIndice_t));
    inicializarRegistroIndice(regAux);

    for (k = 1; k < maxRegs; k++) {

        for (j = 0; j < maxRegs - 1; j++) {

            if (tamanhoRegs[j] > tamanhoRegs[j + 1]) {
                intAux = tamanhoRegs[j];
                regAux->byteOffset = regs[j]->byteOffset;
                regAux->rrn = regs[j]->rrn;
                regAux->indice = regs[j]->indice;

                tamanhoRegs[j] = tamanhoRegs[j + 1];
                regs[j]->byteOffset = regs[j+1]->byteOffset;
                regs[j]->rrn = regs[j+1]->rrn;
                regs[j]->indice = regs[j+1]->indice;

                tamanhoRegs[j + 1] = intAux;
                regs[j+1]->byteOffset = regAux->byteOffset;
                regs[j+1]->rrn = regAux->rrn;
                regs[j+1]->indice = regAux->indice;

                inicializarRegistroIndice(regAux);
            }
        }
    }
    if(regAux)
        free(regAux);
    return 1;

}

//funcao para pegar a fila que tem no topo, somente tipo 2
int pegarFilaTopo(FILE *arqBin,RegistroIndice_t **regsIndExc, int *tamnRegsIndExc ,int qtdExc, int comecoReg)
{
    //pega o topo
    fseek(arqBin,1,SEEK_SET);
    fread(&regsIndExc[qtdExc+comecoReg-1]->byteOffset,8,1,arqBin);
    fseek(arqBin,regsIndExc[qtdExc+comecoReg-1]->byteOffset + 1,SEEK_SET);
    fread(&tamnRegsIndExc[qtdExc+comecoReg-1],sizeof(int),1,arqBin);
    for (int i = 1; i < qtdExc; ++i) {//pega o restante da fila
        fread(&regsIndExc[qtdExc + comecoReg - i - 1]->byteOffset,8,1,arqBin);
        fseek(arqBin,regsIndExc[qtdExc + comecoReg - i - 1]->byteOffset + 1,SEEK_SET);
        fread(&tamnRegsIndExc[qtdExc + comecoReg - i - 1],sizeof(int),1,arqBin);
    }
    return 1;
}

//funcao para excluir os indices do arquivo de indices
int excluirIndices(FILE *arqInd,char *stringArqInd ,RegistroIndice_t **regsExcluir, int qtdRegsExcluir, int tipoArq)
{
    if(!arqInd || !stringArqInd || !regsExcluir || qtdRegsExcluir <= 0 || tipoArq > 2 || tipoArq < 1)
        return -1;

    mudarStatus(arqInd,0);

    fseek(arqInd,1,SEEK_SET);

    //tamanho para cada tipo de arquivo
    int tamRegInd = sizeof(int)  * 2;
    if(tipoArq == 2)
        tamRegInd = sizeof (int) + 8;

    //pega a quantidade de indices no arquivo
    fseek(arqInd,0,SEEK_END);
    long int maxByteOffset = ftell(arqInd) - 1;
    long int qtdRegsInd = maxByteOffset/tamRegInd;

    //organiza em ordem crescente de indice os indices
    quickSort(regsExcluir,0,qtdRegsExcluir-1);

    int regExcluirIndice = 0;
    RegistroIndice_t **indicesArq = malloc(sizeof(RegistroIndice_t) * qtdRegsInd);
    fseek(arqInd,1,SEEK_SET);

    for (int i = 0; i < qtdRegsInd; ++i)//le todos os indices do arquivo e colocam no indicesArq
    {
        indicesArq[i] = malloc(sizeof(RegistroIndice_t));
        inicializarRegistroIndice(indicesArq[i]);
        fread(&indicesArq[i]->indice,sizeof(int),1,arqInd);
        if(tipoArq == 1)
            fread(&indicesArq[i]->rrn,sizeof(int),1,arqInd);
        else
            fread(&indicesArq[i]->byteOffset,8,1,arqInd);

        //caso seja um indice para excluir, ele marca o indice para ser apagado
        if(tipoArq == 1 && regExcluirIndice < qtdRegsExcluir && regsExcluir[regExcluirIndice]->indice <= indicesArq[i]->indice)
        {
            if(regsExcluir[regExcluirIndice]->indice == indicesArq[i]->indice)
            {
                indicesArq[i]->rrn = -1;
                indicesArq[i]->indice = -1;
            }
            regExcluirIndice++;
        }else if(tipoArq == 2 && regExcluirIndice < qtdRegsExcluir && regsExcluir[regExcluirIndice]->indice <= indicesArq[i]->indice)
        {
            if(regsExcluir[regExcluirIndice]->indice == indicesArq[i]->indice)
            {
                indicesArq[i]->byteOffset = -1;
                indicesArq[i]->indice = -1;
            }
            regExcluirIndice++;

        }
    }

    //cria um arquivo auxiliar, ele sera renomeado corretamente em outra funcao
    FILE *fileAux = fopen("auxInd.bin", "wb");
    int status = '0';
    fwrite(&status, 1,1,fileAux);

    //escreve os indices nao excluidos no arquivo auxiliar
    for (int i = 0; i < qtdRegsInd; ++i) {
        if(tipoArq == 1 && indicesArq[i]->rrn >= 0)
        {
            fwrite(&indicesArq[i]->indice,sizeof(int),1,fileAux);
            fwrite(&indicesArq[i]->rrn,sizeof(int),1,fileAux);
        }else if(tipoArq == 2 && indicesArq[i]->byteOffset > 0)
        {
            fwrite(&indicesArq[i]->indice,sizeof(int),1,fileAux);
            fwrite(&indicesArq[i]->byteOffset,8,1,fileAux);
        }

    }

    //libera as memorias
    for (int i = 0; i < qtdRegsInd; ++i) {
        if(indicesArq[i])
            free(indicesArq[i]);
    }
    free(indicesArq);

    mudarStatus(arqInd, 1);
    mudarStatus(fileAux, 1);
    fclose(fileAux);
    return 1;
}