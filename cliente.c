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

int main(int argc, char *argv[]){
	
	char fifo_name[20];
        int fd_ser, fd_cli, op;
	PEDIDO p;

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

	fd_ser = open(FIFO_SERV, O_WRONLY); // escrita
	
	fd_cli = open(fifo_name, O_RDWR);

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

	close(fd_cli);
	close(fd_ser);
	unlink(fifo_name);

	return EXIT_SUCCESS;
}
