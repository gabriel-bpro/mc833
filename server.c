#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define SERVER_PORT 8080
#define MAX_CONNS 5
#define MAX_LINE 256


void func(int conn) {
    char buff[80];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
   
        // read the message from client and copy it in buffer
        read(conn, buff, sizeof(buff));

        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');
   
        // and send that buffer to client
        write(conn, buff, sizeof(buff));
   
        // if msg contains "exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main() {
    struct sockaddr_in s, client;
    int sockfd;

    // Socket creation
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Erro - socket:");
        exit(1);
    }

    // Make sure the struct is empty
    memset(&s, 0, sizeof(s));

    // IP and Port assignments
    s.sin_family = AF_INET;
    s.sin_addr.s_addr = INADDR_ANY;
    s.sin_port = htons(SERVER_PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr *)&s, sizeof(s))) != 0) {
        perror("Erro - bind:");
        exit(1);
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, MAX_CONNS)) != 0) {
        perror("Erro - listen:");
        exit(1);
    }

    len = sizeof(client);
   
    // Accept the data packet from client and verification
    int new_s = accept(sockfd, (struct sockaddr *)&client, &len);
    if (new_s < 0) {
        perror("Erro - accept:");
        exit(1);
    }
   
    // Function for chatting between client and server
    func(new_s);
   
    // Close the socket after client-server interaction
    close(sockfd);

}