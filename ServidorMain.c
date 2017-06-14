/*
 * Main.c
 *
 *  Created on: Apr 19, 2017
 *  Author: possati
 *
 *	As funções de sockets possuem manual no linux, ex man socket
 *
 *  SERVIDOR...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
/* Cabeçalhos para socket em Linux */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

/* Usuário linux padrão só pode abrir portas acima de 1024 */
#define PORTA 2048

/* Estrutura servidor local */
struct sockaddr_in local;
/* Estrutura cliente remoto */
struct sockaddr_in remoto;

int main(int argc, char **argv) {

	/* Descritor para comunicação socket */
	int socket_descritor;
	int client_descritor;

	int size;
	char buffer[4096];

	/* Socket sobre o TCP - MAN SOCKET */
	socket_descritor= socket(AF_INET, SOCK_STREAM, 0);

	if((socket_descritor == -1)) {

		perror("ERRO: Socket Descritor!\n");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Serviço de socket inicializado!\n");
	}

	/* Prenchendo a estrutura de dados do servidor */
	local.sin_family= AF_INET;
	local.sin_port= htons(PORTA); //Converte portas para endereço de rede

	memset(local.sin_zero, 0x0, 8);

	/* Função bind, utilizada somente no servidor do socket - MAN BIND */
	int bind_server= bind(socket_descritor, (struct sockaddr*) &local, sizeof(local));

	if(bind_server == -1) {

		perror("ERRO: Bind Servidor!\n");
		exit(EXIT_FAILURE);
	}

	/* Colocando a porta em escuta - Definido 1 conexão por vez - MAN LISTEN */
	listen(socket_descritor, 1);

	int size_remoto= sizeof(remoto);

	/* Recebendo conexão - MAN ACCEPT */
	client_descritor= accept(socket_descritor, (struct sockaddr*) &remoto, &size_remoto);

	if(client_descritor == -1) {

		perror("ERRO: Accept Servidor!\n");
		exit(EXIT_FAILURE);
	}

	/* Enviando mensagem ao cliente */
	strcpy(buffer, "Bem Vindo!");

	/* Função de envio send - MAN SEND */

	if(send(client_descritor, buffer, strlen(buffer), 0)) {

		printf("Esperando mensagem do cliente...\n");

		/* Função de recebimento recv - MAN RECV */
		while(1) {

			//Limpando o Buffer antes de receber a mensagem
			memset(buffer, 0x0, 4096);

			if((size= recv(client_descritor, buffer, 4096, 0)) > 0) {
				buffer[size]= '\0';
				printf("Mensagem do cliente: %s\n", buffer);

				/* Encerrar a conexão com cliente */
				close(client_descritor);
				break;
			}
		}
	}

	close(socket_descritor);
	printf("Serviço de socket finalizado!");

	return EXIT_SUCCESS;
}
