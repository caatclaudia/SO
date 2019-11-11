#include <fcntl.h>
int nmaxnot = 5;
int nmaxmsg = 15;
char fileWN[20]="palavras.txt";
#define MAXMSG nmaxmsg
#define WORDSNOT fileWN
#define MAXNOT nmaxnot
#define MAXCHAR 1000
#define TIMEOUT 10
#define TAM 50
#define NLINHAS 15
#define NCOLUNAS 67
#define FIFO_SERV    "SERV"
#define FIFO_CLI    "CLI%d" // pid do cliente

typedef struct utilizador{
    char username[TAM];
    int IDuser;
}user;

typedef struct servidor{
    int nclientes;
}server;

typedef struct mensagem{
    char corpo[MAXCHAR];
    char topico[TAM];
    char titulo[TAM];
    int duracao;
}msg;

typedef struct editar{
    msg *texto;
    int linha; //y
    int coluna; //x
}edit;

typedef struct pedido{
	char palavra[20];
	int remetente;
}PEDIDO;