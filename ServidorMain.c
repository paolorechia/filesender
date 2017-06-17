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
#include <unistd.h>
/* Cabeçalhos para socket em Linux */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

/* Usuário linux padrão só pode abrir portas acima de 1024 */
#define PORTA 2048
#define BUFFER_SIZE 4096

/* Estrutura servidor local */
struct sockaddr_in local;
/* Estrutura cliente remoto */
struct sockaddr_in remoto;

int main(int argc, char **argv) {

	/* Descritor para comunicação socket */
	int socket_descritor;
	int client_descritor;
	int size;

    char buffer[BUFFER_SIZE];
    char * ACK = "ACKNOWLEDGE";
    size_t bytesReceived;
    // Arquivo 
    FILE * saida;
    long long fileSize;


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
	strcpy(buffer, "Bem Vindo! Mande-me: nome do arquivo e numero de blocos"); 

	/* Função de envio send - MAN SEND */

    char output_name[BUFFER_SIZE];
    int bytesRead = 0;
	if(send(client_descritor, buffer, strlen(buffer), 0)) {
		printf("Esperando nome do arquivo... ");
        if((size= recv(client_descritor, buffer, BUFFER_SIZE, 0)) > 0) {
            buffer[size]= '\0';
            sprintf(output_name, "copia_%s", buffer);
		    printf("%s\n", output_name);
        }
	    if(send(client_descritor, ACK, strlen(ACK), 0)){
        }
		printf("Esperando tamanho do arquivo cliente... ");
        if((size= recv(client_descritor, buffer, BUFFER_SIZE, 0)) > 0) {
            buffer[size]= '\0';
            fileSize=atoi(buffer);
            printf("%d\n", fileSize);
        }
	    if(send(client_descritor, ACK, strlen(ACK), 0)){
        }
        // Abre arquivo de saida
        saida = fopen(output_name, "wb");
		/* Função de recebimento recv - MAN RECV */
		while(bytesRead < fileSize) {
			//Limpando o Buffer antes de receber a mensagem
			memset(buffer, 0x0, BUFFER_SIZE);
			if((size= recv(client_descritor, buffer, BUFFER_SIZE, 0)) > 0) {
                fwrite(buffer, sizeof(char), size, saida);
                printf("Recebido: %d/%d bytes\n", bytesRead, fileSize);
                bytesRead += size;
            }
            else{
                printf("Error receiving!\n");
                exit(1);
            }
		}
        printf("Recebido: %d/%d bytes\n", bytesRead, fileSize);
        shutdown(socket_descritor, SHUT_RD);
//        if(send(client_descritor, ACK, strlen(ACK), 0)){
//        }
		close(client_descritor);
	}

	close(socket_descritor);
	printf("\nServiço de socket finalizado!\n");

	return EXIT_SUCCESS;
}
