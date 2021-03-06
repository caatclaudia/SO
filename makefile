all: cliente gestor verificador

cliente:
	gcc -c cliente.c
	gcc header.h cliente.c -o cliente -lncurses

	
gestor:
	gcc -c servidor.c
	gcc header.h servidor.c -o gestor -lpthread
	

verificador:
	gcc -c verificador.c
	gcc verificador.c -o verificador


clean: cleang cleanc cleanv
	

cleang: 
	rm gestor
	rm servidor.o


cleanc:
	rm cliente
	rm cliente.o


cleanv:
	rm verificador
	rm verificador.o