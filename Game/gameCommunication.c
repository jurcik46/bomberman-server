//
// Created by jurci on 9. 1. 2019.
//

#include "gameCommunication.h"

char buffer[BUFF_SIZE];

int gameServerSocket;
struct sockaddr_in servaddr, clientaddr;

fd_set socketDsGame;
int sdGame;
int activityGame;


_Bool initGameSocket(u_int16_t port) {
    int opt = 1;
    int addrlen = sizeof(servaddr);
    memset(buffer, '\0', sizeof buffer);
    bzero(&servaddr, sizeof(servaddr));
    if ((gameServerSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        log_error("Game socket failed");
        return false;
    }

    if (setsockopt(gameServerSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        log_error("Game setsockopt");
        exit(EXIT_FAILURE);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(gameServerSocket, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        log_error("Game bind failed");
        return false;
    }
    sdGame = gameServerSocket;
    return true;
}

void *initGame(void *arg) {
    GameServers game = *((GameServers *) arg);
    PlayerSockets playerSockets;

    playerSockets.count = 0;
    playerSockets.end = false;
    log_debug("UDP wating for clients in game SERVER");
    while (game.playerCount > playerSockets.count) {
        if (socketReady()) {
            int pomType;
            sscanf(buffer, "%d", &pomType);
            log_debug("UDP: Player count %d / %d Hrac poslal data : %s", playerSockets.count, game.playerCount, buffer);
            if (gameCommunication(pomType, buffer, &playerSockets) == SUCCESS) {

                playerSockets.count++;
                log_debug("UDP: Player joined to game count increment %d / %d  %s", playerSockets.count,
                          game.playerCount,
                          game.clients[playerSockets.count - 1]->name);

                sendto(gameServerSocket, buffer, BUFF_SIZE, 0, (struct sockaddr *) &clientaddr,
                       sizeof(clientaddr));
            }

        }
    }
    log_debug(" HRACI sa pripojili");

    while (!playerSockets.end) {
        if (socketReady()) {
            int pomType;
            sscanf(buffer, "%d", &pomType);
            log_debug("UDP: Player Hrac poslal data : %s", buffer);
//            if (pomType == END) {
//                gameCommunication(pomType, buffer);
//                log_debug("UDP:  Game END ");
//            }
//            int index = -1;
//            for (int i = 0; i < game.playerCount; ++i) {
//                if (playerSockets.client[i].id == 15) {
//                    sendto(gameServerSocket, buffer, BUFF_SIZE, 0,
//                           (struct sockaddr *) &playerSockets.client[i].address,
//                           sizeof(playerSockets.client[i].address));
//                }
//            }

//            memset(buffer, '\0', BUFF_SIZE);

//            log_debug("play count %d", playerSockets.count);
            for (int i = 0; i < game.playerCount; ++i) {
                if (clientaddr.sin_port != playerSockets.client[i].address.sin_port) {
                    if (sendto(gameServerSocket, buffer, BUFF_SIZE, 0,
                               (struct sockaddr *) &playerSockets.client[i].address,
                               sizeof(playerSockets.client[i].address)) ==
                        -1) {
                        log_debug("Error %s %d  %d  %s", strerror(errno), errno, playerSockets.client[i].id,
                                  playerSockets.client[i].name);
                    }
                    log_debug("UDP: SENDING to player ID %d name: %s  Buffer: %s",
                              playerSockets.client[i].id,
                              playerSockets.client[i].name, buffer);
                }
            }
            memset(buffer, '\0', BUFF_SIZE);
        }
    }

    pthread_exit(NULL);

}

enum gameEnum gameCommunication(enum gameEnum commTyp, char *data, PlayerSockets *pS) {
    switch (commTyp) {
        case IN_GAME:
            log_debug("IN_GAME");
            if (saveInGame(data, pS)) {
                return SUCCESS;
            } else {
                return NON;
            }
//            return loginToServer(data);
        case END:
            log_debug("END");
            pS->end = true;

//            for (int i = 0; i < playerSockets.count; ++i) {
//
//                sprintf(buffer, "%d", END);
//
//                if (clientaddr.sin_addr.s_addr != playerSockets.client[i].address.sin_addr.s_addr) {
//                    if (sendto(gameServerSocket, buffer, BUFF_SIZE, 0,
//                               (struct sockaddr *) &playerSockets.client[i].address,
//                               sizeof(playerSockets.client[i].address)) ==
//                        -1) {
//                        log_debug("Error %s %d ", strerror(errno), errno);
//                    }
//                }
//            }
            break;
        default:
            log_debug("DEFAULT");
            return NON;
    }
}


_Bool saveInGame(char *data, PlayerSockets *ps) {
    int pomType, admin;
//    struct sockaddr_in add;
//    char nm[NAME_LENGTH];
//    int id;

    for (int i = 0; i < ps->count + 1; ++i) {
        if (clientaddr.sin_addr.s_addr == ps->client[i].address.sin_addr.s_addr) {
            return false;
        }
    }
    sscanf(data, "%d %d %s %d",
           &pomType,
           &ps->client[ps->count].id,
           ps->client[ps->count].name,
           &admin);

    ps->client[ps->count].admin = (_Bool) admin;
    ps->client[ps->count].address = clientaddr;
    return true;
//    clientaddr
}

_Bool socketReady() {
    FD_ZERO(&socketDsGame);
    FD_SET(sdGame, &socketDsGame);
    struct timeval tv;
    tv.tv_usec = 1;

    activityGame = select(sdGame + 1, &socketDsGame, NULL, NULL, &tv);

    if ((activityGame < 0) && (errno != EINTR)) {
        log_error("Select Socket Activity error");
    }
    if (sdGame != 0 && FD_ISSET(sdGame, &socketDsGame)) {
        int len = sizeof(clientaddr);
        if (recvfrom(gameServerSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &clientaddr, &len) == 0) {

//            log_fatal("Server disconnected");
            //Close the socket and mark as 0 in list for reuse
//            close(sock.sock);
//            sock.sock = 0;
//            sleep(10);
//            exit(EXIT_FAILURE);
        } else {
//            log_debug("data %s", sock.buffer);
            return true;
        }
    }

    return false;
}

char *dataFromSocket() {
    return buffer;
}


int getUsedPort() {
    socklen_t len = sizeof(servaddr);
    if (getsockname(gameServerSocket, (struct sockaddr *) &servaddr, &len) != -1)
        return ntohs(servaddr.sin_port);
    return -1;
}


void setPlayerSockets(PlayerSockets *sockets) {
//    playerSockets = sockets;
}