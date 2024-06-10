#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define SERVERIP "10.0.0.2"
#define CLIENTIP "10.0.0.1"

#define BUFFER_SIZE 64
#define MAX_LOGS 8
#define LOG_FILE "log.txt"

// Function designed for logging between client and server.
void handle_client(int connfd) 
{ 
    char buffer[BUFFER_SIZE];
    char logs[MAX_LOGS][BUFFER_SIZE];
    char mcopy[MAX_LOGS*BUFFER_SIZE];
    int log_count = 0;
    int log_index = 0;
     int logs_to_send = 0;
     int index =0 ;
     int start_index;

    FILE *log_file;

    // Open log file for appending
    log_file = fopen(LOG_FILE, "w");
    if (log_file == NULL) {
        perror("Failed to open log file");
        close(connfd);
        return;
    }

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        memset(mcopy, 0, MAX_LOGS*BUFFER_SIZE);


        // read the message from client and copy it in buffer 
        int read_bytes = read(connfd, buffer, sizeof(buffer) - 1);
        if (read_bytes == -1) {
            perror("Read failed");
            break;
        }

        buffer[read_bytes] = '\0';
        printf("Received: %s\n", buffer);

        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Server exiting...\n");
            break;
        }

        if (strncmp(buffer, "write:", 6) == 0) {
            // Log the message
            strncpy(logs[log_index], buffer + 6, BUFFER_SIZE - 6);
            fprintf(log_file, "%s\n", logs[log_index]);
            fflush(log_file);
            log_index = (log_index + 1) % MAX_LOGS;
            log_count++;
            write(connfd, "Message received and logged\n", 29);
        } else if (strncmp(buffer, "read:", 5) == 0) {
          int n = atoi(buffer + 5);
            if(n>log_count){
            n = log_count;
            }
            if (log_count > MAX_LOGS) {
                   logs_to_send = MAX_LOGS;
                } else {
                     logs_to_send = log_count;
                        }  
            index = 0 ; 
            start_index = (log_index - n + MAX_LOGS) % MAX_LOGS;          

            for (int i = 0; i < n ; i++) {
                        index = (start_index + i) % MAX_LOGS;
                        strcat(mcopy, logs[index]);
                        strcat(mcopy, "\n");
             //   write(connfd, logs[i], strlen(logs[i]));
            }
            strcat(mcopy, "\0");
            write(connfd, mcopy, strlen(mcopy));
            memset(mcopy, 0, MAX_LOGS*BUFFER_SIZE);

        }else {
            write(connfd, "write only 'write:' 'read:' 'exit'\n", 36);
        }
    }

    fclose(log_file);
    close(connfd);
}

// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 

    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 

    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(SERVERIP); 
    servaddr.sin_port = htons(PORT); 

    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 

    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 

    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server accept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server accept the client...\n"); 

    // Function for logging between client and server 
    handle_client(connfd); 

    // After logging close the socket 
    close(sockfd); 
}