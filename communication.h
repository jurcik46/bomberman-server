#ifndef BOMBERMAN_SERVER_COMMUNICATION_H
#define BOMBERMAN_SERVER_COMMUNICATION_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>


#include "logging/log.h"
#include "constants.h"
#include "database.h"

#define BUFFER_SIZE 1024
#define MAX_CLIENT 10
/**
 * Client socket
 */
typedef struct clientsSockets {
    int socket[MAX_CLIENT];
    int count;
    _Bool end;

    pthread_mutex_t lock;
} ClientsSockets;

ClientsSockets cSocket;
pthread_t acceptSocketThread;

/**
 * Main Socket
 */
int server_fd;
struct sockaddr_in address;
int opt;
int addrlen;
char buffer[BUFFER_SIZE];

/**
 * Communication Select
 */
fd_set socketDs;
int max_sd;
int sd;
int activity;

/**
 *
 * @param port
 */

void initSocket(u_int16_t port);

void setSocketToFD();

void *accpetSocketThreadFun(void *arg);

void closeSocket();

void startCommunication();

void communication(enum communication_type commuType);

void loginFromClient();

#endif //BOMBERMAN_SERVER_COMMUNICATION_H
