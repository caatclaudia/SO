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

void adicionaMensagem(Msg mensagens[], int n, Msg msg){
	strcpy(mensagens[n-1].corpo,msg.corpo);
	strcpy(mensagens[n-1].topico,msg.topico);
	strcpy(mensagens[n-1].titulo,msg.titulo);
	mensagens[n-1].duracao=msg.duracao;
	mensagens[n-1].remetente=msg.remetente;
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
	printf("Nao ha mensagens deste topico!\n");
}

void consultarMensagem(Msg mensagens[], int totalMensagens){
	char topico[20];
	printf("\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	//CONSULTAR MENSAGEM DESTE TOPICO
	mensagensTopico(mensagens, totalMensagens, topico);

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

void iniciaMensagens(Msg mensagens[]){
    for(int i=0; i<nmaxmsg; i++){
	mensagens[i].remetente=-1;
	strcpy(mensagens[i].corpo," ");
	strcpy(mensagens[i].topico," ");
	strcpy(mensagens[i].titulo," ");
	mensagens[i].duracao=-1;
    }
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

	fd_ser = open(FIFO_SERV, O_WRONLY); // escrita

	fd_cli = open(fifo_name, O_RDWR);
        res = write(fd_ser,&cli,sizeof(Login));
	res = read(fd_cli,&cli,sizeof(Login));
  	
        printf("\nUtilizador: %s\n", cli.nome);

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
			fgets(nova.corpo,MAXCHAR,stdin);
			if(nova.corpo[strlen(nova.corpo)-1]=='\n')
				nova.corpo[strlen(nova.corpo)-1]='\0';
			fflush(stdin);
			write(fd_ser,&cli,sizeof(Login));
			write(fd_ser, &nova, sizeof(Msg));
	
			read(fd_cli, &nova, sizeof(Msg));
			fflush(stdout);	
			
			if(nova.resposta==0)
				printf("\nMensagem nao foi guardada!\n");
			else{
				totalMensagens++;
				adicionaMensagem(mensagens, totalMensagens, nova);
				printf("\nMensagem %d guardada!\n", nova.resposta);

			}	
		}
		else if(op==2){
			consultarTopicos(); //CONSULTAR TOPICOS
		}
		else if(op==3){
			consultarTitulos(); //CONSULTAR TITULOS
		}
		else if(op==4){
			consultarMensagem(mensagens, totalMensagens); //CONSULTAR MENSAGEM DE UM TOPICO
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
