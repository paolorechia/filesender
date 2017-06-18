------------------------------------------------------------
------------------------------------------------------------
Projeto Pratico - Redes de Computadores I
Prof. Michele Nogueira
Alunos: L Ma....
        Paolo Andreas Stall Rechia - GRR 20135196
------------------------------------------------------------
------------------------------------------------------------
O projeto inclui:

- README.txt
- ServidorMain.c
- ClienteMain.c
- makefile
------------------------------------------------------------
COMO USAR
------------------------------------------------------------
1. Para compilar os programas, basta usar:

$ make

E os binarios "servidor" e "cliente" serao compilados no diretorio corrente.
------------------------------------------------------------
2. Para executa-los:

Na maquina que sera o servidor, execute:

$ ./servidor

Na maquina cliente, execute:

$ ./cliente nome_arquivo endereco_ip_servidor

O arquivo será enviado e salvo no diretório corrente da máquina que está executando o binário 'servidor', com o mesmo nome, porém prefixado com 'copia_'.
------------------------------------------------------------
IMPORTANTE 
------------------------------------------------------------
O arquivo deve estar no mesmo diretório do binário 'cliente', porque o programa não trata caracteres especiais, como a '/'.
Além disso, é necessário evitar arquivos que contenham '@' ou '$', porque esses caracteres são usados como "padding" no cabeçalho nos programas. Seu uso leva a erros.
------------------------------------------------------------
------------------------------------------------------------
EXPLICAÇÂO SOBRE OS PROGRAMAS
------------------------------------------------------------
CLIENTE
O cliente é responsável por ler um arquivo de entrada, verificar seu tamanho e seu nome. Com isso, monta um header (cabeçalho) de tamanho pré-definido (200 bytes, 100 para o filename, 100 para o filesize) e que contém essas informaçõs sobre o arquivo.
Após, inicia uma conexão TCP com o servidor e começa a enviar dados. Primeiro manda o cabeçalho, em um buffer de 200 bytes, e, logo em seguida, o arquivo inteiro, usando buffers de 4096 bytes. Após terminar o envio, espera por um período, para que a fila de envio seja completamente esvaziada e encerra a conexão.
Algumas informações são jogadas para a saída padrão, como tamanho do arquivo em bytes, string representando o header montado e progresso do envio.
O progresso de envio é representado por vários caracteres '.', em que cada um representa aproximadamente 1%. 
O esgotamento da fila de envio também é representado por '.', porém aqui não há um valor pré-definido para cada caracter.
------------------------------------------------------------

SERVIDOR
Escuta até o cliente solicitar o início da conexão. Manda uma mensagem de boas-vindas e espera o header. Quando o header é inteiramente recebido, passa então a tratar os próximos bytes como parte do arquivo, que será montado na medida em que é recebido, também por meio de um buffer de 4096 bytes. Assim como no cliente, as informações relevantes são impressas na saída padrão. O progresso é marcado como acontece no cliente.
------------------------------------------------------------
------------------------------------------------------------
HISTÓRICO DE FRACASSOS

Por mera curiosidade, incluímos algumas tentativas fracassadas de programar o envio do arquivo:

1 - Primeiramente, tentou-se enviar todas as informações de uma vez só. No entanto, como o servidor não sabe o tamanho do arquivo, nem o tamanho do nome do arquivo, não há
como coletar essas informações dos bytes recebidos, dificultando a sincronização.
2 - Em segundo lugar, programou-se um protocolo de eco, em que para cada buffer enviado ao servidor, este será retornado ao cliente. Funciona em Loopback, porém apresenta problemas quando as taxas de envio e recebimento são assimétricas.
3 - Em terceiro, tentou-se usar mensagens de controle, como ACKNOLWEDGE. Essa solução funcionou, porém possivelmente por mero acaso. Como descobrimos eventualmente que o protocolo TCP não respeita limites de mensagens enviados com send e recebidas com recv, ficou a dúvida se o protocolo não iria fracassar em algum momento. Talvez a ideia funcionasse melhor com UDP.
4 - Por fim, a solução atual do cabeçalho, que leva em consideração o fato de que o TCP manda informações em streaming. Funcionou para diversos testes que fizemos, incluindo envio por WiFi de um arquivo de 851Mbytes.
