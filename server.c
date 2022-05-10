#include <stdio.h>
#include <dirent.h>
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
	char genre[60];
	char director[30];
	int year;
} Movie;


int existe_arq(int id) {
    FILE *f;
    char file_name[15];

    sprintf(file_name, "%d", id);
    strcat(file_name, ".txt");    

    if ((f = fopen(file_name, "r"))) {
        fclose(f);
        return 1;
    }

    return 0;
}

Movie file_to_movie(int id) {
    FILE *f;
    Movie movie;
    char file_name[15];
    char line[70];

    sprintf(file_name, "%d", id);
    strcat(file_name, ".txt");

    f = fopen(file_name, "r");

    // Primeira linha corresponde ao ID
    fgets(line, sizeof(line), f);
    char* token = strtok(line, ":");
    token = strtok(NULL, ":");
    movie.id = atoi(token);

    memset(&line, 0, sizeof(line));

    // Segunda linha corresponde ao titulo
    fgets(line, sizeof(line), f);
    token = strtok(line, ":");
    token = strtok(NULL, ":");
    token[strcspn(token,"\n")] = 0;
    strcpy(movie.title, token);

    memset(&line, 0, sizeof(line));

    // Terceira linha corresponde ao genero
    fgets(line, sizeof(line), f);
    token = strtok(line, ":");
    token = strtok(NULL, ":");
    token[strcspn(token,"\n")] = 0;
    strcpy(movie.genre, token);

    memset(&line, 0, sizeof(line));

    // Quarta linha corresponde a direcao
    fgets(line, sizeof(line), f);
    token = strtok(line, ":");
    token = strtok(NULL, ":");
    token[strcspn(token,"\n")] = 0;
    strcpy(movie.director, token);

    memset(&line, 0, sizeof(line));

    // Quinta linha corresponde ao ano
    fgets(line, sizeof(line), f);
    token = strtok(line, ":");
    token = strtok(NULL, ":");
    movie.year = atoi(token);

    fclose(f);

    return movie;
}

void movie_to_file(Movie movie) {
    FILE *f;
    char file_name[30];

    sprintf(file_name, "%d", movie.id);
    strcat(file_name, ".txt");

    f = fopen(file_name, "w");
    if (f == NULL) {
        fprintf(stderr, "\nErro ao abrir arquivo\n");
        exit(1);
    }

    fprintf(f, "ID:%d\nTitulo:%s\nGenero:%s\nDirecao:%s\nAno:%d", movie.id, movie.title, movie.genre, movie.director, movie.year);

    fclose(f);
}

void cadastrar_filme(int conn) {
    char msg[1024];
    Movie movie;


    // Pede o titulo do filme
    strcpy(msg, "Digite o titulo do filme: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    strcpy(movie.title, msg);

    memset(&msg, 0, sizeof(msg));

    // Pede o genero do filme
    strcpy(msg, "Digite o genero do filme: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    strcpy(movie.genre, msg);

    memset(&msg, 0, sizeof(msg));

    // Pede o nome do diretor do filme
    strcpy(msg, "Digite o nome do diretor do filme: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    strcpy(movie.director, msg);

    memset(&msg, 0, sizeof(msg));

    // Pede o ano do filme
    strcpy(msg, "Digite o ano do filme: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    movie.year = atoi(msg);

    // Atribui um ID unico para o filme
    int id = rand();
    while(existe_arq(id)) {
        id = rand();
    }
    movie.id = id;

    movie_to_file(movie);

    memset(&msg, 0, sizeof(msg));
    strcpy(msg, "Filme cadastrado com sucesso! Deseja fazer outra operação? ");
    write(conn, msg, strlen(msg));
}

void adicionar_genero(int conn) {
    Movie movie;
    char msg[1024];

    // Pede o ID do filme
    strcpy(msg, "Digite o ID do filme: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));

    int id = atoi(msg);

    memset(&msg, 0, sizeof(msg));

    // Pede o genero
    strcpy(msg, "Digite o genero que deseja adicionar: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));

    char genre[60];
    strcpy(genre, msg);

    memset(&msg, 0, sizeof(msg));

    if (!existe_arq(id)) {
        strcpy(msg, "ID inválido. Deseja fazer outra operação? ");
        write(conn, msg, strlen(msg));
    } else {
        movie = file_to_movie(id);
        strcat(movie.genre, ",");
        strcat(movie.genre, genre);
        movie_to_file(movie);

        strcpy(msg, "Deseja fazer outra operação? ");
        write(conn, msg, strlen(msg));
    }
}

void listar_titulos(int conn) {
    char msg[1024];
    memset(&msg, 0, sizeof(msg));

    char movie_info[1024];
    Movie movie;

    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".txt") != NULL) {
                char* token = strtok(dir->d_name, ".");
                int id = atoi(token);

                movie = file_to_movie(id);

                sprintf(movie_info, "ID:%d Titulo:%s\n", movie.id, movie.title);
                strcat(msg, movie_info);
                memset(&movie_info, 0, strlen(movie_info));
            }
        }
        strcat(msg, "Deseja fazer outra operação? ");
        write(conn, msg, strlen(msg));
        closedir(d);
    }
}

void listar_filmes(int conn) {
    char msg[1024];
    memset(&msg, 0, sizeof(msg));

    char movie_info[1024];
    Movie movie;

    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".txt") != NULL) {
                char* token = strtok(dir->d_name, ".");
                int id = atoi(token);

                movie = file_to_movie(id);

                sprintf(movie_info, "ID:%d Titulo:%s Genero:%s Direcao:%s Ano:%d\n", movie.id, movie.title, movie.genre, movie.director, movie.year);
                strcat(msg, movie_info);
                memset(&movie_info, 0, strlen(movie_info));
            }
        }
        strcat(msg, "Deseja fazer outra operação? ");
        write(conn, msg, strlen(msg));
        closedir(d);
    }
}

void listar_info_gen(int conn) {
    char msg[1024];
    char genre[60];

    // Pede o genero
    strcpy(msg, "Digite o genero: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));
    strcpy(genre, msg);

    memset(&msg, 0, sizeof(msg));

    char movie_info[1024];
    Movie movie;

    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".txt") != NULL) {
                char* token = strtok(dir->d_name, ".");
                int id = atoi(token);

                movie = file_to_movie(id);

                if (strstr(movie.genre, genre) != NULL) {
                    sprintf(movie_info, "ID:%d Titulo:%s Direcao:%s Ano:%d\n", movie.id, movie.title, movie.director, movie.year);
                    strcat(msg, movie_info);
                    memset(&movie_info, 0, strlen(movie_info));
                }
            }
        }
        strcat(msg, "Deseja fazer outra operação? ");
        write(conn, msg, strlen(msg));
        closedir(d);
    }
}

void remover_filme(int conn) {
    char msg[1024];
    int flag;

    // Pede o ID do filme
    strcpy(msg, "Digite o ID do filme que deseja remover: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));

    int id_to_rmv = atoi(msg);

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

    write(conn, msg, strlen(msg));
}

void ver_infos_filme(int conn) {
    char msg[1024];
    Movie movie;

    strcpy(msg, "Digite o ID do filme: ");
    write(conn, msg, strlen(msg));
    memset(&msg, 0, sizeof(msg));
    read(conn, msg, sizeof(msg));

    int id_to_see = atoi(msg);

    memset(&msg, 0, sizeof(msg));

    if (!existe_arq(id_to_see)) {
        strcpy(msg, "ID inválido. Deseja fazer outra operação? ");
        write(conn, msg, strlen(msg));
    } else {
        movie = file_to_movie(id_to_see);
        sprintf(msg, "ID:%d\nTitulo:%s\nGenero:%s\nDirecao:%s\nAno:%d\nDeseja fazer outra operação? ", movie.id, movie.title, movie.genre, movie.director, movie.year);
        write(conn, msg, strlen(msg));
    }
}

void func(int conn) {
    char buff[1024];
    int n;
    // infinite loop for chat
    for (;;) {
        memset(&buff, 0, sizeof(buff));
   
        read(conn, buff, sizeof(buff));

        printf("Msg do cliente: %s", buff);

        if (strcmp(buff,"cadastrar filme") == 0) {
			printf("\nCadastrando filme...");
            cadastrar_filme(conn);
            printf("\nCadastro finalizado\n");
		} else if (strcmp(buff,"adicionar genero") == 0) {
			printf("\nAdicionando genero...\n");
            adicionar_genero(conn);
            printf("Genero adicionado\n");
		} else if (strcmp(buff,"listar titulos") == 0) {
			printf("\nListando titulos...\n");
            listar_titulos(conn);
		} else if (strcmp(buff,"listar info genero") == 0) {
			printf("Listando info genero");
            listar_info_gen(conn);
		} else if (strcmp(buff,"listar filmes") == 0) {
			printf("\nListando filmes...\n");
            listar_filmes(conn);
		} else if (strcmp(buff,"ver infos filme") == 0) {
			printf("\nVendo infos do filme...\n");
            ver_infos_filme(conn);
		} else if (strcmp(buff,"remover filme") == 0) {
			printf("\nRemovendo filme...\n");
            remover_filme(conn);
            printf("Filme removido\n");
		} else if (strcmp(buff,"sair") == 0) {
            return;
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