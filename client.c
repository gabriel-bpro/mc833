#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define SERVER_PORT 8080


void chat(int sockfd) {
	char buff[1024];
	int n;

	printf("O que deseja fazer: ");
	for (;;) {
        memset(&buff, 0, sizeof(buff));
		
		// Leitura do usuário ocorre até que pressione Enter
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		buff[strcspn(buff,"\n")] = 0;
		
		write(sockfd, buff, strlen(buff));

		// Ao digitar "sair", o cliente se desconecta, fechando seu socket
		if (strcmp(buff, "sair") == 0) {
			return;
		}

		memset(&buff, 0, sizeof(buff));

		read(sockfd, buff, sizeof(buff));
		// Após a leitura da mensagem enviada pelo servidor, temos a impressão do conteúdo no lado do cliente
		printf("%s", buff);
	}
}

int main() {
	int sockfd;
	struct sockaddr_in s;

	// Criação do socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Erro na criação do socket");
        exit(1);
	}
    
    // Esvazia o espaço de memória alocado para a estrutura s
    memset(&s, 0, sizeof(s));

	// Atribuição do IP e porta
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = inet_addr("127.0.0.1");
	s.sin_port = htons(SERVER_PORT);

	// Conexão entre cliente e servidor
	if (connect(sockfd, (struct sockaddr *)&s, sizeof(s)) != 0) {
		perror("Erro ao tentar conectar com o servidor");
		exit(1);
	}

	// Chat entre cliente e servidor
	chat(sockfd);

	// Fecha o socket após a interação com o servidor
	close(sockfd);
}
