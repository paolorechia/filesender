all:
	gcc ClienteMain.c -o cliente
	gcc ServidorMain.c -o servidor
servidor:
	gcc ServidorMain.c -o servidor
cliente:
	gcc ClienteMain.c -o cliente
clean:
	rm servidor cliente
