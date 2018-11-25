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