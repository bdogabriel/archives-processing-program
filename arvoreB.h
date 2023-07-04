// Gabriel Barbosa de Oliveira - 12543415
// Felipe Seiji Momma Valente - 12543700

typedef struct regIndiceArvoreB
{
    char tipoNo;
    int nroChaves;
    int chaves[4];
    long int byteOffset[4];
    int rrn[4];
    int prox[5];
    long int endReg;
} RegIndiceArvoreB_t; // struct dos indices da arvore B, rrn e byteoffset servem para apontar para o arquivo de dados

int printBuscaIdArvoreB(char *nomeArqBin, char *nomeArqIndArvB, int id, int tipoArq);//printa o registro de acordo com o id passado
int inserirValoresArvB(FILE *arqIndArvB,int idInserir ,int rrnInserir, long int byteOffsetInserir, int tipoArq);//insere um valor na arvore b
int criarIndArvB(char *nomeArqBin, char *nomeArqArvB, int tipoArq);//cria um arquivo de indice da arvore b de um arquivo
