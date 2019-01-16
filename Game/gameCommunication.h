
#ifndef BOMBERMAN_SERVER_GAMECOMMUNICATION_H
#define BOMBERMAN_SERVER_GAMECOMMUNICATION_H

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>

#include "../logging/log.h"
#include "../communication.h"
#include "../Game/gameConstants.h"

#define BUFF_SIZE 200
#define MAX_GAME_PLAYER 4
#define NAME_LENGTH 50


typedef struct playerInfo {
    struct sockaddr_in address;
    int id;
    char name[NAME_LENGTH];
    _Bool admin;
} PlayerInfo;

typedef struct playerSockets {
    PlayerInfo client[MAX_GAME_PLAYER];
    int count;
    _Bool end;
} PlayerSockets;


_Bool initGameSocket(u_int16_t port);

void *initGame(void *arg);

void *acceptGameSocketPLayer(void *arg);

_Bool socketReady();
_Bool saveInGame(char *data, struct playerSockets * ps);


enum gameEnum gameCommunication(enum gameEnum commTyp, char *data, PlayerSockets * pS);

char *dataFromSocket();


#endif //BOMBERMAN_SERVER_GAMECOMMUNICATION_H
