all: cliente gestor verificador clean

cliente:
	gcc -c cliente.c
	gcc header.h cliente.c -o cliente

	
gestor:
	gcc -c servidor.c
	gcc header.h servidor.c -o gestor
	

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