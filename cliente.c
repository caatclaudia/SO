#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include "header.h"

int main(int argc, char *argv[]){
	
	/*char fifo_name[20], str[20];
        int fd_ser, fd_cli;
	PEDIDO p;
	if(access(FIFO_SERV, F_OK)!=0) {
        	printf("[ERRO] Nao ha nenhum servidor!\n");
        	return EXIT_FAILURE;
	}
	sprintf(fifo_name, FIFO_CLI, getpid());
    	p.remetente = getpid();
	
    	mkfifo(fifo_name, 0600);
	fd_ser = open(FIFO_SERV, O_WRONLY); // escrita

	do{
		printf(">> ");
		scanf("%s", str);
		strcpy(p.palavra,str);

		write(fd_ser, &p, sizeof(PEDIDO));

		fd_cli = open(fifo_name, O_RDONLY);
		read(fd_cli, &p, sizeof(PEDIDO));
		close(fd_cli);
	}while(strcmp(p.palavra,"sair")!=0);

	close(fd_ser);
	unlink(fifo_name);*/

	return EXIT_SUCCESS;
}
