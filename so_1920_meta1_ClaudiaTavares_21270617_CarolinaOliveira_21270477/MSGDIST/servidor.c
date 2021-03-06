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
    printf("Numero maximo de mensagens a armazenar: %d\n", nmaxmsg);
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
		close(0);
		dup(p[0]);
		close(p[0]);
		close(p[1]);
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

    if(getenv("MAXNOT") != NULL)
        nmaxnot = atoi(getenv("MAXNOT"));
    if(getenv("MAXMSG") != NULL)
        nmaxmsg = atoi(getenv("MAXMSG"));
	if(getenv("WORDSNOT") != NULL){
		strcpy(fileWN, getenv("WORDSNOT"));
	}
        

    settings();
    comandosmenu();
	
    do{
		fflush(stdout);
		printf("\nIntroduza um comando: ");
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
    }while (FLAG_SHUTDOWN != 1);
    return EXIT_SUCCESS;
}