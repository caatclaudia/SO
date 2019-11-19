#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include "header.h"

int main(int argc, char *argv[]){
	
	char fifo_name[20];
        int fd_ser, fd_cli;
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

	do{
		printf(">> ");
		fgets(p.frase,MAXCHAR,stdin);
		write(fd_ser, &p, sizeof(PEDIDO));

		fd_cli = open(fifo_name, O_RDONLY);
		read(fd_cli, &p, sizeof(PEDIDO));
		close(fd_cli);
		fflush(stdout);
	}while(strcmp(p.frase, "sair")!=0);		//NAO ESTA A FUNCIONAR

	close(fd_ser);
	unlink(fifo_name);

	return EXIT_SUCCESS;
}
