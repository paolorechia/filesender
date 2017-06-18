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
#define FILENAME_MAXSIZE 200
#define FILESIZE_MAXSIZE 200
#define HEADER_SIZE FILENAME_MAXSIZE + FILESIZE_MAXSIZE

/* Estrutura servidor local */
struct sockaddr_in local;
/* Estrutura cliente remoto */
struct sockaddr_in remoto;

int main(int argc, char **argv) {

	/* Descritor para comunicação socket */
	int socket_descritor;
	int client_descritor;
	int size;

    char header[HEADER_SIZE];
    char buffer[BUFFER_SIZE];
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
	strcpy(buffer, "Bem Vindo! Esperando o header...");

	/* Função de envio send - MAN SEND */

    char received_name[FILENAME_MAXSIZE];
    char output_name[FILENAME_MAXSIZE + 6];
    char fileSizeString[FILESIZE_MAXSIZE];
    char aux;
    int i = 0;
    int bytesRead = 0;
	memset(header, 0x0, HEADER_SIZE);
	if(send(client_descritor, buffer, strlen(buffer), 0)) {
		printf("Esperando header... ");
        while(size < HEADER_SIZE){
            if((size = recv(client_descritor, header, HEADER_SIZE, 0)) > 0) {
            size++;
            }
        }
        printf("%s\n", header);
        aux = header[0];
        while(aux != '@' && aux != '$'){
            received_name[i]=aux;
            i++;
            aux = header[i];
        }
        received_name[i]='\0';
        printf("filename: %s\n", received_name);
        sprintf(output_name, "copia_%s", received_name);
        printf("output name: %s\n", output_name);
        int j = 0;
        while (aux != '$'){
            i++;
            aux = header[i];
        }
        i++;
        aux = header[i];
        while (aux != '@' && aux != '\0'){
            fileSizeString[j]=aux;
            i++;
            j++;
            aux = header[i];
        }
        fileSizeString[j]='\0';
        printf("filesize: %s bytes\n", fileSizeString);
        fileSize = atoll(fileSizeString);
        
        printf("Iniciando recebimento...\n");
        // Abre arquivo de saida
        saida = fopen(output_name, "wb");
        // Recebe bytes enquanto nao atingir o tamanho do arquivo
        j = 1;
        int progressBar = fileSize/100;
		while(bytesRead < fileSize) {
			//Limpando o Buffer antes de receber a mensagem
			memset(buffer, 0x0, BUFFER_SIZE);
			if((size= recv(client_descritor, buffer, BUFFER_SIZE, 0)) > 0) {
                fwrite(buffer, sizeof(char), size, saida);
           //     printf("Recebido: %d/%d bytes\n", bytesRead, fileSize);
                bytesRead += size;
                if (bytesRead > progressBar * j){
                    printf(".");
                    fflush(stdout);
                    j++;
                }
            }
            else{
                printf("Error receiving!\n");
                exit(1);
            }
		}
        printf(" finished!\n");
        printf("Recebido: %d/%d bytes\n", bytesRead, fileSize);
        // Shutdown fecha a comunicacao para recebimento.
        // Isso manda um pacote FIN para o cliente, que sinaliza 
        // o termino da comunicacao.
        shutdown(socket_descritor, SHUT_RD);
        // Desaloca socket
		close(client_descritor);
	}
    // Desaloca socket
	close(socket_descritor);
	printf("\nServiço de socket finalizado!\n");

	return EXIT_SUCCESS;
}
