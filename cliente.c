#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<signal.h>
#include<ncurses.h>
#include "header.h"

#define MEIO 10
#define FIM 20

#define TECLA_SETA_UP 3
#define TECLA_SETA_DOWN 2
#define TECLA_SETA_LEFT 4
#define TECLA_SETA_RIGHT 5
#define TECLA_ENTER 10

Login cli;
Topic topicos[50];
int ntopicos;
int totalMensagens;

int scanfInteiro(){
  int inteiro, fim=0;
  while (fim < 1) {
    if(scanw(" %d", &inteiro)== 1) 
        fim++;
  }
  return inteiro;
}

void limpaFirst(){
	for(int i=1; i<MEIO; i++){
		mvprintw(i,0, "                                                ");
		refresh();
	}
}

void limpaSec(){
	for(int i=MEIO; i<FIM; i++){
		mvprintw(i,0, "                                                ");
		refresh();
	}
}

void menu(){
	int i=1;
	mvprintw(i++,0,"1.Escrever nova mensagem");
	mvprintw(i++,0,"2.Consultar lista de topicos");
	mvprintw(i++,0,"3.Consultar lista de titulos de um topico");
	mvprintw(i++,0,"4.Consultar uma mensagem de um topico");
	mvprintw(i++,0,"5.Subscrever/Cancelar subscricao de um topico");
	mvprintw(i++,0,"6.Sair");
	mvprintw(i++,0,"Opcao: ");
	refresh();
	return ;
}

void iniciaMensagens(Msg mensagens[]){
    for(int i=0; i<nmaxmsg; i++){
	mensagens[i].remetente=-1;
	strcpy(mensagens[i].corpo," ");
	strcpy(mensagens[i].topico," ");
	strcpy(mensagens[i].titulo," ");
	mensagens[i].duracao=-1;
	mensagens[i].termina=0;
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
	int j=MEIO;
    if(ntopicos==0){
	mvprintw(j++,0,"Nao ha topicos!");
	refresh();
	return ;
    }
    mvprintw(j++,0,"Topicos:");
    for(int i=0; i<ntopicos; i++){
	if(j==FIM){
		sleep(2);
		j=MEIO;
	}	
	mvprintw(j++,0,"-%s", topicos[i].nome);
	refresh();
    }
    return ;
}

void apagaTopicos(){
    for(int i=0; i<ntopicos; i++)
	strcpy(topicos[i].nome," ");
    ntopicos=0;
    return ;
}

void titulosTopico(Msg mensagens[], char topico[]){
    int EXISTE=0, s=MEIO+1;
    for(int i=0; i<totalMensagens; i++){
	if(strcmp(mensagens[i].topico,topico)==0){
	    if(s==FIM){
		sleep(2);
		s=MEIO;
	    }
	    mvprintw(s++,0,"   Mensagem %d - Titulo: %s", i+1, mensagens[i].titulo);
	    refresh();
	    EXISTE=1;
	}
    }
    if(!EXISTE)
	mvprintw(s++,0,"Nao ha titulos deste topico!");
    refresh();
    return ;
}

void consultarTitulos(Msg mensagens[]){
	char topico[20];
	mvprintw(MEIO+1,0,"Topico>> ");
	refresh();
	getstr(topico);

	titulosTopico(mensagens, topico);
	return ;
}

void mensagensTopico(Msg mensagens[], char topico[]){
    int EXISTE=0, s=MEIO+1;
    for(int i=0; i<totalMensagens; i++){
	if(strcmp(mensagens[i].topico,topico)==0){
	    if(s==FIM){
		sleep(2);
		s=MEIO;
	    }
	    mvprintw(s++,0,"   Mensagem %d - Titulo: %s", i+1, mensagens[i].titulo);
	    mvprintw(s++,0,"   Mensagem: %s", mensagens[i].corpo);
	    refresh();
	    EXISTE=1;
	}
    }
    if(!EXISTE)
	mvprintw(s++,0,"Nao ha mensagens deste topico!");
    refresh();
    return ;
}

void consultarMensagem(Msg mensagens[]){
	char topico[20];
	mvprintw(MEIO+1,0,"Topico>> ");
	refresh();
	getstr(topico);

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
		mvprintw(MEIO+1,0,"Ja subscreve este topico!");
		refresh();
		return ;
	}
	for(i=0; i<ntopicos; i++)
		if(strcmp(topicos[i].nome,topico)==0)
			EXISTE=1;
	if(EXISTE){
		for(i=0; i<nmaxmsg && strcmp(cli.subscricoes[i].nome," ")!=0; i++);
		strcpy(cli.subscricoes[i].nome,topico);	
		mvprintw(MEIO+1,0,"Subscricao do topico '%s'", topico);
	}
	else
		mvprintw(MEIO+1,0,"Este topico nao existe!");
	refresh();
	return ;
}

void cancelarTopico(char topico[]){
	if(!subscreveEsteTopico(topico)){
		mvprintw(MEIO+1,0,"Nao subscreve este topico!");
		refresh();
		return ;
	}

	for(int i=0; i<nmaxmsg && strcmp(cli.subscricoes[i].nome," ")!=0; i++){
		if(strcmp(cli.subscricoes[i].nome,topico)==0){
			for(int j=i; j<nmaxmsg; j++)
				cli.subscricoes[j]=cli.subscricoes[j+1];
			strcpy(cli.subscricoes[nmaxmsg-1].nome, " ");
		}	
	}
	mvprintw(MEIO+1,0,"Cancelada subscricao do topico '%s'", topico);
	refresh();
	return ;
}

void verificaTopicos(){
	int i, j, s=MEIO;
	for(i=0; strcmp(cli.subscricoes[i].nome," ")!=0; i++){
		int EXISTE=0;
		for(j=0; j<ntopicos && EXISTE==0; j++)
			if(strcmp(topicos[j].nome,cli.subscricoes[i].nome)==0)
				EXISTE=1;
		if(EXISTE==0){
			if(s==FIM){
				sleep(2);
				s=MEIO;
			}
			mvprintw(s++,0,"Cancelada subscricao a topico '%s'", cli.subscricoes[i].nome);
			refresh();			
			cancelarTopico(cli.subscricoes[i].nome);
		}

	}
	return ;
}

void subscreverOuCancelar(){
	int op, i=1;
	do{
		limpaFirst();
		mvprintw(i++,0,"1.Subscrever topico novo");
		mvprintw(i++,0,"2.Cancelar subscricao de topico");
		mvprintw(i++,0,"3.Voltar");		
		mvprintw(i++,0,"Opcao: ");
		refresh();
		scanw(" %d", &op);
	}while(op<1 || op>3);
	i++;
	if(op==1){//SUBSCREVER TOPICO
		char topico[20];
		mvprintw(i++,0,"Topico>> ");
		refresh();
		getstr(topico);
		subscreverTopico(topico); 	
	}
	else if(op==2){//CANCELAR SUBSCRICAO
		char topico[20];
		mvprintw(i++,0,"Topico>> ");
		getstr(topico);
		cancelarTopico(topico); 	
	}
	return ;
}

int calculaIndice(int inicio, int y, int x){
	int aux=0, ind;
	if(y==inicio)
		ind=x-1;
	else{
		aux=y-inicio;
		ind=aux*NCOLUNAS+(x-1);
	}
	return ind;
}

int fd_cli;
char fifo_name[20];

void sair(int n){
    int fd,res;
    Login c;
    clear();
    mvprintw(MEIO+1,0,"[CLIENTE VAI DESLIGAR]");
    refresh();
    sleep(1);
    c.remetente = getpid();
    c.acesso = 0;
    fd = open(FIFO_SERV,O_WRONLY);
    res = write(fd,&c,sizeof(Login));
    close(fd);
    close(fd_cli);
    unlink(fifo_name);

    clear();
    endwin();
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
		printf("\n[ERRO] Falta o nome de utilizador!\n");
		exit(EXIT_FAILURE);
	}
	if(access(FIFO_SERV, F_OK)!=0) {
        	printf("\n[ERRO] Nao ha nenhum servidor!\n");
        	exit(EXIT_FAILURE);
	}
	sprintf(fifo_name, FIFO_CLI, getpid());
	
	if(access(fifo_name, F_OK)==0) {
       		printf("\n[ERRO] Cli ja existe.\n");
         	exit(EXIT_FAILURE);
	 }
	if(mkfifo(fifo_name, 0600) == -1){
		printf("\n[ERRO] mkfifo do FIFO do cliente deu erro");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGINT,trataSig) == SIG_ERR){
	  printf("\n[ERRO] Não foi possivel configurar o sinal SIGINT\n");
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
	
	cli.primeiro=0;

	initscr();		//NCURSES
	clear();
	keypad(stdscr, TRUE);
	int x, y;
	char ch;
	int s=0;
	mvprintw(s++,0,"Utilizador %s", cli.nome);
	refresh();

	do{
		limpaSec();
		op=0;
		do{		
			limpaFirst();
			menu();
			alarm(10);
			scanw(" %d", &op);
			alarm(0);
		}while(op<1 || op>6);
		if(SIGALRM==0)
			sair(0);
	
		if(op>=2 && op<=4){
		        fd_atu = open(FIFO_ATU, O_WRONLY);
			write(fd_atu,&cli,sizeof(Login));
			iniciaMensagens(mensagens);
			read(fd_cli,&totalMensagens,sizeof(int));
			for(int i =0;i<totalMensagens;i++){
				read(fd_cli,&mensagens[i],sizeof(Msg));
				if(subscreveEsteTopico(mensagens[i].topico))	//NAO ENTR
					mvprintw(MEIO-1,0,"Nova mensagem %s do topico %s disponivel durante %d!", 
					mensagens[i].titulo, mensagens[i].topico, mensagens[i].duracao);
					refresh();
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
			sleep(1);
			limpaFirst();
			int i=s;
			mvprintw(i++,0,"--Escrever mensagem nova--");
			Msg nova;
			nova.termina=0;
		    	nova.remetente = getpid();
			nova.resposta=0;
			i++;
			mvprintw(i++,0,"Topico>> ");
			refresh();
			getstr(nova.topico);
	
			mvprintw(i++,0,"Titulo>> ");
			refresh();
			getstr(nova.titulo);

			mvprintw(i++,0,"Duracao>> ");	
			refresh();
			nova.duracao=scanfInteiro();
			mvprintw(i-1,10,"%d         ", nova.duracao);
		
			mvprintw(i++,0,"Corpo>> ");
			refresh();
			y=i;
			x=1;
			move(y,x);
			refresh();
			
			for(int a=0; a<MAXCHAR; a++)
				nova.corpo[a]=' ';
			do{
				ch = getch();
				if(ch==TECLA_SETA_UP && y>i){
					y--;
					move(y,x);
					refresh();
				}
				else if(ch==TECLA_SETA_DOWN && y<i+NLINHAS){
					y++;
					move(y,x);
					refresh();
				}
				else if(ch==TECLA_SETA_LEFT && x>2){
					x--;
					move(y,x);
					refresh();
				}
				else if(ch==TECLA_SETA_RIGHT && x<NCOLUNAS){
					int PODE=1;
					if(y==(i+NLINHAS) && x>=NULTIMCOLUNAS)
						PODE=0;
					if(PODE){
						x++;
						move(y,x);
						refresh();
					}
				}
				else{
					int ind=calculaIndice(i, y, x);
					nova.corpo[ind]=ch;
					mvprintw(y, x++, "%c", ch);
				}
				refresh();
			}while(ch!=TECLA_ENTER);
			int fim=MAXCHAR, AINDATEMTEXTO=0;
			for(int a=0; a<MAXCHAR; a++){
				if(nova.corpo[a]==' ' && AINDATEMTEXTO==1){
					fim=a;
					AINDATEMTEXTO=0;
				}
				else if(nova.corpo[a]!=' ')
					AINDATEMTEXTO=1;
			}
			nova.corpo[fim-1]='\0';

			mvprintw(FIM, 0, "%s", nova.corpo); 	

			write(fd_ser,&cli,sizeof(Login));
			write(fd_ser, &nova, sizeof(Msg));
	
			read(fd_cli, &nova, sizeof(Msg));
			
			if(nova.resposta==0)
				mvprintw(FIM+1,0,"Mensagem nao foi guardada!");
			else{
				totalMensagens++;
				adicionaMensagem(mensagens, nova);
				mvprintw(FIM+1,0,"Mensagem %d guardada!", nova.resposta);
			}
			refresh();	
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
		sleep(3);
	}while(op!=6);

	cli.acesso=0;
	write(fd_ser,&cli,sizeof(Login));

	close(fd_atu);
	close(fd_cli);
	close(fd_ser);
	unlink(fifo_name);

	clear();
	endwin();

	return EXIT_SUCCESS;
}
