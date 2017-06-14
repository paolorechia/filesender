/*
 * Main.c
 *
 *  Created on: Apr 19, 2017
 *  Author: possati
 *
 *  CLIENTE...
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
/* Cabeçalhos para socket em Linux */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORTA 2048

struct sockaddr_in remoto;

int main(int argc, char **argv) {

	/* Descritor para comunicação socket */
	int socket_descritor;

	int size;
	char buffer[4096];

	printf("Cliente do serviço de Socket!\n");

	/* A maioria das funções do cliente é semelhante as funções do servidor */
	socket_descritor= socket(AF_INET, SOCK_STREAM, 0);

	if((socket_descritor == -1)) {

		perror("ERRO: Socket Descritor!\n");
		exit(EXIT_FAILURE);
	}

	/* Prenchendo a estrutura de dados  */
	remoto.sin_family= AF_INET;
	remoto.sin_port= htons(PORTA); //Converte portas para endereço de rede
	remoto.sin_addr.s_addr= inet_addr("127.0.0.1"); //Endereço do Servidor

	memset(remoto.sin_zero, 0x0, 8);

	int size_remoto= sizeof(remoto);

	/* Conectando a conexão - MAN CONNECT*/
	connect(socket_descritor, (struct sockaddr*) &remoto, size_remoto);

	/* Função de recebimento recv */
	while(1) {

		if((size= recv(socket_descritor, buffer, 4096, 0)) > 0) {
			buffer[size]= '\0';
			printf("Mensagem do servidor: %s\n", buffer);
		}

		memset(buffer, 0x0, 4096);
		/* Entrada do Teclado */
		fgets(buffer, 4096, stdin);

		/* Enviando os dados do teclado */
		send(socket_descritor, buffer, strlen(buffer), 0);

		break;
	}

	close(socket_descritor);
	printf("\nCliente finalizado!\n");

	return EXIT_SUCCESS;
}
