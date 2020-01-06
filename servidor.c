#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<pthread.h>
#include"header.h"

#define THREADS nmaxmsg

Topic *topicos;
Msg *mensagens;
Login *clientes;
Server s;
int FLAG_MENSAGENSATUALIZA;
int FLAG_SHUTDOWN;

void comandosMenu() {
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
	return;
}

void settings() {
	printf("\n===========Configuracoes Iniciais======\n");
	printf("Numero de palavras proibidas: %d\n", nmaxnot);
	printf("Numero maximo de mensagens a armazenar: %d\n", nmaxmsg);
	printf("Numero maximo de utilizadores: %d\n", maxusers);
	printf("Valor de Timeout: %d\n", TIMEOUT);
	printf("Nome de ficheiro de palavras proibidas: %s\n", fileWN);
	printf("=========================================\n\n");
	return;
}

//PARA VERIFICADOR
int p[2], r[2];
int filho;

void iniciaVerificador() {
	int estado, num;

	pipe(p);
	pipe(r);
	filho = fork();
	if (filho == 0) {
		close(0);//FECHAR ACESSO AO TECLADO
		dup(p[0]);//DUPLICAR P[0] NA PRIMEIRA POSICAO DISPONIVEL
		close(p[0]);//FECHAR EXTREMIDADE DE LEITURA DO PIPE
		close(p[1]);//FECHAR EXTREMIDADE DE ESCRITA DO PIPE

		close(1);//FECHAR ACESSO AO MONITOR
		dup(r[1]);//DUPLICAR P[1] NA PRIMEIRA POSICAO DISPONIVEL
		close(r[0]);//FECHAR EXTREMIDADE DE LEITURA DO PIPE
		close(r[1]);//FECHAR EXTREMIDADE DE ESCRITA DO PIPE
		execl("verificador", "verificador", WORDSNOT, NULL);
	}
	close(p[0]);
	close(r[1]);
}

void terminaVerificador() {
	close(r[0]);
	kill(filho, SIGUSR2);
}

int chamaVerificador(char frase[]) {
	int bytes = 0, pal = 0;
	char resp[5];

	write(p[1], frase, strlen(frase));
	write(p[1], "\n", 1);
	write(p[1], "##MSGEND##", strlen("##MSGEND##"));
	write(p[1], "\n", 1);
	bytes = read(r[0], resp, strlen(resp));
	resp[bytes] = '\0';
	sscanf(resp, "%d", &pal);
	printf("\n[Frase com %d palavras proibidas!]\n", pal);

	return pal;
}

int adicionaCliente(int* n, Login c) {
	int i;
	for (i = 0; i < *n && strcmp(c.nome, clientes[i].nome) != 0; i++);
	if (i == *n) {
		clientes[*n] = c;
		(*n)++;
		return 0;
	}
	return 1;
}

int existeCliente(char nome[]) {
	int i;
	for (i = 0; i < s.ncliativos; i++) {
		if (strcmp(clientes[i].nome, nome) == 0)
			return 1;
	}
	return 0;
}

void eliminaCliente(int* n, int pid) {
	int i;
	for (i = 0; i < *n && clientes[i].remetente != pid; i++);
	if (i != *n) {
		clientes[i] = clientes[*n - 1];
		(*n)--;
	}
	return;
}

void iniciaMensagens() {
	for (int i = 0; i < nmaxmsg; i++) {
		mensagens[i].remetente = -1;
		strcpy(mensagens[i].corpo, " ");
		strcpy(mensagens[i].topico, " ");
		strcpy(mensagens[i].titulo, " ");
		mensagens[i].duracao = -1;
	}
	s.ntopicos = 0;
	s.nmensagens = 0;
	s.nmensagensreais = 0;
	for (int i = 0; i < 50; i++)
		strcpy(topicos[i].nome, " ");
	return;
}

void adicionaTopico(char topico[]) {
	for (int i = 0; i < s.ntopicos; i++)
		if (strcmp(topicos[i].nome, topico) == 0)
			return;
	s.ntopicos++;
	strcpy(topicos[s.ntopicos - 1].nome, topico);
	fprintf(stderr, "[Topico '%s' adicionado!]\n", topico);
	return;
}

void adicionaMensagem(Msg msg) {
	mensagens[s.nmensagensreais] = msg;
	adicionaTopico(msg.topico);
	s.nmensagensreais++;
	s.nmensagens++;
}

void mensagensTopico(char topico[]) {
	int EXISTE = 0;
	for (int i = 0; i < s.nmensagensreais; i++) {
		if (strcmp(mensagens[i].topico, topico) == 0) {
			printf("   Mensagem %d - Titulo: %s\n", mensagens[i].resposta, mensagens[i].titulo);
			printf("   Mensagem: %s\n\n", mensagens[i].corpo);
			EXISTE = 1;
		}
	}
	if (!EXISTE)
		printf("Nao ha mensagens deste topico!\n");
	return;
}

void listaMensagens() {
	int i;
	for (i = 0; i < s.nmensagensreais; i++) {
		printf("   Mensagem %d - Topico: %s\n", mensagens[i].resposta, mensagens[i].topico);
		printf("   Titulo: %s\n", mensagens[i].titulo);
		printf("   Mensagem: %s\n\n", mensagens[i].corpo);
	}
	if (i == 0)
		printf("Nao ha mensagens!\n");
	return;
}

int apagarMensagem(int ind) {
	if (ind > s.nmensagensreais || ind < 0) {
		printf("Esta mensagem nao existe!\n");
		return 0;
	}

	for (int i = ind - 1; i < s.nmensagensreais - 1; i++) {
		mensagens[i] = mensagens[i + 1];
	}
	mensagens[s.nmensagensreais - 1].remetente = -1;
	strcpy(mensagens[s.nmensagensreais - 1].corpo, " ");
	strcpy(mensagens[s.nmensagensreais - 1].topico, " ");
	strcpy(mensagens[s.nmensagensreais - 1].titulo, " ");
	mensagens[s.nmensagensreais - 1].duracao = -1;
	s.nmensagensreais--;
	return 1;
}

int eliminaMensagemTempo() {
	int apaga[s.nmensagensreais], num = 0, i;
	for (i = 0; i < s.nmensagensreais; i++)
		if (mensagens[i].duracao == 0) {
			fprintf(stderr, "\n[Mensagem %d ja nao esta disponivel!]", mensagens[i].resposta);
			apaga[num] = i;
			num++;
		}
	for (i = num - 1; i >= 0; i--)
		apagarMensagem(apaga[i]);
	if(num==0)
		return 0;
	return 1;
}

void listaTopicos() {
	if (s.ntopicos == 0) {
		printf("Nao ha topicos!\n");
		return;
	}
	printf("Topicos: \n");
	for (int i = 0; i < s.ntopicos; i++)
		printf("-%s\n", topicos[i].nome);
	return;
}

int apagarTopicosSemMensagens() {
	int EXISTE = 0;
	int apaga[s.ntopicos], apagaN = 0;
	for (int i = 0; i < s.ntopicos; i++) {
		apaga[i] = -1;
		EXISTE = 0;
		for (int j = 0; j < s.nmensagensreais && EXISTE == 0; j++) {
			if (strcmp(mensagens[j].topico, topicos[i].nome) == 0)
				EXISTE = 1;
		}
		if (!EXISTE) {
			apaga[apagaN] = i;
			apagaN++;
		}
	}
	for (int i = apagaN - 1; i >= 0; i--) {
		int ind = apaga[i];
		fprintf(stderr, "[Apagado topico '%s' !]\n", topicos[ind].nome);
		for (int j = ind; j < s.ntopicos - 1; j++)
			topicos[j] = topicos[j + 1];
		strcpy(topicos[s.ntopicos - 1].nome, " ");
		s.ntopicos--;
	}
	if (apagaN == 0)
		return 0;
	return 1;
}

void* func(void* dados) {

	do{
		for(int i=0; i<s.nmensagensreais; i++){
			mensagens[i].duracao--;
		}
		if(eliminaMensagemTempo())
			FLAG_MENSAGENSATUALIZA = 1;
			
		sleep(1);
	}while(!FLAG_SHUTDOWN);

	pthread_exit(NULL);
}

void mandaAtualizar(int cli[]) {
	for (int i = 0; i < maxusers; i++) {
		if (cli[i] != -1) {
			kill(cli[i], SIGUSR1);
			for(int i=0; i<s.nmensagensreais; i++)
				mensagens[i].tempoI=0;
		}
	}
	return;
}

int main(int argc, char* argv[]) {
	char comando[60], * comandoAux[500], fifo_name[20], fifo_name1[20];
	int num, fd_ser, fd_cli, res, adicionaNome = 0, fd_atu, n;
	int FLAG_FILTER = 1;
	fd_set fontes;
	Msg msg;
	struct timeval t;
	Login cli;
	int numcli = 0, r;
	FLAG_SHUTDOWN = 0;
	
	topicos= (Topic *) malloc( MAXTOPICOS * sizeof(Topic));
	mensagens=(Msg *) malloc(sizeof(Msg)*MAXMSG);
	clientes=(Login *) malloc(sizeof(Login)*MAXUSERS);

	int listaUsers[maxusers];
	for (int i = 0; i < maxusers; i++)
		listaUsers[i] = -1;

	iniciaMensagens(mensagens);

	if (access(FIFO_SERV, F_OK) == 0) {
		fprintf(stderr, "[ERROR] Server ja existe.\n");
		return EXIT_FAILURE;
	}
	if (mkfifo(FIFO_SERV, 0600) == -1) {
		fprintf(stderr, "\nmkfifo do FIFO do servidor deu erro");
		exit(EXIT_FAILURE);
	}
	fd_ser = open(FIFO_SERV, O_RDWR);
	mkfifo(FIFO_ATU, 0600);
	fd_atu = open(FIFO_ATU, O_RDWR);

	if (getenv("MAXNOT") != NULL)
		nmaxnot = atoi(getenv("MAXNOT"));
	if (getenv("MAXMSG") != NULL)
		nmaxmsg = atoi(getenv("MAXMSG"));
	if (getenv("WORDSNOT") != NULL) 
		strcpy(fileWN, getenv("WORDSNOT"));
	if (getenv("MAXUSERS") != NULL) 
		maxusers = atoi(getenv("MAXUSERS"));

	pthread_t* threads;
	threads = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(threads, NULL, (void*)func, NULL);
	int nThreads = 0;
	void* resultado;

	FLAG_MENSAGENSATUALIZA = 0;
	settings();
	comandosMenu();
	iniciaVerificador();

	do {
		fflush(stdout);
		printf("\nIntroduza um comando: ");
		fflush(stdout);

		FD_ZERO(&fontes);
		FD_SET(0, &fontes);
		FD_SET(fd_ser, &fontes);
		FD_SET(fd_atu, &fontes);
		t.tv_sec = 20;
		t.tv_usec = 0;
		res = select(fd_atu + 1, &fontes, NULL, NULL, &t);

		if (FLAG_MENSAGENSATUALIZA) {
			eliminaMensagemTempo(mensagens);
			FLAG_MENSAGENSATUALIZA = 0;
			mandaAtualizar(listaUsers);
		}
		else {


			if (res > 0 && FD_ISSET(fd_ser, &fontes)) {		//FIFO
				r = read(fd_ser, &cli, sizeof(Login));
				if (cli.primeiro) {
					if (r == sizeof(Login) && cli.acesso == 1) {
						sprintf(fifo_name, FIFO_CLI, cli.remetente);
						fd_cli = open(fifo_name, O_WRONLY | O_NONBLOCK);
						while (existeCliente(cli.nome)) {
							adicionaNome++;
							char adiciona[20];
							adiciona[0] = adicionaNome + '0';
							strcat(cli.nome, adiciona);
						}
						if ((s.ncliativos + 1 <= maxusers) && adicionaCliente(&numcli, cli) == 0) {
							s.ncliativos++;
							for (int i = 0; i < maxusers; i++) {
								if (listaUsers[i] == -1) {
									listaUsers[i] = cli.remetente;
									break;
								}
							}
						}
						fprintf(stderr, "\n[%s iniciou sessao!]\n", cli.nome);
						res = write(fd_cli, &cli, sizeof(Login));

						write(fd_cli, &s.nmensagensreais, sizeof(int));
						for (int i = 0; i < s.nmensagensreais; i++)
							res = write(fd_cli, &mensagens[i], sizeof(Msg));

						close(fd_cli);
					}
				}
				else if (r == sizeof(Login) && cli.acesso == 0) {
					for (int i = 0; i < maxusers; i++)
						if (listaUsers[i] == cli.remetente) {
							listaUsers[i] = -1;
							s.ncliativos--;
							eliminaCliente(&numcli, cli.remetente);
							fprintf(stderr, "\n[Cliente %d a terminar]\n", cli.remetente);
						}
				}
				else {
					sprintf(fifo_name, FIFO_CLI, cli.remetente);
					fd_cli = open(fifo_name, O_WRONLY | O_NONBLOCK);
					read(fd_ser, &msg, sizeof(Msg));
					fprintf(stderr, "\n[Interrompido...\nRecebi '%s']\n\n", msg.corpo);

					if (s.nmensagensreais < nmaxmsg) {
						//VERIFICA AS PALAVRAS MAS
						int pal = 0;
						if (FLAG_FILTER == 1)
							pal = chamaVerificador(msg.corpo);
						if (pal <= 2) {
							msg.resposta = s.nmensagens;
							adicionaMensagem(msg);
							mandaAtualizar(listaUsers);
						}
					}
					write(fd_cli, &msg, sizeof(Msg));
					close(fd_cli);
				}
			}
			if (res > 0 && FD_ISSET(fd_atu, &fontes)) {
				read(fd_atu, &cli, sizeof(Login));
				sprintf(fifo_name, FIFO_CLI, cli.remetente);
				fd_cli = open(fifo_name, O_WRONLY | O_NONBLOCK);
				write(fd_cli, &s.nmensagensreais, sizeof(int));
				for (int i = 0; i < s.nmensagensreais; i++)
					res = write(fd_cli, &mensagens[i], sizeof(Msg));
				write(fd_cli, &s.ntopicos, sizeof(int));
				for (int i = 0; i < s.ntopicos; i++)
					res = write(fd_cli, &topicos[i], sizeof(Topic));
				fprintf(stderr, "\n[Atualizacao feita no cliente %d]\n\n", cli.remetente);
				close(fd_cli);
			}
			else if (res > 0 && FD_ISSET(0, &fontes)) {		//TECLADO
				fgets(comando, 60, stdin);
				comando[strlen(comando) - 1] = '\0';
				num = 0;
				comandoAux[num] = strtok(comando, " ");
				if (strcmp(comando, "mensagem") != 0) {
					num++;
					while ((comandoAux[num] = strtok(NULL, " ")) != NULL)
						num++;
				}
				else {
					num++;
					while ((comandoAux[num] = strtok(NULL, "\0")) != NULL)
						num++;
				}

				if (strcmp(comando, "filter") == 0 && comandoAux != NULL) {
					printf("\n%s %s\n", comando, comandoAux[1]);

					if (strcmp(comandoAux[1], "on") == 0)
						FLAG_FILTER = 1;
					else if (strcmp(comandoAux[1], "off") == 0)
						FLAG_FILTER = 0;
				}
				else if (strcmp(comando, "users") == 0 && comandoAux[1] == NULL) {//LISTAR USERS
					if(s.ncliativos==0)	
						printf("\nNenhum utilizador ligado!\n");
					else{
						printf("\nUtilizadores ligados:\n");
						for (int i = 0; i < s.ncliativos; i++)
							printf("   %s\n", clientes[i].nome);
					}
				}
				else if (strcmp(comando, "topics") == 0 && comandoAux[1] == NULL) {//LISTAR TOPICOS
					listaTopicos(mensagens);
				}
				else if (strcmp(comando, "msg") == 0 && comandoAux[1] == NULL) {//LISTAR MENSAGENS
					if(s.nmensagensreais==0)	
						printf("\nNenhuma mensagem disponivel!\n");
					else{					
						printf("\nLista de mensagens:\n");
						listaMensagens(mensagens);
					}
				}
				else if (strcmp(comando, "topic") == 0 && comandoAux[1] != NULL) {//LISTAR MENSAGENS DESTE TOPICO
					printf("\nTopico: %s\n", comandoAux[1]);
					mensagensTopico(comandoAux[1]);
				}
				else if (strcmp(comando, "del") == 0 && comandoAux != NULL) {//APAGAR MENSAGEM
					printf("\nIntroduziu comando %s %s\n", comando, comandoAux[1]);
					int v = atoi(comandoAux[1]);
					for(int i=0; i<s.nmensagensreais; i++){
						if(v==mensagens[i].resposta){
							apagarMensagem(i);
							printf("\nMensagem apagada com sucesso!\n");
							mandaAtualizar(listaUsers);
						}
					}
				}
				else if (strcmp(comando, "kick") == 0 && comandoAux != NULL) { //EXCLUIR USER
					for (int i = 0; i < s.ncliativos; i++) {
						if (strcmp(clientes[i].nome, comandoAux[1]) == 0) {
							for (int i = 0; i < s.ncliativos; i++)
								if (listaUsers[i] == clientes[i].remetente) {
									listaUsers[i] = -1;
									s.ncliativos--;
									eliminaCliente(&numcli, clientes[i].remetente);
									fprintf(stderr, "\n[Cliente %d a terminar]\n", clientes[i].remetente);
									kill(clientes[i].remetente, SIGINT);
								}
						}
					}
				}
				else if (strcmp(comando, "shutdown") == 0 && comandoAux[1] == NULL) {
					FLAG_SHUTDOWN = 1;
					fprintf(stderr, "\n\n ===========Servidor vai desligar==========\n\n");
				}
				else if (strcmp(comando, "prune") == 0 && comandoAux[1] == NULL) {//APAGAR TOPICOS SEM MENSAGENS
					printf("\nApagando topicos sem mensagens!\n");
					if (apagarTopicosSemMensagens())
						mandaAtualizar(listaUsers);
				}
				else if (strcmp(comando, "help") == 0 && comandoAux[1] == NULL)
					comandosMenu();
				else if (strcmp(comando, "mensagem") == 0 && comandoAux != NULL) {//Enviar mensagem ao verificador
					printf("\nMensagem: %s\n", comandoAux[1]);
					chamaVerificador(comandoAux[1]);	//NAO ESTA A DAR CERTO
				}
				else if(strcmp(comando, " ")==0)
					printf("\n[ERRO] Comando invalido!\n");
			}
			else if (res == 0) {
				fprintf(stderr, "TIMEOUT\n\n");
				FLAG_SHUTDOWN = 1;
			}
		}
	} while (FLAG_SHUTDOWN != 1);

	free(threads);

	terminaVerificador();
	
	for (int i = 0; i < maxusers; i++) {
		if (listaUsers[i] != -1) {
			kill(listaUsers[i], SIGINT);
		}
	}

	remove(FIFO_SERV);
	close(fd_ser);
	unlink(FIFO_SERV);
	remove(FIFO_ATU);
	close(fd_atu);
	unlink(FIFO_ATU);
	
	return EXIT_SUCCESS;
}
