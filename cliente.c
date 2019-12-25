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
	printf("\n1.Escrever nova mensagem\n");
	printf("2.Consultar lista de topicos\n");
	printf("3.Consultar lista de titulos de um topico\n");
	printf("4.Consultar uma mensagem de um topico\n");
	printf("5.Subscrever/Cancelar subscricao de um topico\n");
	printf("6.Sair\n");
	printf("Opcao: ");
	return ;
}

void iniciaMensagens(Msg mensagens[]){
    for(int i=0; i<nmaxmsg; i++){
	mensagens[i].remetente=-1;
	strcpy(mensagens[i].corpo," ");
	strcpy(mensagens[i].topico," ");
	strcpy(mensagens[i].titulo," ");
	mensagens[i].duracao=-1;
    }
}

void adicionaMensagem(Msg mensagens[], int n, Msg msg){
	mensagens[n-1]=msg;
}

void listaTopicos(Msg mensagens[], int totalMensagens){
    Msg aux[nmaxmsg];
    iniciaMensagens(aux);
    int topicos=0, EXISTE=-1;
    
    for(int i=0; i<totalMensagens; i++){
	EXISTE=0;
	for(int j=0; j<topicos; j++)
	    if(strcmp(mensagens[i].topico,aux[j].topico)==0)
		EXISTE=1;
	if(EXISTE==0){
	    topicos++;
	    adicionaMensagem(aux, topicos, mensagens[i]);
	}
    }	
    if(EXISTE==-1)
	printf("\n\nNao ha topicos!\n");
    else{
	printf("\n\nTopicos: \n");
	for(int j=0; j<topicos; j++)
	    printf("-%s\n", aux[j].topico);
    }
    return ;
}

void titulosTopico(Msg mensagens[], int n, char topico[]){
    int EXISTE=0;
    for(int i=0; i<n; i++){
	if(strcmp(mensagens[i].topico,topico)==0){
	    printf("   Mensagem %d - Titulo: %s\n\n", i+1, mensagens[i].titulo);
	    EXISTE=1;
	}
    }
    if(!EXISTE)
	printf("\n\nNao ha titulos deste topico!\n");
}

void consultarTitulos(Msg mensagens[], int totalMensagens){
	char topico[20];
	printf("\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	titulosTopico(mensagens, totalMensagens, topico);
	return ;
}

void mensagensTopico(Msg mensagens[], int n, char topico[]){
    int EXISTE=0;
    for(int i=0; i<n; i++){
	if(strcmp(mensagens[i].topico,topico)==0){
	    printf("   Mensagem %d - Titulo: %s\n", i+1, mensagens[i].titulo);
	    printf("   Mensagem: %s\n\n", mensagens[i].corpo);
	    EXISTE=1;
	}
    }
    if(!EXISTE)
	printf("\n\nNao ha mensagens deste topico!\n");
}

void consultarMensagem(Msg mensagens[], int totalMensagens){
	char topico[20];
	printf("\n\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	mensagensTopico(mensagens, totalMensagens, topico);

	return ;
}

void subscreverTopico(Login cli){
	int i;	
	char topico[20];
	printf("\n\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	for(i=0; i<nmaxmsg && cli.subscricoes[i].nome!=" "; i++);
	strcpy(cli.subscricoes[i].nome,topico);	

	return ;
}

void cancelarTopico(Login cli){
	char topico[20];
	printf("\n\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	for(int i=0; i<nmaxmsg && cli.subscricoes[i].nome!=" "; i++){
		if(strcmp(cli.subscricoes[i].nome,topico)==0){
			for(int j=i; j<nmaxmsg; j++)
				cli.subscricoes[j]=cli.subscricoes[j+1];
			strcpy(cli.subscricoes[nmaxmsg-1].nome, " ");
		}	
	}
	return ;
}

void subscreverOuCancelar(Login cli){
	int op;
	do{
		printf("\n\n1.Subscrever topico novo\n");
		printf("2.Cancelar subscricao de topico\n");
		printf("3.Voltar\n");		
		printf("Opcao: ");
		scanf(" %d", &op);
		clean_input();
	}while(op<1 || op>3);
	if(op==1){//SUBSCREVER TOPICO
		subscreverTopico(cli); 	
	}
	else if(op==2){//CANCELAR SUBSCRICAO
		cancelarTopico(cli); 	
	}
	return ;
}

int fd_cli;
char fifo_name[20];

void trataSig(int i){
    int fd,res;
    Login c;
	printf("\n\n[CLIENTE VAI DESLIGAR]\n\n");
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
        int fd_ser, op, res, totalMensagens=0;
	Login cli;
	Msg mensagens[nmaxmsg];

	iniciaMensagens(mensagens);

	if(argc!=2){
		return 0;
		printf("[ERRO] Falta o nome de utilizador!\n");
	}
	if(access(FIFO_SERV, F_OK)!=0) {
        	printf("[ERRO] Nao ha nenhum servidor!\n");
        	return EXIT_FAILURE;
	}
	sprintf(fifo_name, FIFO_CLI, getpid());
	
	if(access(fifo_name, F_OK)==0) {
       		fprintf(stderr, "[ERRO] Cli ja existe.\n");
         	return EXIT_FAILURE;
	 }
	if(mkfifo(fifo_name, 0600) == -1){
		perror("\n[ERRO] mkfifo do FIFO do cliente deu erro");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGINT,trataSig) == SIG_ERR){
	  perror("\n[ERRO] Não foi possivel configurar o sinal SIGINT\n");
	  exit(EXIT_FAILURE);
	}

	cli.remetente = getpid();
        cli.acesso = 1;
	cli.primeiro = 1;
	strcpy(cli.nome,argv[1]);
	for(int i=0; i<nmaxmsg; i++)
		strcpy(cli.subscricoes[i].nome," ");

	fd_ser = open(FIFO_SERV, O_WRONLY); // escrita

	fd_cli = open(fifo_name, O_RDWR);
        res = write(fd_ser,&cli,sizeof(Login));
	res = read(fd_cli,&cli,sizeof(Login));
  	
        printf("\n\nUtilizador: %s\n\n", cli.nome);

	cli.primeiro=0;

	do{
		do{		
			menu();
			scanf(" %d", &op);
			clean_input();
		}while(op<1 || op>6);

		if(op==1){//ESCREVER MENSAGEM
			Msg nova;
		    	nova.remetente = getpid();
			nova.resposta=0;
			printf("\n\nTopico>> ");
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
			fgets(nova.corpo,MAXCHAR,stdin);
			if(nova.corpo[strlen(nova.corpo)-1]=='\n')
				nova.corpo[strlen(nova.corpo)-1]='\0';
			fflush(stdin);
			write(fd_ser,&cli,sizeof(Login));
			write(fd_ser, &nova, sizeof(Msg));
	
			read(fd_cli, &nova, sizeof(Msg));
			fflush(stdout);	
			
			if(nova.resposta==0)
				printf("\n\nMensagem nao foi guardada!\n");
			else{
				totalMensagens++;
				adicionaMensagem(mensagens, totalMensagens, nova);
				printf("\n\nMensagem %d guardada!\n", nova.resposta);

			}	
		}
		else if(op==2){//CONSULTAR TOPICOS
			listaTopicos(mensagens, totalMensagens); 
		}
		else if(op==3){//CONSULTAR TITULOS DE UM TOPICO
			consultarTitulos(mensagens, totalMensagens); 
		}
		else if(op==4){//CONSULTAR MENSAGEM DE UM TOPICO
			consultarMensagem(mensagens, totalMensagens); 
		}	
		else if(op==5){//SUBSCREVER/CANCELAR SUBSCRICAO DE TOPICO
			subscreverOuCancelar(cli); 
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
