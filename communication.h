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
#include <time.h>

#include "logging/log.h"
#include "constants.h"
#include "database.h"

#define BUFFER_SIZE 1024
#define MAX_CLIENT 20
#define NAME_LENGTH 50
#define PASSWORD_LENGTH 50
#define GAME_NAME_LENGTH 20

#define MAX_GAME_PLAYERS 4

/**
 * Client socket
 */

typedef struct clientInfo {
    int socket;
    int id;
    char name[NAME_LENGTH];

} ClientInfo;

typedef struct clientsSockets {
    ClientInfo client[MAX_CLIENT];
    int count;
    _Bool end;

    pthread_mutex_t lock;
} ClientsSockets;

ClientsSockets cSockets;
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
 * Game Servers
 */

typedef struct  gameServer{
    int adminId;
    int gameId;
    char name[GAME_NAME_LENGTH];
    int mapNumber;
    int maxPlayerCount;
    ClientInfo * clients[MAX_GAME_PLAYERS];
} GameServers;


GameServers gameServers[MAX_CLIENT];

/**
 *
 * @param port
 */

void initSocket(u_int16_t port);

void setSocketToFD();

void *accpetSocketThreadFun(void *arg);

void closeSocket();

void startCommunication();

void communication(enum communication_type commuType, ClientInfo *client);

void loginFromClient();

void createGameFromClient(ClientInfo *client);
int getFreeGameSlot();

#endif //BOMBERMAN_SERVER_COMMUNICATION_H
