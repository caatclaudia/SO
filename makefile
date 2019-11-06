all: cliente gestor verificador clean

cliente:
	gcc -c cliente.c
	gcc header.h cliente.c -o cliente

	
gestor:
	gcc -c servidor.c
	gcc header.h servidor.c -o servidor
	

verificador:
	gcc -c verificador.c
	gcc verificador.c -o verificador


clean:
	rm cliente
	rm servidor
	rm verificador
	rm cliente.o
	rm servidor.o
	rm verificador.o
