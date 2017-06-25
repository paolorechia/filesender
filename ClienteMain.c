/*
 * Main.c
 *
 *  Created on: Apr 19, 2017
 *  Author: possati
 *
 *  CLIENTE...
 * ****************************** 
 *  Last modified by Paolo and L. Mateus on June 25, 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
// Cabecalhos depleteSendBuffer
#include <sys/ioctl.h>
#include <linux/sockios.h>
/* Cabeçalhos para socket em Linux */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTA 2048
#define BUFFER_SIZE 4096
#define FILENAME_MAXSIZE 200
#define FILESIZE_MAXSIZE 200
#define HEADER_SIZE FILENAME_MAXSIZE + FILESIZE_MAXSIZE
#define LOCALHOST "127.0.0.1"
#define LINGER 0
#define LINGER_TIME 120 

/* depleteSendBuffer 
  Funcao escrita por Bert Hubert, Netherlabs Computer Consulting BV - bert.hubert@netherlabs.nl
  Tem como objetivo esperar que todos os bytes da fila de saida sejam efetiva
  mente enviados.
  A chamada dessa funcao antes do close(socket) garante que o Sistema Operacional
  nao desaloca a fila de envio antes que todos os bytes sejam enviados. 
  Só funciona para Linux.
*/
void depleteSendBuffer(int fd) 
{
	int lastOutstanding=-1;
    printf("Esvaziando fila de envio");
    int i = 0;
	for(;;) {
		int outstanding;
		ioctl(fd, SIOCOUTQ, &outstanding);
		if(outstanding != lastOutstanding) 
		lastOutstanding = outstanding;
		if(!outstanding){
            printf(" terminado!\n");
			break;
        }
        i++;

        if ((i % 10 == 0)){
            printf(".");
            fflush(stdout);
        }
		usleep(1000);
	}
}

/* padString
 * Esta funcao recebe um buffer de tamanho FILENAME_MAXSIZE,
 * que contem uma string de tamanho n, descrevendo um filename,
 * ou um filesize. Encontra o final da string e preenche os 
 * espacos vazios do buffer com o caracter'@'.
 * Ao final, um '$' delimita seu fim (ao inves de um '\0').
 * Esses caracteres de padding sao usados no servidor para
 * a extracao das informacoes.
*/
void padString(char filename[FILENAME_MAXSIZE]){
    for (int i = strlen(filename); i < FILENAME_MAXSIZE; i++){
        filename[i]='@';
    }
    filename[FILENAME_MAXSIZE-1]='$';
}
/* buildHeader
 * Usando padString, constroi o header contendo o filename
 * e o fileSize, que serao enviados para o servidor.
*/
void buildHeader(char header[HEADER_SIZE],
                 char filename[FILENAME_MAXSIZE],
                 char fileSizeString[FILESIZE_MAXSIZE]){
    padString(filename);
    padString(fileSizeString);
    printf("%s\n", filename);
    printf("%s\n", fileSizeString);
    memset(header, 0x0, HEADER_SIZE);
    strcat(header, filename);
    strcat(header, fileSizeString);
    header[HEADER_SIZE]='\0';
}

struct sockaddr_in remoto;

int main(int argc, char **argv) {
    struct linger so_linger;

    so_linger.l_onoff=LINGER;
    so_linger.l_linger=LINGER_TIME;

	/* Descritor para comunicação socket */
	int socket_descritor;
    int set;

	int size;
	char buffer[BUFFER_SIZE];
    char filename[FILENAME_MAXSIZE];
    char fileSizeString[FILESIZE_MAXSIZE];
    char header[HEADER_SIZE];
    FILE * entrada;
    long long fileSize;


    // Verifica que ha um argumento passado
    if (argc < 3){
        printf("Usage: %s filename [ip address]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Le nome do arquivo da linha de comando
    sprintf(filename, "%s", argv[1]);
    // Le arquivo 
    entrada = fopen(filename, "rb");
    if (entrada == NULL){
        printf("Nao foi possivel ler o arquivo de entrada\n");
        exit(EXIT_FAILURE);
    }
    // E obtem seu tamanho 
    fseek(entrada , 0 , SEEK_END);
    fileSize = ftell(entrada);
    rewind(entrada);
    sprintf(fileSizeString, "%lli", fileSize);
    
    // Constroi header com o nome do arquivo e seu tamanho
    buildHeader(header, filename, fileSizeString);
    printf("Header: %s\n", header);
    printf("Filesize: %lli bytes...\n", fileSize);

    // Inicia servico Socket
	socket_descritor= socket(AF_INET, SOCK_STREAM, 0);
	if((socket_descritor == -1)) {

		perror("ERRO: Socket Descritor!\n");
		exit(EXIT_FAILURE);
	}
    /* Configura SO_LINGER Time
       SO_LINGER time determina quanto tempo o cliente espera
       para desalocar a fila de envio (na interface de rede).
    */
    set = setsockopt(
           socket_descritor,
           SOL_SOCKET,
           SO_LINGER,
           &so_linger,
           sizeof (so_linger)
        );
    if (set){
        printf("Nao foi possivel configurar o linger time\n");
        exit(EXIT_FAILURE);
    }

	/* Prenchendo a estrutura de dados  */
	remoto.sin_family= AF_INET;
	remoto.sin_port= htons(PORTA); //Converte portas para endereço de rede
    // Trata argumento 2 da CLI como endereco IP
    remoto.sin_addr.s_addr= inet_addr(argv[2]); //Endereço do Servidor
	memset(remoto.sin_zero, 0x0, 8);
	int size_remoto= sizeof(remoto);

	/* Conectando ao servidor */
	if (connect(socket_descritor, (struct sockaddr*) &remoto, size_remoto) != 0){
        printf("Nao foi possivel se conectar ao servidor: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

	/* Le fila de bytes recebidos*/
    if((size= recv(socket_descritor, buffer, 4096, 0)) > 0) {
        buffer[size]= '\0';
        printf("Servidor: %s", buffer);
    }
    else{
        printf("\nNao foi possivel receber mensagem do servidor\n");
    }

    /* Manda header para fila de envio */
    if ((send(socket_descritor, header, HEADER_SIZE, 0)) > 0){
        printf(" enviado!\n");
    }
    else{
        printf("\nNao foi possivel enviar o header.\n");
    }

    // bytesSent = bytes enviados
    long long sent = 0;
    long long bytesSent = 0;
    size_t bytesRead;
    int i = 1;
    int k;
    long long progressBar = fileSize/100;
    printf("Enviando");
	while(bytesSent < fileSize) {
        // Limpa buffer
		memset(buffer, 0x0, BUFFER_SIZE);
        // Le BUFFER_SIZE bytes do arquivo de entrada
        bytesRead = fread(buffer,1,BUFFER_SIZE, entrada);
		// Envia buffer para a fila de envio
		sent = send(socket_descritor, buffer, bytesRead, 0);
        if (sent < 0){
            printf("\nNao foi possivel enviar bytes para fila de envio.\n");
            exit(1);
        }
        else{
            bytesSent += sent;
            // Imprime barra de progresso
            if (bytesSent > progressBar * i){
                // Apaga linha
                printf("\r");
                // Reescreve
                printf("Enviando");
                k = 0;
                while ( k < i){
                    printf(".");
                    k++;
                }
                k = 0;
                while ( k < 100 - i){
                    printf(" ");
                    k++;
                }
                printf("%d%%", i);
                fflush(stdout);
                i++;
            }
//            printf("Enviados %lli/%lli bytes\n", bytesSent, fileSize);
        }
    }
    printf(" terminado!\n");
    printf("Enviados: %lli bytes\n", bytesSent);
    // Arquivo inteiro lido, desaloca memoria
    fclose(entrada);
    // Fecha conexao no sentido de escrita
    // Isso manda um pacote FIN para o servidor
    shutdown(socket_descritor, SHUT_WR);
    // Espera ateh que a fila de envio esteja vazia, ou seja,
    // ate que todos os bytes sejam enviados
    depleteSendBuffer(socket_descritor);
    // Loop infinito para esperar receber o FIN do servidor
    for (;;){
	size=read(socket_descritor, buffer, sizeof(buffer));
		if(size < 0) {
			perror("reading");
			exit(1);
		}
        // Quando size == 0, FIN foi recebido, tratar como EOF
		if(!size) {
			printf("EOF\n");
			break;
		}
    }
    // Desaloca socket
	close(socket_descritor);

	printf("\nCliente finalizado!\n");
	return EXIT_SUCCESS;
}
