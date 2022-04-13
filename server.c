#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define SERVER_PORT 10
#define MAX_PENDING 5
#define MAX_LINE 256

int main() {
    struct sockaddr_in sin;
    int sockfd;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket:");
        exit(1);
    }

    memset(&sin, 0, sizeof(sin));       // make sure the struct is empty

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    

}

