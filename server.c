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
	int year;
} Movie;

Movie movies[10];
int movies_counter = 0;

void movie_to_file(Movie movie) {
    FILE *f;
    char file_name[15];

    sprintf(file_name, "%d", movie.id);
    strcat(file_name, ".txt");

    f = fopen(file_name, "w");
    if (f == NULL) {
        fprintf(stderr, "\nErro ao abrir arquivo\n");
        exit(1);
    }

    fprintf(f, "ID: %d\nTitulo: %s\nGenero: %s\nDirecao: %s\nAno: %d", movie.id, movie.title, movie.genre, movie.director, movie.year);

    fclose(f);
}

void cadastrar_filme(int conn) {
    char msg[80];

    if (movies_counter == 10) {
        // Lista de filmes atingiu seu máximo
        strcpy(msg, "Numero maximo de filmes atingido");
        write(conn, msg, sizeof(msg));
        return;
    }


    // Pede o titulo do filme
    strcpy(msg, "Digite o titulo do filme: ");
    write(conn, msg, sizeof(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    strcpy(movies[movies_counter].title, msg);

    memset(&msg, 0, sizeof(msg));

    // Pede o genero do filme
    strcpy(msg, "Digite o genero do filme: ");
    write(conn, msg, sizeof(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    strcpy(movies[movies_counter].genre, msg);

    memset(&msg, 0, sizeof(msg));

    // Pede o nome do diretor do filme
    strcpy(msg, "Digite o nome do diretor do filme: ");
    write(conn, msg, sizeof(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    strcpy(movies[movies_counter].director, msg);

    memset(&msg, 0, sizeof(msg));

    // Pede o ano do filme
    strcpy(msg, "Digite o ano do filme: ");
    write(conn, msg, sizeof(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    movies[movies_counter].year = atoi(msg);

    int id = rand();
    for (int i=0; i < movies_counter; i++) {
        while (movies[i].id == id) {
            id = rand();
        }
    }

    movies[movies_counter].id = id;

    movie_to_file(movies[movies_counter]);

    movies_counter++;

    memset(&msg, 0, sizeof(msg));
    strcpy(msg, "Filme cadastrado com sucesso! Deseja fazer outra operação? ");
    write(conn, msg, sizeof(msg));
}

void listar_titulos(int conn) {
    char msg[80];

    for (int i=0; i < movies_counter; i++) {
        // Modificar pra escrita de arquivo dps
        printf("Titulo: %s, ID: %d\n", movies[i].title, movies[i].id);
    }

    strcpy(msg, "Deseja fazer outra operação? ");
    write(conn, msg, sizeof(msg));
}

void listar_filmes(int conn) {
    char msg[80];

    for (int i=0; i < movies_counter; i++) {
        // Modificar pra escrita de arquivo dps
        printf("ID: %d, Titulo: %s, Ano: %d\n", movies[i].id, movies[i].title, movies[i].year);
        printf("Genero: %s, Diretor: %s\n", movies[i].genre, movies[i].director);
    }

    strcpy(msg, "Deseja fazer outra operação? ");
    write(conn, msg, sizeof(msg));
}

void remover_filme(int conn) {
    char msg[80];
    int flag;

    // Pede o ID do filme
    strcpy(msg, "Digite o ID do filme que deseja remover: ");
    write(conn, msg, sizeof(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));

    int id_to_rmv = atoi(msg);

    for (int i = movies_counter-1; i >= 0; i--) {
        if (movies[i].id == id_to_rmv) {
            flag = 1;
            if (i != movies_counter-1) {
                movies[i] = movies[i+1];
            } else {
                memset(&movies[i], 0, sizeof(Movie));
            }

            movies_counter--;
        }
    }

    // Deleta o arquivo do servidor
    char file_name[15];
    sprintf(file_name, "%d", id_to_rmv);
    strcat(file_name, ".txt");
    flag = remove(file_name);

    memset(&msg, 0, sizeof(msg));

    if (flag == 0) {
        strcpy(msg, "Filme removido. Deseja fazer outra operação? ");
    } else {
        strcpy(msg, "Erro ao remover filme: cheque novamente o ID. Deseja fazer outra operação? ");
    }

    write(conn, msg, sizeof(msg));
}

void ver_infos_filme(int conn) {
    char msg[80];
    int flag = 0;

    strcpy(msg, "Digite o ID do filme: ");
    write(conn, msg, sizeof(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));

    int id_to_see = atoi(msg);

    for (int i=0; i < movies_counter; i++) {
        if (movies[i].id == id_to_see) {
            flag = 1;
            printf("ID: %d, Titulo: %s, Ano: %d\n", movies[i].id, movies[i].title, movies[i].year);
            printf("Genero: %s, Diretor: %s\n", movies[i].genre, movies[i].director);
        }
    }

    memset(&msg, 0, sizeof(msg));

    if (flag != 1) {
        strcpy(msg, "ID inválido: filme não existe. Deseja fazer outra operação? ");
    } else {
        strcpy(msg, "Deseja fazer outra operação? ");
    }

    write(conn, msg, sizeof(msg));
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

        if (strcmp(buff,"cadastrar filme") == 0) {
			printf("\nCadastrando filme...");
            cadastrar_filme(conn);
            printf("\nCadastro finalizado\n");
		} else if (strcmp(buff,"adicionar genero") == 0) {
			printf("Adicionando genero");
		} else if (strcmp(buff,"listar titulos") == 0) {
			printf("\nListando titulos...\n");
            listar_titulos(conn);
		} else if (strcmp(buff,"listar info genero") == 0) {
			printf("Listando info genero");
		} else if (strcmp(buff,"listar filmes") == 0) {
			printf("\nListando filmes...\n");
            listar_filmes(conn);
		} else if (strcmp(buff,"ver infos filme") == 0) {
			printf("\nVendo infos do filme...\n");
            ver_infos_filme(conn);
		} else if (strcmp(buff,"remover filme") == 0) {
			printf("\nRemovendo filme...\n");
            remover_filme(conn);
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