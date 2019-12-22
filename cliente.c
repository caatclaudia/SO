#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include "header.h"

void clean_input(void) {
    while (getchar()!='\n');
    return;
}

void menu(){
	printf("1.Escrever nova mensagem\n");
	printf("2.Consultar lista de topicos\n");
	printf("3.Consultar lista de titulos\n");
	printf("4.Consultar uma mensagem de um topico\n");
	printf("5.Subscrever/Cancelar subscricao de um topico\n");
	printf("6.Sair\n");
	printf("Opcao: ");
	return ;
}

void consultarTopicos(){

	//LISTA DE TOPICOS
	return;
}

void consultarTitulos(){
	char topico[20];
	printf("\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	//LISTA DE TITULOS DESTE TOPICO
	return ;
}

void consultarMensagem(){
	char topico[20];
	printf("\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	//CONSULTAR MENSAGEM DESTE TOPICO
	return ;
}

void subscreverTopico(){
	char topico[20];
	printf("\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	//SUBSCREVER TOPICO
	return ;
}

void cancelarTopico(){
	char topico[20];
	printf("\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	//CANCELAR SUBSCRICAO
	return ;
}

void subscreverOuCancelar(){
	int op;
	do{
		printf("1.Subscrever topico novo\n");
		printf("2.Cancelar subscricao de topico\n");
		printf("3.Voltar\n");		
		printf("Opcao: ");
		scanf(" %d", &op);
		clean_input();
	}while(op<1 || op>3);
	if(op==1){
		subscreverTopico(); 	//SUBSCREVER TOPICO
	}
	else if(op==2){
		cancelarTopico(); 	//CANCELAR SUBSCRICAO
	}
	return ;
}

int fd_cli;
char fifo_name[20];

void trataSig(int i){
    int fd,res;
    Login c;
	printf("\n[CLIENTE VAI DESLIGAR]\n");
    c.remetente = getpid();
    c.acesso = 0;
    fd = open(FIFO_SERV,O_WRONLY);
    res = write(fd,&c,sizeof(Login));
    close(fd);
    close(fd_cli);
    unlink(fifo_name);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	
	char nome[20];
        int fd_ser, op, res;
	PEDIDO p;
	Login cli, c;

	if(access(FIFO_SERV, F_OK)!=0) {
        	printf("[ERRO] Nao ha nenhum servidor!\n");
        	return EXIT_FAILURE;
	}
	sprintf(fifo_name, FIFO_CLI, getpid());
    	p.remetente = getpid();
	
	if(access(fifo_name, F_OK)==0) {
       		fprintf(stderr, "[ERROR] Cli ja existe.\n");
         	return EXIT_FAILURE;
	 }
	if(mkfifo(fifo_name, 0600) == -1){
		perror("\nmkfifo do FIFO do cliente deu erro");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGINT,trataSig) == SIG_ERR){
	  perror("\nNão foi possivel configurar o sinal SIGINT\n");
	  exit(EXIT_FAILURE);
	}

	printf("Introduza o seu username: ");
	scanf(" %s",nome);
    cli.remetente = getpid();
    cli.acesso = 1;
	cli.primeiro = 1;
	strcpy(cli.nome,nome);

	fd_ser = open(FIFO_SERV, O_WRONLY); // escrita

	fd_cli = open(fifo_name, O_RDWR);
    res = write(fd_ser,&cli,sizeof(Login));
	res = read(fd_cli,&c,sizeof(Login));
  	if(c.resposta == 0){
          printf("\nUtilizador nao tem permissao!\n");
		exit(0);
	}

	cli.primeiro=0;

	do{
		do{		
			menu();
			scanf(" %d", &op);
			clean_input();
		}while(op<1 || op>6);

		if(op==1){
			msg nova;
			printf("\nTopico>> ");
			fgets(nova.topico,TAM,stdin);
			if(nova.topico[strlen(nova.topico)-1]=='\n')
				nova.topico[strlen(nova.topico)-1]='\0';
			fflush(stdin);
	
			printf("\nTitulo>> ");
			fgets(nova.titulo,TAM,stdin);
			if(nova.titulo[strlen(nova.titulo)-1]=='\n')
				nova.titulo[strlen(nova.titulo)-1]='\0';
			fflush(stdin);

			printf("\nDuracao>> ");	
			scanf(" %d", &nova.duracao);
			clean_input();
		
			printf("\nCorpo>> ");
			fgets(p.frase,MAXCHAR,stdin);
			if(p.frase[strlen(p.frase)-1]=='\n')
				p.frase[strlen(p.frase)-1]='\0';
			fflush(stdin);
			strcpy(nova.corpo,p.frase);
			write(fd_ser,&cli,sizeof(Login));
			write(fd_ser, &p, sizeof(PEDIDO));
	
			read(fd_cli, &p, sizeof(PEDIDO));
			fflush(stdout);	
		
			//ESCREVER MENSAGEM
		}
		else if(op==2){
			consultarTopicos(); //CONSULTAR TOPICOS
		}
		else if(op==3){
			consultarTitulos(); //CONSULTAR TITULOS
		}
		else if(op==4){
			consultarMensagem(); //CONSULTAR MENSAGEM DE UM TOPICO
		}	
		else if(op==5){
			subscreverOuCancelar(); //SUBSCREVER/CANCELAR SUBSCRICAO DE TOPICO
		}
		fflush(stdout);
	}while(op!=6);

	cli.acesso=0;
	write(fd_ser,&cli,sizeof(Login));

	close(fd_cli);
	close(fd_ser);
	unlink(fifo_name);

	return EXIT_SUCCESS;
}
