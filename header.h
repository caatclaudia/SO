#include <fcntl.h>
#include <time.h>
int nmaxnot = 5;
int nmaxmsg = 15;
char fileWN[20]="palavras.txt";
char namebd[20] =  "medit.db";	//ACRESCENTEI
#define MAXMSG nmaxmsg
#define WORDSNOT fileWN
#define MAXNOT nmaxnot
#define MAXCHAR 1000
#define TIMEOUT 10
#define TAM 50
#define NLINHAS 15
#define NCOLUNAS 67

#define FIFO_SERV "SERV"
#define FIFO_CLI "CLI%d"

typedef struct{		//ACRESCENTEI
   int ativo;
   int ncliativos;
}Servidor;

typedef struct{		//ACRESCENTEI
   int hora,min,seg;
}Data;

typedef struct{		//ACRESCENTEI
  int remetente;
  char nome[20];
  int  resposta;
  char npinter[20];
  int acesso;//1 login 0 logout
  Data inicioLogin;
  int nlinhas;
}Login;

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
	char frase[MAXCHAR];
	int remetente;
}PEDIDO;