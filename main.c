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

#define LOG_FILE_PATH "logs.log"
#define PORT 8080


void loggerInit(FILE *logFile) {

    if (logFile == NULL) {
        printf("Error! opening file");
        // Program exits if the file pointer returns NULL.
        exit(EXIT_FAILURE);
    }
    log_set_quiet(0);
    log_set_level(LOG_DEBUG);
    log_set_fp(logFile);
}

void closingApp(FILE *logFile) {
    closeSocket();
    fclose(logFile);
};
//
//void *SendFileToClient(int *arg) {
//    int connfd = (int) *arg;
//    write(connfd, fname, 256);
//
//    FILE *fp = fopen(fname, "rb");
//    if (fp == NULL) {
//        printf("File opern error");
//        return 1;
//    }
//
//    /* Read data from file and send it */
//    while (1) {
//        /* First read file in chunks of 256 bytes */
//        unsigned char buff[1024] = {0};
//        int nread = fread(buff, 1, 1024, fp);
//        //printf("Bytes read %d \n", nread);
//
//        /* If read was success, send data. */
//        if (nread > 0) {
//            //printf("Sending \n");
//            write(connfd, buff, nread);
//        }
//        if (nread < 1024) {
//            if (feof(fp)) {
//                printf("End of file\n");
//                printf("File transfer completed for id: %d\n", connfd);
//            }
//            if (ferror(fp))
//                printf("Error reading\n");
//            break;
//        }
//    }
//    printf("Closing Connection for id: %d\n", connfd);
//    close(connfd);
//    shutdown(connfd, SHUT_WR);
//    sleep(2);
//}


int main(int argc, char *argv[]) {

    FILE *logFile = fopen(LOG_FILE_PATH, "w+");
    loggerInit(logFile);
    initTables();

//    int someInt = 222;
//    int someInst = 333;
//    char str[50];
//    sprintf(str, "%d %d ", someInt, someInst);
//
//    log_debug("%s", str);
//    int a = atoi(str);
//    log_debug("%d", a);
    initSocket(PORT);
    startCommunication();


    closingApp(logFile);
    return 0;
}