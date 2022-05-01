#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define SERVER_PORT 8080
#define MAX_CONNS 5


typedef struct Movie {
	int id;
	char title[60];
	char genre[30];
	char director[30];
	unsigned int year;
} Movie;

Movie movies[10];
int movies_counter = 0;

void cadastrar_filme(int conn) {
    char msg[80];
    // memset(&buff, 0, sizeof(buff));

    if (movies_counter == 10) {
        // Lista de filmes atingiu seu máximo
        strcpy(msg, "Numero maximo de filmes atingido");
        write(conn, msg, sizeof(msg));
        return;
    }

    int id;
    char title[60];
    char genre[30];
    char director[30];
    char year[4];

    // Pede o titulo do filme
    strcpy(msg, "Digite o titulo do filme: ");
    write(conn, msg, sizeof(msg));
    read(conn, title, sizeof(title));

    memset(msg, 0, sizeof(msg));

    // Pede o genero do filme
    strcpy(msg, "Digite o genero do filme: ");
    write(conn, msg, sizeof(msg));
    read(conn, genre, sizeof(genre));

    memset(&msg, 0, sizeof(msg));

    // Pede o nome do diretor do filme
    strcpy(msg, "Digite o nome do diretor do filme: ");
    write(conn, msg, sizeof(msg));
    read(conn, director, sizeof(director));

    memset(&msg, 0, sizeof(msg));

    // Pede o ano do filme
    strcpy(msg, "Digite o ano do filme: ");
    write(conn, msg, sizeof(msg));
    read(conn, year, sizeof(year));

    id = rand();
    for (int i=0; i < movies_counter; i++) {
        while (movies[i].id == id) {
            id = rand();
        }
    }

    movies[movies_counter].id = id;
    strcpy(movies[movies_counter].title, title);
    strcpy(movies[movies_counter].genre, genre);
    strcpy(movies[movies_counter].director, director);
    movies[movies_counter].year = atoi(year);

    movies_counter++;

    memset(&msg, 0, sizeof(msg));
    strcpy(msg, "Filme cadastrado com sucesso! Deseja fazer outra operação? ");
    write(conn, msg, sizeof(msg));
}

void listar_titulos(int conn) {
    for (int i=0; i < movies_counter; i++) {
        // Modificar pra escrita de arquivo dps
        printf("Titulo: %s , ID: %d\n", movies[i].title, movies[i].id);
    }
}

void func(int conn) {
    char buff[80];
    int n;
    // infinite loop for chat
    for (;;) {
        memset(&buff, 0, sizeof(buff));
   
        // read the message from client and copy it in buffer
        read(conn, buff, sizeof(buff));

        // print buffer which contains the client contents
        printf("From client: %s", buff);

        if (strcmp(buff,"cadastrar filme\n") == 0) {
			printf("\nCadastrando filme...");
            cadastrar_filme(conn);
            printf("Cadastro finalizado\n");
		} else if (strcmp(buff,"adicionar genero\n") == 0) {
			printf("Adicionando genero");
		} else if (strcmp(buff,"listar titulos\n") == 0) {
			printf("\nListando titulos...\n");
            listar_titulos(conn);
		} else if (strcmp(buff,"listar info genero\n") == 0) {
			printf("Listando info genero");
		} else if (strcmp(buff,"listar filmes\n") == 0) {
			printf("Listando filmes");
		} else if (strcmp(buff,"ver infos filme\n") == 0) {
			printf("Vendo infos do filme");
		} else if (strcmp(buff,"remover filme\n") == 0) {
			printf("Removendo filme");
		}


    }
}

int main() {
    struct sockaddr_in s, client;
    int sockfd;
    pid_t childpid;

    // Socket creation
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error when creating socket");
        exit(1);
    }

    // Make sure the struct is empty
    memset(&s, 0, sizeof(s));

    // IP and Port assignments
    s.sin_family = AF_INET;
    s.sin_addr.s_addr = INADDR_ANY;
    s.sin_port = htons(SERVER_PORT);

    // Binding socket to given address
    if ((bind(sockfd, (struct sockaddr *)&s, sizeof(s))) != 0) {
        perror("Error when binding socket");
        exit(1);
    }

    // Server is ready to listen
    if ((listen(sockfd, MAX_CONNS)) != 0) {
        perror("Error when listening");
        exit(1);
    }

    for (;;) {
        int len = sizeof(client);
   
        // Accept the connection request from client
        int new_s = accept(sockfd, (struct sockaddr *)&client, &len);
        if (new_s < 0) {
            perror("Error when accepting a connection");
            exit(1);
        }

        if ((childpid = fork()) == 0) {
            // Child closes listening main socket
            close(sockfd);

            // Function for chatting between client and server
            func(new_s);

            // Parent closes connected socket
            close(new_s);
            exit(0);
        }

        close(new_s);
    }

    // Close the socket after client-server interaction
    close(sockfd);
}