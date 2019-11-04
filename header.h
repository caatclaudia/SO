int nmaxnot = 5;
int nmaxmsg = 15;

#define MAXMSG nmaxmsg
#define WORDSNOT "palavras.txt"
#define MAXNOT nmaxnot
#define MAXCHAR 1000
#define TIMEOUT 10
#define TAM 50
#define NLINHAS 15
#define NCOLUNAS 67

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