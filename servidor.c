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
    printf("Ligar/desligar  filtragem de palvras proibidas (filter on / filter off)\n");
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
    printf("Numero maximo de mensagens a armazenar: %d\n", MAXMSG);
    printf("Valor de Timeout: %d\n", TIMEOUT);
    printf("Nome de ficheiro de palavras proibidas: %s\n",WORDSNOT);
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

int main(int argc, char *argv[]){   
    char comando[60], *comandoAux[500], fifo_name[20];
    int num, FLAG_SHUTDOWN = 0, fd_ser, fd_cli, res;
    /*fd_set fontes;
    PEDIDO p;

	if(access(FIFO_SERV, F_OK)==0) { //verificar se o sv esta aberto
       	fprintf(stderr, "[ERROR] Server ja existe.\n");
         	return EXIT_FAILURE;
	 }
	if(mkfifo(FIFO_SERV, 0600) == -1){
		perror("\nmkfifo do FIFO do servidor deu erro");
		exit(EXIT_FAILURE);
	}
	fd_ser = open(FIFO_SERV, O_RDWR);*/

    if(getenv("MAXNOT") != NULL)
        nmaxnot = atoi(getenv("MAXNOT"));
    if(getenv("MAXMSG") != NULL)
        nmaxmsg = atoi(getenv("MAXMSG"));

    settings();
    comandosmenu();
	
    do{
	/*FD_ZERO(&fontes);
        FD_SET(0, &fontes);
        FD_SET(fd_ser, &fontes);
        res = select(fd_ser + 1, &fontes, NULL, NULL, NULL);*/

	fflush(stdout);
	printf("\nIntroduza um comando: ");
	
	
	/*if(res>0 && FD_ISSET(fd_ser, &fontes)) {		//FIFO
		read(fd_ser, &p, sizeof(PEDIDO));
		printf("Recebi %s\n\n", p.palavra);

		sprintf(fifo_name, FIFO_CLI, p.remetente);
		fd_cli = open(fifo_name, O_WRONLY);
		write(fd_cli, &p, sizeof(PEDIDO));
		close(fd_cli);
    	}*/

    	//if(res>0 && FD_ISSET(0, &fontes)){		//TECLADO
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
			//diferenciar on e off
		}
		else if(strcmp(comando,"users")==0 && comandoAux[1]==NULL)
			printf("Introduziu comando %s\n", comando);
		else if(strcmp(comando,"topics")==0 && comandoAux[1]==NULL)
			printf("Introduziu comando %s\n", comando);
		else if(strcmp(comando,"msg")==0 && comandoAux[1]==NULL)
			printf("Introduziu comando %s\n", comando);
		else if(strcmp(comando,"topic")==0 && comandoAux[1]!=NULL){
			printf("Introduziu comando %s %s\n", comando, comandoAux[1]);
			//Topico em questao
		}
		else if(strcmp(comando,"del")==0 && comandoAux!=NULL){
			printf("Introduziu comando %s %s\n", comando, comandoAux[1]);
			//Mensagem em questao
		}
		else if(strcmp(comando,"kick")==0 && comandoAux!=NULL){
			printf("Introduziu comando %s %s\n", comando, comandoAux[1]);
			//User em questao
		}
		else if(strcmp(comando,"shutdown")==0 && comandoAux[1]==NULL){
			FLAG_SHUTDOWN = 1;
			printf("\n\n ===========Servidor vai desligar==========\n\n");
		}
		else if(strcmp(comando,"prune")==0 && comandoAux[1]==NULL)
			printf("Introduziu comando %s\n", comando);
		else if(strcmp(comando,"help")==0 && comandoAux[1]==NULL)
			comandosmenu();
		else if(strcmp(comando,"mensagem")==0 && comandoAux!=NULL){
			printf("Introduziu comando %s %s\n", comando, comandoAux[1]);
			//Enviar mensagem ao verificador
			chamaVerificador(comandoAux[1]);
		}
		else{
			printf("\n[ERRO] Comando invalido!\n");
		}
	//}
    }while (FLAG_SHUTDOWN != 1);

    /*remove(FIFO_SERV); //funciona!
    close(fd_ser);
    unlink(FIFO_SERV);*/
    
    return EXIT_SUCCESS;
}
