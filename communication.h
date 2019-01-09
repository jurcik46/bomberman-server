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
#include <dirent.h>

#include "logging/log.h"
#include "constants.h"
#include "database.h"

#define MAPY "../Mapy/"
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
    _Bool admin;
    _Bool inLobby;
} ClientInfo;

typedef struct clientsSockets {
    ClientInfo client[MAX_CLIENT];
    int count;
    _Bool end;

    pthread_mutex_t lock;
} ClientsSockets;


/**
 * Game Servers
 */

typedef struct gameServer {
    int adminId;
    int gameId;
    char name[GAME_NAME_LENGTH];
    int mapNumber;
    int maxPlayerCount;
    int playerCount;
    ClientInfo *clients[MAX_GAME_PLAYERS];
} GameServers;


/**
 *
 * @param port
 */

void initSocket(u_int16_t port);

void setSocketToFD();

static _Bool isLogged(int id);

void *accpetSocketThreadFun(void *arg);

void closeSocket();

void startCommunication();

_Bool communication(enum communication_type commuType, ClientInfo *client);

void loginFromClient(ClientInfo *client);

void createGameFromClient(ClientInfo *client);

static _Bool mapaExist(int mapNumber);

int getFreeGameSlot();

void findServersFromClient(ClientInfo *client);

void getPlayerInLobby();

void joinGameFromClient(ClientInfo *client);


void leaveLobbyFromClient(ClientInfo *client);

void leaveLobbyDisconnect(ClientInfo *client);

static int existGame(int gameId);

void sendMapToClient(ClientInfo *clinet);

static void playerLeft(int gameIndex, int playerId);

#endif //BOMBERMAN_SERVER_COMMUNICATION_H
