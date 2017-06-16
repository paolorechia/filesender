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
#include <string.h>
#include <unistd.h>
/* Cabeçalhos para socket em Linux */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTA 2048
#define BUFFER_SIZE 4096
#define IP_ADDR "127.0.0.1"
struct sockaddr_in remoto;

int main(int argc, char **argv) {

	/* Descritor para comunicação socket */
	int socket_descritor;

	int size;
	char buffer[BUFFER_SIZE];
    char * ACK = "ACKNOWLEDGE";
    FILE * entrada;
    long fileSize;
    size_t bytesRead;
    

    // Verifica que ha um argumento passado
    if (argc != 2){
        printf("Usage: %s filename\n", argv[0]);
        return -1;
    }

    // Le arquivo da linha de comando
    entrada = fopen(argv[1], "rb");
    // Obtem tamanho do arquivo
    fseek(entrada , 0 , SEEK_END);
    fileSize = ftell(entrada);
    rewind(entrada);

	printf("Cliente do serviço de Socket!\n");
	socket_descritor= socket(AF_INET, SOCK_STREAM, 0);
	if((socket_descritor == -1)) {

		perror("ERRO: Socket Descritor!\n");
		exit(EXIT_FAILURE);
	}

	/* Prenchendo a estrutura de dados  */
	remoto.sin_family= AF_INET;
	remoto.sin_port= htons(PORTA); //Converte portas para endereço de rede
	remoto.sin_addr.s_addr= inet_addr(IP_ADDR); //Endereço do Servidor
	memset(remoto.sin_zero, 0x0, 8);
	int size_remoto= sizeof(remoto);

	/* Conectando a conexão - MAN CONNECT*/
	connect(socket_descritor, (struct sockaddr*) &remoto, size_remoto);

    int i = 0;
    int test;
    int numBlocks = fileSize / BUFFER_SIZE;
    if (fileSize % BUFFER_SIZE != 0){
        numBlocks++;
    }
	/* Função de recebimento recv */
    if((size= recv(socket_descritor, buffer, 4096, 0)) > 0) {
        buffer[size]= '\0';
        printf("Mensagem do servidor: %s\n", buffer);
    }
    memset(buffer, 0x0, BUFFER_SIZE);
    sprintf(buffer, "%s", argv[1]);
    send(socket_descritor, buffer, strlen(buffer), 0);
    if((size= recv(socket_descritor, ACK, strlen(ACK), 0)) > 0){

    }

    printf("Separamos o arquivo em %d blocos...\n", numBlocks);
    memset(buffer, 0x0, BUFFER_SIZE);
    sprintf(buffer, "%d", numBlocks);
    send(socket_descritor, buffer, strlen(buffer), 0);
    if((size= recv(socket_descritor, ACK, strlen(ACK), 0)) > 0){

    }
	while(i < numBlocks) {
		memset(buffer, 0x0, BUFFER_SIZE);
        // Le BUFFER_SIZE bytes do arquivo de entrada
        bytesRead = fread(buffer,1,BUFFER_SIZE, entrada);
		// Envia buffer para o servidor
		test = send(socket_descritor, buffer, bytesRead, 0);
        printf("Bloco:%d - Enviados %d bytes\n", i, test);
        if (test < 0){
            printf("Failed to send:%d\n", test); }
        i++;
        if((size= recv(socket_descritor, ACK, strlen(ACK), 0)) > 0){
        }
    }


    fclose(entrada);
	close(socket_descritor);
	printf("\nCliente finalizado!\n");

	return EXIT_SUCCESS;
}
