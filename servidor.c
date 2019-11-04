#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/wait.h>
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
    printf("Desligra o gestor (shutdown)\n");
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

void chamaVerificador(){
	int pid=fork();
	if(pid==0)
		execl("verificador","verificador",WORDSNOT,NULL);
	//wait(&pid);
}

int main(int argc, char *argv[]){   
    char comando[60], *comandoAux[20];
    int num;

    if(getenv("MAXNOT") != NULL)
        nmaxnot = atoi(getenv("MAXNOT"));
    if(getenv("MAXMSG") != NULL)
        nmaxmsg = atoi(getenv("MAXMSG"));

    settings();
    comandosmenu();
    do{
	chamaVerificador();
	fflush(stdout);
        printf("\nIntroduza um comando: ");
	fgets(comando,60,stdin);
        comando[strlen(comando) - 1] = '\0';
	num=0;	
	comandoAux[num]=strtok(comando," ");
	num++;
	while((comandoAux[num]=strtok(NULL," "))!=NULL)
		num++;

	if(strcmp(comando, "filter")==0 && comandoAux[1]!=NULL){
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
	else if(strcmp(comando,"del")==0 && comandoAux[1]!=NULL){
		printf("Introduziu comando %s %s\n", comando, comandoAux[1]);
		//Mensagem em questao
	}
	else if(strcmp(comando,"kick")==0 && comandoAux[1]!=NULL){
		printf("Introduziu comando %%s %s\n", comando, comandoAux[1]);
		//User em questao
	}
	else if(strcmp(comando,"shutdown")==0 && comandoAux[1]==NULL)
		break;
	else if(strcmp(comando,"prune")==0 && comandoAux[1]==NULL)
		printf("Introduziu comando %s\n", comando);
	else if(strcmp(comando,"help")==0 && comandoAux[1]==NULL)
		comandosmenu();
	else
		printf("[ERRO]Comando invalido!\n");
	
    }while (strcmp(comando, "shutdown")!= 0);
    return EXIT_SUCCESS;
}
