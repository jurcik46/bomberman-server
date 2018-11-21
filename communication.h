//
// Created by Jurco on 20. 11. 2018.
//

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

#include "logging/log.h"

#define BUFFER_SIZE 1024
#define MAX_CLIENT 101
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


void initSocket(u_int16_t port);

void *accpetSocketThreadFun(void *arg);

void closeSocket();


#endif //BOMBERMAN_SERVER_COMMUNICATION_H
