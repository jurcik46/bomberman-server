#include <stdio.h>
#include <sqlite3.h>

#include "database.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "logging/log.h"
#include "communication.h"

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {

    log_set_quiet(0);
    log_set_level(LOG_DEBUG);
    FILE *logFile = fopen("logs.log", "w+");
    log_set_fp(logFile);
//    initTables();
//    registration("aaaa", "bbb");
//    int a = login("aaaa", "bbb");
//    log_debug("%d", a);
    if (logFile == NULL) {
        printf("Error! opening file");
        // Program exits if the file pointer returns NULL.
        return 1;
    }

    initSocket(8080);
    char *hello = "Hello from server";
//    read(new_socket, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);
//    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

//    log_trace(const char *fmt, ...);
//    log_debug(const char *fmt, ...);
//    log_info(const char *fmt, ...);
//    log_warn(const char *fmt, ...);
//    log_error(const char *fmt, ...);
//    log_fatal(const char *fmt, ...);

//    int sockfd, newsockfd, portno;
//    socklen_t clilen;
//    char buffer[256];
//    struct sockaddr_in serv_addr, cli_addr;
//    int n;
////    if (argc < 2) {
////        fprintf(stderr,"ERROR, no port provided\n");
////        exit(1);
////    }
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (sockfd < 0)
//        error("ERROR opening socket");
//    bzero((char *) &serv_addr, sizeof(serv_addr));
//    portno = atoi("2666");
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = INADDR_ANY;
//    serv_addr.sin_port = htons(portno);
//    if (bind(sockfd, (struct sockaddr *) &serv_addr,
//             sizeof(serv_addr)) < 0)
//        error("ERROR on binding");
//    listen(sockfd, 5);
//    clilen = sizeof(cli_addr);
//    newsockfd = accept(sockfd,
//                       (struct sockaddr *) &cli_addr,
//                       &clilen);
//    if (newsockfd < 0)
//        error("ERROR on accept");
//    bzero(buffer, 256);
//    n = read(newsockfd, buffer, 255);
//    if (n < 0) error("ERROR reading from socket");
//    printf("Here is the message: %s\n", buffer);
//    n = write(newsockfd, "I got your message", 18);
//    if (n < 0) error("ERROR writing to socket");
//    close(newsockfd);
//    close(sockfd);
    closeSocket();
    fclose(logFile);
    return 0;
}