#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define SERVERIP "10.0.0.2"
#define CLIENTIP "10.0.0.1"


#define BUFFER_SIZE 64

void communicate_with_server(int sockfd) {
    char buffer[BUFFER_SIZE];
    int n;

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        printf("Enter message (use 'write:<message>' to log or 'read' to get logs): ");
        n = 0;
        while ((buffer[n++] = getchar()) != '\n')
            ;
        buffer[n] = '\0';

        // Send message to server
        write(sockfd, buffer, strlen(buffer));

        // If message is "exit", terminate the connection
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Client exiting...\n");
            break;
        }

        // Read server response
        bzero(buffer, BUFFER_SIZE);
        read(sockfd, buffer, sizeof(buffer));
        printf("From Server: %s\n", buffer);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVERIP);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else {
        printf("connected to the server..\n");
    }

    // function for communication
    communicate_with_server(sockfd);

    // close the socket
    close(sockfd);
}
