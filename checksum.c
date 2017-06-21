#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int main(int argc, char * argv[]){
    FILE * entrada;
    FILE * saida;
    long fileSize;
    char buffer[BUFFER_SIZE];
    char output_name[100];
    size_t bytesRead;
    char checksum = 0;

    if (argc != 2){
        printf("Usage: %s filename\n", argv[0]);
        return -1;
    }

    entrada = fopen(argv[1], "rb");
    // obtain file size:
    fseek(entrada , 0 , SEEK_END);
    fileSize = ftell(entrada);
    rewind(entrada);
    
    sprintf(output_name, "copy_%s", argv[1]);
    while((bytesRead = fread (buffer,1,BUFFER_SIZE, entrada)) == BUFFER_SIZE){
        for (int i = 0; i < BUFFER_SIZE; i++){
            checksum += ~buffer[i];
        }
    }
    printf("%d\n", (unsigned int)checksum);

    fclose(entrada);
    return 0;
}
