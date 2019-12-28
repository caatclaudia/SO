#include <fcntl.h>
#include <time.h>
int nmaxnot = 5;
int nmaxmsg = 15;
int maxusers = 10;
char fileWN[20]="palavras.txt";
#define MAXMSG nmaxmsg
#define WORDSNOT fileWN
#define MAXNOT nmaxnot
#define MAXUSERS maxusers
#define MAXCHAR 1000
#define TIMEOUT 10
#define TAM 50
#define NLINHAS 15
#define NCOLUNAS 67

#define FIFO_SERV "SERV"
#define FIFO_CLI "CLI%d"
#define FIFO_ATU "ATU"

typedef struct{		//ACRESCENTEI
   int ativo;
   int nmensagens;
   int ncliativos;
}Server;

typedef struct{
   char nome[TAM];
}Topic;

typedef struct{		//ACRESCENTEI
  int remetente;
  char nome[20];
  int primeiro;//1 inicio ou 0
  int acesso;//1 login 0 logout
  Topic subscricoes[15];
}Login;

typedef struct mensagem{
    char corpo[MAXCHAR];
    char topico[TAM];
    char titulo[TAM];
    int duracao;
    int resposta;
    int remetente;
}Msg;

typedef struct editar{
    Msg *texto;
    int linha; //y
    int coluna; //x
}Edit;