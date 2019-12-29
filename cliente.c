#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<signal.h>
#include "header.h"

Login cli;
Topic topicos[50];
int ntopicos;
int totalMensagens;

int scanfInteiro(){
  int inteiro, fim=0;
  char tmp;
  while (fim < 1) {
    if(scanf(" %d", &inteiro)== 1) 
        fim++;
    scanf("%c", &tmp);
  }
  return inteiro;
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
    ntopicos=0;
    totalMensagens=0;
    return ;
}

void adicionaTopico(char topico[]){
	for(int i=0; i<ntopicos; i++)
		if(strcmp(topicos[i].nome,topico)==0)
			return ;
	ntopicos++;
	strcpy(topicos[ntopicos-1].nome,topico);
}

void adicionaMensagem(Msg mensagens[], Msg msg){
	mensagens[totalMensagens-1]=msg;
	adicionaTopico(msg.topico);
	return ;
}

void listaTopicos(){
    if(ntopicos==0){
	printf("Nao ha topicos!\n");
	return ;
    }
    printf("Topicos: \n");
    for(int i=0; i<ntopicos; i++)
	printf("-%s\n", topicos[i].nome);
    return ;
}

void apagaTopicos(){
    for(int i=0; i<ntopicos; i++)
	strcpy(topicos[i].nome," ");
    ntopicos=0;
    return ;
}

void titulosTopico(Msg mensagens[], char topico[]){
    int EXISTE=0;
    for(int i=0; i<totalMensagens; i++){
	if(strcmp(mensagens[i].topico,topico)==0){
	    printf("   Mensagem %d - Titulo: %s\n\n", i+1, mensagens[i].titulo);
	    EXISTE=1;
	}
    }
    if(!EXISTE)
	printf("\n\nNao ha titulos deste topico!\n");
    return ;
}

void consultarTitulos(Msg mensagens[]){
	char topico[20];
	printf("\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	titulosTopico(mensagens, topico);
	return ;
}

void mensagensTopico(Msg mensagens[], char topico[]){
    int EXISTE=0;
    for(int i=0; i<totalMensagens; i++){
	if(strcmp(mensagens[i].topico,topico)==0){
	    printf("   Mensagem %d - Titulo: %s\n", i+1, mensagens[i].titulo);
	    printf("   Mensagem: %s\n\n", mensagens[i].corpo);
	    EXISTE=1;
	}
    }
    if(!EXISTE)
	printf("\n\nNao ha mensagens deste topico!\n");
    return ;
}

void consultarMensagem(Msg mensagens[]){
	char topico[20];
	printf("\n\nTopico>> ");
	fgets(topico,20,stdin);
	if(topico[strlen(topico)-1]=='\n')
   		topico[strlen(topico)-1]='\0';
	fflush(stdin);

	mensagensTopico(mensagens, topico);
	return ;
}

int subscreveEsteTopico(char topico[]){
	for(int i=0; strcmp(cli.subscricoes[i].nome," ")!=0; i++)
		if(strcmp(cli.subscricoes[i].nome,topico)==0)
			return 1;
	return 0;
}

void subscreverTopico(char topico[]){
	int i, EXISTE=0;	

	if(subscreveEsteTopico(topico)){
		printf("\nJa subscreve este topico\n");
		return ;
	}
	for(i=0; i<ntopicos; i++)
		if(strcmp(topicos[i].nome,topico)==0)
			EXISTE=1;
	if(EXISTE){
		for(i=0; i<nmaxmsg && strcmp(cli.subscricoes[i].nome," ")!=0; i++);
		strcpy(cli.subscricoes[i].nome,topico);	
		printf("\nSubscricao do topico '%s'\n", topico);
	}
	else
		printf("\nEste topico nao existe!\n\n");
	return ;
}

void cancelarTopico(char topico[]){
	if(!subscreveEsteTopico(topico)){
		printf("\nNao subscreve este topico\n");
		return ;
	}

	for(int i=0; i<nmaxmsg && strcmp(cli.subscricoes[i].nome," ")!=0; i++){
		if(strcmp(cli.subscricoes[i].nome,topico)==0){
			for(int j=i; j<nmaxmsg; j++)
				cli.subscricoes[j]=cli.subscricoes[j+1];
			strcpy(cli.subscricoes[nmaxmsg-1].nome, " ");
		}	
	}
	printf("\nCancelada subscricao do topico '%s'\n", topico);
	
	return ;
}

void verificaTopicos(){
	int i, j;
	for(i=0; strcmp(cli.subscricoes[i].nome," ")!=0; i++){
		int EXISTE=0;
		for(j=0; j<ntopicos && EXISTE==0; j++)
			if(strcmp(topicos[j].nome,cli.subscricoes[i].nome)==0)
				EXISTE=1;
		if(EXISTE==0){
			printf("\nCancelada subscricao a topico '%s'\n", cli.subscricoes[i].nome);
			cancelarTopico(cli.subscricoes[i].nome);
		}
	}
	return ;
}

void subscreverOuCancelar(){
	int op;
	do{
		printf("\n\n1.Subscrever topico novo\n");
		printf("2.Cancelar subscricao de topico\n");
		printf("3.Voltar\n");		
		printf("Opcao: ");
		op=scanfInteiro();
	}while(op<1 || op>3);
	if(op==1){//SUBSCREVER TOPICO
		char topico[20];
		printf("\n\nTopico>> ");
		scanf(" %s", topico);
		fflush(stdin);
		subscreverTopico(topico); 	
	}
	else if(op==2){//CANCELAR SUBSCRICAO
		char topico[20];
		printf("\n\nTopico>> ");
		scanf(" %s", topico);
		fflush(stdin);
		cancelarTopico(topico); 	
	}
	return ;
}

int fd_cli;
char fifo_name[20];

void sair(int n){
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

void trataSig(int i){
    sair(i);
}

int main(int argc, char *argv[]){
	int ntopicos=0;
	char nome[20], fifo_name1[20];
        int fd_ser, op, res, fd_atu;
	Msg mensagens[nmaxmsg];

	struct sigaction act;
	act.sa_handler=sair;
	act.sa_flags=0;		//usar funcao void sair(int)
	sigaction(SIGALRM, &act, NULL);

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

	read(fd_cli,&totalMensagens,sizeof(int));
	for(int i =0;i<totalMensagens;i++){			//AQUI
		res = read(fd_cli,&mensagens[i],sizeof(Msg));
	}
  	
        printf("\n\nUtilizador: %s\n\n", cli.nome);

	cli.primeiro=0;

	do{
		op=0;
		do{		
			menu();
			alarm(10);
			op=scanfInteiro();
			alarm(0);
		}while(op<1 || op>6);
		if(SIGALRM==0)
			sair(0);
	
		if(op>=2 && op<=4){
		        fd_atu = open(FIFO_ATU, O_WRONLY);
			write(fd_atu,&cli,sizeof(Login));
			write(fd_atu,&totalMensagens,sizeof(int));
			int num=totalMensagens;
			read(fd_cli,&totalMensagens,sizeof(int));
			for(int i =num;i<totalMensagens;i++){
				read(fd_cli,&mensagens[i],sizeof(Msg));
				if(subscreveEsteTopico(mensagens[i].topico))	//NAO ENTR
					printf("\nNova mensagem %s do topico %s disponivel durante %d!\n\n", 
					mensagens[i].titulo, mensagens[i].topico, mensagens[i].duracao);
			}
			apagaTopicos();			
			read(fd_cli,&ntopicos,sizeof(int));
			for(int i =0;i<ntopicos;i++){
				read(fd_cli,&topicos[i],sizeof(Topic));
				adicionaTopico(topicos[i].nome);
			}
			close(fd_atu);
			verificaTopicos();
		}

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
			nova.duracao=scanfInteiro();
		
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
				adicionaMensagem(mensagens, nova);
				printf("\n\nMensagem %d guardada!\n", nova.resposta);

			}	
		}
		else if(op==2){//CONSULTAR TOPICOS
			listaTopicos(); 
		}
		else if(op==3){//CONSULTAR TITULOS DE UM TOPICO
			consultarTitulos(mensagens); 
		}
		else if(op==4){//CONSULTAR MENSAGEM DE UM TOPICO
			consultarMensagem(mensagens); 
		}	
		else if(op==5){//SUBSCREVER/CANCELAR SUBSCRICAO DE TOPICO
			subscreverOuCancelar(); 
		}
		fflush(stdout);
	}while(op!=6);

	cli.acesso=0;
	write(fd_ser,&cli,sizeof(Login));

	close(fd_atu);
	close(fd_cli);
	close(fd_ser);
	unlink(fifo_name);

	return EXIT_SUCCESS;
}
