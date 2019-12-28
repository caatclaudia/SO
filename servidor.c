#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include"header.h"

void comandosmenu(){
    printf("=========Configuracoes=========\n");
    printf("Ver instrucoes novamente (help) \n");
    printf("Ligar/desligar  filtragem de palavras proibidas (filter on / filter off)\n");
    printf("Listar utilizadores (users)\n");
    printf("Listar topicos (topics)\n");
    printf("Listar mensagens (msg)\n");
    printf("Listar mensagens de um topico (topic ____ )\n");
    printf("Apagar mensagem (del ____ ) \n");
    printf("Excluir um utilizador (kick ___ )\n");
    printf("Desligar o gestor (shutdown)\n");
    printf("Eliminar topicos sem mensagens (prune) \n");
    printf("===============================\n\n");
}

void settings(){
    printf("\n===========Configuracoes Iniciais======\n");
    printf("Numero de palavras proibidas: %d\n", nmaxnot);
    printf("Numero maximo de mensagens a armazenar: %d\n", nmaxmsg);
    printf("Numero maximo de utilizadores: %d\n", maxusers);
    printf("Valor de Timeout: %d\n", TIMEOUT);
    printf("Nome de ficheiro de palavras proibidas: %s\n",fileWN);
    printf("=========================================\n\n");
}

void chamaVerificador(char *frase){
	int p[2], estado, num;
	printf("\n\n%s\n\n", frase);

	pipe(p);
	int res=fork();
	if(res==0){
		close(0);//FECHAR ACESSO AO TECLADO
		dup(p[0]);//DUPLICAR P[0] NA PRIMEIRA POSICAO DISPONIVEL
		close(p[0]);//FECHAR EXTREMIDADE DE LEITURA DO PIPE
		close(p[1]);//FECHAR EXTREMIDADE DE ESCRITA DO PIPE
		execl("verificador","verificador",WORDSNOT,NULL);
	}
	close(p[0]);
	write(p[1], frase, strlen(frase));
	write(p[1], "\n", 1);
	close(p[1]);
	wait(&estado);
}

int adicionacliente(Login m[],int *n,Login c){
	int i;
	for(i = 0;i<*n && strcmp(c.nome,m[i].nome)!=0;i++);
	if(i == *n){   
	        m[*n] = c;
	        (*n)++;
	        return 0;
	}
	return 1;
}

int existecliente(Login m[], int n, char nome[]){
    int i;
    for(i=0; i<n; i++){
	if(strcmp(m[i].nome,nome)==0)
        	return 1;
    }
    return 0;
}

void eliminacliente(Login m[],int *n,int pid){
	int i;
	for(i = 0;i<*n && m[i].remetente!=pid;i++);
	if(i!= *n){   
 	       m[i] = m[*n-1];
 	       (*n)--;
	}
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

void listaMensagens(Msg mensagens[], int n){
    int i;
    for(i=0; i<n; i++){
	    printf("   Mensagem %d - Topico: %s\n", i+1, mensagens[i].topico);
	    printf("   Titulo: %s\n", mensagens[i].titulo);
	    printf("   Mensagem: %s\n\n", mensagens[i].corpo);
    }
    if(i==0)
	printf("Nao ha mensagens!\n");
}

int apagarMensagem(Msg mensagens[], int n, int ind){
    if(ind>n || ind<0){
	printf("Esta mensagem nao existe!\n");
	return 0;
    }
    
    for(int i=ind-1; i<n-1; i++){
	    mensagens[i]=mensagens[i+1];	    
    }
    mensagens[n-1].remetente=-1;
    strcpy(mensagens[n-1].corpo," ");
    strcpy(mensagens[n-1].topico," ");
    strcpy(mensagens[n-1].titulo," ");
    mensagens[n-1].duracao=-1;
    return 1;
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
	printf("Nao ha topicos!\n");
    else{
	printf("Topicos: \n");
	for(int j=0; j<topicos; j++)
	    printf("-%s\n", aux[j].topico);
    }
}

void apagarTopicosSemMensagens(Msg mensagens[], int totalMensagens){
	for(int i=0; i<totalMensagens; i++){
		if(strcmp(mensagens[i].corpo," ")==0){
			for(int j=i; j<totalMensagens; j++){
				mensagens[j]=mensagens[j+1];
			}
			strcpy(mensagens[totalMensagens-1].corpo," ");
  		  	strcpy(mensagens[totalMensagens-1].topico," ");
    			strcpy(mensagens[totalMensagens-1].titulo," ");
    			mensagens[totalMensagens-1].duracao=-1;
    			mensagens[totalMensagens-1].remetente=-1;
		}	
	}
}

int main(int argc, char *argv[]){   
    char comando[60], *comandoAux[500], fifo_name[20];
    int num, fd_ser, fd_cli, res, adicionaNome=0;
    int FLAG_SHUTDOWN = 0, FLAG_FILTER=1;
    fd_set fontes;
    Msg msg, mensagens[nmaxmsg];
    struct timeval t;
    Login cli, clientes[maxusers];
    Server s;
    int numcli=0, r;

    int listaUsers[maxusers];
    for(int i = 0;i<maxusers;i++)
	listaUsers[i] = -1;
    iniciaMensagens(mensagens);
    s.nmensagens=0;

    if(access(FIFO_SERV, F_OK)==0) { //verificar se o sv esta aberto
    	fprintf(stderr, "[ERROR] Server ja existe.\n");
      	return EXIT_FAILURE;
    }
    if(mkfifo(FIFO_SERV, 0600) == -1){
	perror("\nmkfifo do FIFO do servidor deu erro");
	exit(EXIT_FAILURE);
    }
    fd_ser = open(FIFO_SERV, O_RDWR);

    if(getenv("MAXNOT") != NULL)
        nmaxnot = atoi(getenv("MAXNOT"));
    if(getenv("MAXMSG") != NULL)
        nmaxmsg = atoi(getenv("MAXMSG"));
	if(getenv("WORDSNOT") != NULL){
		strcpy(fileWN, getenv("WORDSNOT"));
	}
	if(getenv("MAXUSERS") != NULL){
		maxusers = atoi(getenv("MAXUSERS"));
	}
        

    settings();
    comandosmenu();
	
    do{
	fflush(stdout);
	printf("\nIntroduza um comando: ");
	fflush(stdout);

	FD_ZERO(&fontes);
        FD_SET(0, &fontes);
        FD_SET(fd_ser, &fontes);
	t.tv_sec=20;
	t.tv_usec=0;
        res = select(fd_ser + 1, &fontes, NULL, NULL, &t);

	
	if(res>0 && FD_ISSET(fd_ser, &fontes)) {		//FIFO
		r = read(fd_ser,&cli,sizeof(Login));
		if(cli.primeiro){
		      if(r == sizeof(Login) && cli.acesso == 1){
			sprintf(fifo_name, FIFO_CLI, cli.remetente);
			fd_cli = open(fifo_name, O_WRONLY |O_NONBLOCK);
			while(existecliente(clientes, s.ncliativos, cli.nome)){
				adicionaNome++;
				char adiciona[20];
				adiciona[0]=adicionaNome+'0';
				strcat(cli.nome, adiciona);
			}
                	if((s.ncliativos + 1 <= maxusers) && adicionacliente(clientes,&numcli,cli)==0){
	               	        s.ncliativos++;
                  	       	for(int i=0;i<maxusers;i++){
                     	               	if(listaUsers[i] == -1){
        	               			listaUsers[i] = cli.remetente;
        	       	      			break;
        	      	      		}
                   	        }
       	        	}
		        printf("\n%s iniciou sessao!\n",cli.nome);
			res = write(fd_cli,&cli,sizeof(Login));
			
			write(fd_cli,&s.nmensagens,sizeof(int));
			for(int i=0;i<s.nmensagens;i++)	//AQUI
	                  {
	                      res = write(fd_cli,&mensagens[i],sizeof(Msg));
	                  }

			close(fd_cli);
		      }
		}
		else if(r == sizeof(Login) && cli.acesso ==0){
		        for(int i=0;i<maxusers;i++)
            		    if(listaUsers[i] == cli.remetente){
			        listaUsers[i] = -1;
            		    	s.ncliativos--;
            	                eliminacliente(clientes,&numcli,cli.remetente);
            			printf("\n[Cliente %d a terminar]\n",cli.remetente);
            		}
        	}
		else{
			sprintf(fifo_name, FIFO_CLI, cli.remetente);
			fd_cli = open(fifo_name, O_WRONLY |O_NONBLOCK);
			read(fd_ser, &msg, sizeof(Msg));
			printf("\nInterrompido...\nRecebi '%s'\n\n", msg.corpo);

			if(s.nmensagens < nmaxmsg){
				//VERIFICA AS PALAVRAS MAS
				if(FLAG_FILTER==1)			
					chamaVerificador(msg.corpo);
	               	        s.nmensagens++;
				msg.resposta=s.nmensagens;
				adicionaMensagem(mensagens, s.nmensagens, msg);
       	        	}
			write(fd_cli, &msg, sizeof(Msg));
			close(fd_cli);
		}
    	}
    	else if(res>0 && FD_ISSET(0, &fontes)){		//TECLADO
		fgets(comando,60,stdin);
		comando[strlen(comando) - 1] = '\0';
		num=0;
		comandoAux[num]=strtok(comando," ");
		if(strcmp(comando,"mensagem")!=0){
			num++;
			while((comandoAux[num]=strtok(NULL," "))!=NULL)
				num++;
		}
		else{
			num++;
			while((comandoAux[num]=strtok(NULL,"\0"))!=NULL)
				num++;
		}
		
		if(strcmp(comando, "filter")==0 && comandoAux!=NULL){
			printf("Introduziu comando %s %s\n", comando, comandoAux[1]);

			if(strcmp(comandoAux[1],"on")==0)
				FLAG_FILTER=1;
			else if(strcmp(comandoAux[1],"off")==0)
				FLAG_FILTER=0;
		}
		else if(strcmp(comando,"users")==0 && comandoAux[1]==NULL){//LISTAR USERS
			printf("\nUtilizadores ligados:\n");
			for(int i=0; i<s.ncliativos; i++)
				printf("   %s\n", clientes[i].nome);
			
		}
		else if(strcmp(comando,"topics")==0 && comandoAux[1]==NULL){//LISTAR TOPICOS
			listaTopicos(mensagens, s.nmensagens);
		}
		else if(strcmp(comando,"msg")==0 && comandoAux[1]==NULL){//LISTAR MENSAGENS
			printf("Lista de mensagens:\n");			
			listaMensagens(mensagens, s.nmensagens);
		}
		else if(strcmp(comando,"topic")==0 && comandoAux[1]!=NULL){//LISTAR MENSAGENS DESTE TOPICO
			printf("Topico: %s\n", comandoAux[1]);
			mensagensTopico(mensagens, s.nmensagens, comandoAux[1]);
		}
		else if(strcmp(comando,"del")==0 && comandoAux!=NULL){//APAGAR MENSAGEM
			printf("Introduziu comando %s %s\n", comando, comandoAux[1]);
			int v=atoi(comandoAux[1]);
			if(apagarMensagem(mensagens, s.nmensagens, v))
				s.nmensagens--;
			
		}
		else if(strcmp(comando,"kick")==0 && comandoAux!=NULL){ //EXCLUIR USER
			for(int i=0; i<s.ncliativos; i++){
				if(strcmp(clientes[i].nome,comandoAux[1])==0){
					for(int i=0;i<s.ncliativos;i++)
            		    			if(listaUsers[i] == clientes[i].remetente){
			        			listaUsers[i] = -1;
            		    				s.ncliativos--;
            	                			eliminacliente(clientes,&numcli,clientes[i].remetente);
            						printf("\n[Cliente %d a terminar]\n",clientes[i].remetente);
							kill(clientes[i].remetente,SIGINT);
            					}
				}
			}
		}
		else if(strcmp(comando,"shutdown")==0 && comandoAux[1]==NULL){
			FLAG_SHUTDOWN = 1;
			printf("\n\n ===========Servidor vai desligar==========\n\n");
		}
		else if(strcmp(comando,"prune")==0 && comandoAux[1]==NULL){//APAGAR TOPICOS SEM MENSAGENS
			printf("\nApagando topicos sem mensagens!\n");
			apagarTopicosSemMensagens(mensagens, s.nmensagens);
			
			//AVISAR CLIENTES
		}
		else if(strcmp(comando,"help")==0 && comandoAux[1]==NULL)
			comandosmenu();
		else if(strcmp(comando,"mensagem")==0 && comandoAux!=NULL){//Enviar mensagem ao verificador
			printf("Mensagem: %s\n", comandoAux[1]);
			chamaVerificador(comandoAux[1]);
		}
		else{
			printf("\n[ERRO] Comando invalido!\n");
		}
	}
	else if(res==0){
		printf("TIMEOUT\n\n");
		FLAG_SHUTDOWN=1;
	}
    }while (FLAG_SHUTDOWN != 1);

    remove(FIFO_SERV);
    close(fd_ser);
    unlink(FIFO_SERV);

    for(int i=0;i<maxusers;i++){
	if(listaUsers[i] != -1){
	   kill(listaUsers[i],SIGINT);
	}
    }
    
    return EXIT_SUCCESS;
}
