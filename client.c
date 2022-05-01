#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define SERVER_PORT 8080


void func(int sockfd) {
	char buff[80];
	int n;

	printf("O que deseja fazer: ");
	for (;;) {
        memset(&buff, 0, sizeof(buff));
		
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		
		write(sockfd, buff, sizeof(buff));
		memset(&buff, 0, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("%s", buff);
	}
}

int main() {
	int sockfd, connfd;
	struct sockaddr_in s;

	// Socket creation
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Error when creating client socket");
        exit(1);
	}
    
    // Make sure the struct is empty
    memset(&s, 0, sizeof(s));

	// IP and Port assignments
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = inet_addr("127.0.0.1");
	s.sin_port = htons(SERVER_PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (struct sockaddr *)&s, sizeof(s)) != 0) {
		perror("Error when connecting with server");
		exit(1);
	}

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}
