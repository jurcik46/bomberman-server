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

PlayerSockets playerSockets;

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
//    if (listen(gameServerSocket, MAX_GAME_PLAYER) < 0) {
//        log_error("Game listen failed");
//        return false;
//    }
//    /**
//     * Init Select for File description changes
//     */
//    FD_ZERO(&socketDs);
//
//    /**
//     * Init new thread for accepting new clients connnections
//     */
//    cSockets.end = false;
//    cSockets.count = 0;
//
//    pthread_create(&acceptSocketThread, NULL, &accpetSocketThreadFun, &cSockets);
    return true;
}

void *initGame(void *arg) {
    GameServers game = *((GameServers *) arg);
    int count = 0;
    playerSockets.end = false;
    while (game.playerCount >= count) {
        if (socketReady()) {
            int pomType;
            sscanf(buffer, "%d", &pomType);

            log_debug("UDP: Hrac poslal data : %s", buffer);

            if (gameCommunication(pomType, buffer) == SUCCESS) {
                count++;
            }
        }
    }


    log_debug(" HRACI sa pripojili");
//    while (!end) {
//        if (socketReady()) {
//
//            if (gameCommunication(pomType, buffer) != NON) {
//                count++;
//            }
//        }
//
//    }
    pthread_exit(NULL);

}

enum gameEnum gameCommunication(enum gameEnum commTyp, char *data) {
    switch (commTyp) {
        case IN_GAME:
            log_debug("IN_GAME");
            if (saveInGame(data)) {
                return SUCCESS;
            } else {
                return NON;
            }
//            return loginToServer(data);
        case END:
            log_debug("END");
            playerSockets.end = true;
//            return createGameToServer(data);
        default:
            log_debug("DEFAULT");
            return NON;
    }
}


_Bool saveInGame(char *data) {
    int pomType, admin;

    sscanf(data, "%d %d %s %d",
           &pomType,
           &playerSockets.client[playerSockets.count].id,
           playerSockets.client[playerSockets.count].name,
           &admin);

    playerSockets.client[playerSockets.count].admin = (_Bool) admin;
    playerSockets.client[playerSockets.count].address = clientaddr;
//    clientaddr
}

//void *acceptGameSocketPLayer(void *arg) {
//
//    playerCount = *((int *) arg);
//    int addrlen = sizeof(address);
//    int count = 0;
//    while (count >= playerCount) {
//
//        int new_socket;
//        if ((new_socket = accept(gameServerSocket, (struct sockaddr *) &address,
//                                 (socklen_t *) &addrlen)) < 0) {
//            log_error("Accept Error");
//            continue;
//        }
//
//
//        for (int i = 0; i < playerCount; i++) {
//
//            if (playerSockets->client[i].socket == 0) {
//                playerSockets->client[i].socket = new_socket;
//                count++;
//                setSocketToFD();
//                log_info("New Socket Added Count %d Id socket %d ", count, new_socket);
//                recvfrom(playerSockets->client[i].socket, buffer, BUFFER_SIZE,
//                         0, (struct sockaddr *) &cliaddr, &len);
//                break;
//            }
//        }
//
//
//    };
//
//    log_info("Exit GAme accept socketThread");
//    pthread_exit(NULL);
//}




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