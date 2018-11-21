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

#include "logging/log.h"

#define BUFFER_SIZE 1024
#define MAX_CLIENT 100

int server_fd, new_socket;
struct sockaddr_in address;
int opt;
int addrlen;
char buffer[BUFFER_SIZE];


void initSocket(u_int16_t port);

void closeSocket();


#endif //BOMBERMAN_SERVER_COMMUNICATION_H
