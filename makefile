all:
	gcc ClienteMain.c -o cliente
	gcc ServidorMain.c -o servidor
servidor:
	gcc ServidorMain.c -o servidor -Wall
cliente:
	gcc ClienteMain.c -o cliente -Wall
clean:
	rm servidor cliente
