#include "communication.h"


void initSocket(u_int16_t port) {
    opt = 1;
    addrlen = sizeof(address);
    memset(buffer, '\0', sizeof buffer);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        log_error("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        log_error("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        log_error("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENT) < 0) {
        log_error("listen failed");
        exit(EXIT_FAILURE);
    }
    /**
     * Init Select for File description changes
     */
    FD_ZERO(&socketDs);
    max_sd = 0;

    /**
     * Init new thread for accepting new clients connnections
     */
    cSockets.end = false;
    cSockets.count = 0;
    pthread_mutex_init(&cSockets.lock, NULL);

    pthread_create(&acceptSocketThread, NULL, &accpetSocketThreadFun, &cSockets);

}

void startCommunication() {
    int a = 1;
    struct timeval tv;
    tv.tv_usec = 1;
    while (a) {
        FD_ZERO(&socketDs);

        setSocketToFD();
        if (cSockets.count == 0) {
            log_info("No clients connected waiting");
            sleep(5);
            continue;
        }

        activity = select(max_sd + 1, &socketDs, NULL, NULL, &tv);

        if ((activity < 0) && (errno != EINTR)) {
            log_error("Select Socket Activity error");
//            continue;
        }

        //else its some IO operation on some other socket
        for (int i = 0; i < MAX_CLIENT; i++) {
            sd = cSockets.client[i].socket;

            if (sd != 0 && FD_ISSET(sd, &socketDs)) {
                //Check if it was for closing , and also read the
                //incoming message
                if (recv(sd, buffer, BUFFER_SIZE, 0) == 0) {

                    //Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *) &address, \
                        (socklen_t *) &addrlen);
                    log_warn("Host disconnected , ip %s , port %d , socket %d , nick %s",
                             inet_ntoa(address.sin_addr), ntohs(address.sin_port),
                             sd, cSockets.client[i].name);

                    //Close the socket and mark as 0 in list for reuse
                    pthread_mutex_lock(&cSockets.lock);
                    close(sd);
                    cSockets.client[i].socket = 0;
//                    sd = 0 ;
                    memset(cSockets.client[i].name, '\0', sizeof cSockets.client[i].name);
                    cSockets.count--;
                    pthread_mutex_unlock(&cSockets.lock);

                } else {
                    log_debug("Client number %d , name %s , sent: %s",
                              sd, cSockets.client[i].name, buffer);

                    int pomType;
                    sscanf(buffer, "%d ", &pomType);

                    communication((enum communication_type) pomType,
                                  &cSockets.client[i]);

                    log_debug("Sending to client SOCKET_ID : %d , name %s , data : %s ",
                              sd, cSockets.client[i].name, buffer);
                    send(sd, buffer, BUFFER_SIZE, 0);
                    memset(buffer, '\0', sizeof buffer);
                }
            }
        }
    }
}

void communication(enum communication_type commuType, ClientInfo *client) {

    switch (commuType) {
        case LOGIN:
            log_debug("LOGIN");
            loginFromClient(client);
            break;
        case REGISTRATION:
            log_debug("REGISTRATION");

            break;
        case CREATE_GAME:
            log_debug("CREATE GAME");
            createGameFromClient(client);
            break;
        case FIND_SERVERS:
            log_debug("FIND SERVERS");
            findServersFromClient(client);
            break;
        case JOIN_LOBBY:
            log_debug("Player JOIN TO GAME");
            joinGameFromClient(client);
        default:
            log_debug("DEFAULT");
    }
}

void loginFromClient(ClientInfo *client) {
    int pomT, pomR;
    char nick[NAME_LENGTH];
    char password[PASSWORD_LENGTH];
    sscanf(buffer, "%d %d %s %s", &pomT, &pomR, nick, password);

    enum result_code a = login(nick, password);
    if (a == CREATED || a == OKEJ) {
        strcpy(client->name, nick);
        client->id = getPlayerId(nick);
    }
    memset(buffer, '\0', sizeof buffer);

    sprintf(buffer, "%d %d %d", LOGIN, a, client->id);
}

void createGameFromClient(ClientInfo *client) {
    srand(time(NULL));
    _Bool done;
    int r = 0;
    do {
        r = rand() % 90 + 1;
        done = true;

        for (int i = 0; i < MAX_CLIENT; ++i) {
            if (gameServers[i].gameId == r) {
                done = false;
            }
        }
    } while (!done);

    int gameSlot = getFreeGameSlot();

    if (gameSlot != -1) {

        gameServers[gameSlot].gameId = r;
        gameServers[gameSlot].clients[0] = client;
        gameServers[gameSlot].adminId = client->id;
        int pom;
        sscanf(buffer, "%d %d %s %d %d", &pom, &pom, gameServers[gameSlot].name,
               &gameServers[gameSlot].mapNumber, &gameServers[gameSlot].maxPlayerCount);
//        memset(buffer, '\0', sizeof buffer);
        sprintf(buffer, "%d %d %d %s %d %d", CREATE_GAME, CREATED, gameServers[gameSlot].gameId,
                gameServers[gameSlot].name,
                gameServers[gameSlot].maxPlayerCount, gameServers[gameSlot].mapNumber);
        gameServers[gameSlot].playerCount = 1;
    } else {
//        memset(buffer, '\0', sizeof buffer);
        sprintf(buffer, "%d %d", CREATE_GAME, SERVICE_UNAVAILABLE);
    }
//TODO DESTROY LOBBY AFTER USER LEAVE OR DISCONNECT
}

int getFreeGameSlot() {
    int k = -1;
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (gameServers[i].gameId == 0) {
            k = i;
            break;
        }
    }
    return k;
}

void findServersFromClient(ClientInfo *client) {
    int pomT, pomR, count;
    sscanf(buffer, "%d %d %d", &pomT, &pomR, &count);
    if (count > 19) {
        sprintf(buffer, "%d %d", FIND_SERVERS, DONE);
        return;
    }
    int pomC = 0;

    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (gameServers[i].gameId != 0) {
            if (pomC == count) {
                sprintf(buffer, "%d %d %d %s %d %d %d", FIND_SERVERS, ZERO,
                        gameServers[i].gameId,
                        gameServers[i].name,
                        gameServers[i].mapNumber,
                        gameServers[i].maxPlayerCount,
                        gameServers[i].adminId);
                return;
            }
            pomC++;
            log_debug("pomC  %d", pomC);
        }
    }
    sprintf(buffer, "%d %d", FIND_SERVERS, DONE);


}

void joinGameFromClient(ClientInfo *client) {
    int pomT, pomR, gameId;

    sscanf(buffer, "%d %d %d", &pomT, &pomR, &gameId);

    enum result_code result = OKEJ;
    char data[BUFFER_SIZE];
    memset(buffer, '\0', sizeof buffer);
    int gameIndex = existGame(gameId);
    if (gameIndex == -1) {
        result = NOT_FOUND;
    } else if (gameServers[gameIndex].playerCount == gameServers[gameIndex].maxPlayerCount) {
        result = SERVICE_UNAVAILABLE;
    }

    if (result == OKEJ) {
        sprintf(buffer, "%d %d %d %s", JOIN_LOBBY, CREATED, client->id, client->name);
        for (int i = 0; i < gameServers[gameIndex].playerCount; ++i) {
            log_debug("SEND INFO FOR PLAYERS IN LOBBY: ID: %d --- %s ", i, buffer);

            send(gameServers[gameIndex].clients[i]->socket, buffer, BUFFER_SIZE, 0);
        }

        gameServers[gameIndex].clients[gameServers[gameIndex].playerCount] = client;
        sprintf(data, "%d %s %d %d %d %d", gameServers[gameIndex].gameId,
                gameServers[gameIndex].name,
                gameServers[gameIndex].mapNumber,
                gameServers[gameIndex].playerCount,
                gameServers[gameIndex].maxPlayerCount,
                gameServers[gameIndex].adminId);
        gameServers[gameIndex].playerCount++;

    }
    sprintf(buffer, "%d %d %s", JOIN_LOBBY, result, data);

}

int existGame(int gameId) {
    int ret = -1;
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (gameServers[i].gameId == gameId) {
            ret = i;
            break;
        }
    }
    return ret;
}

void setSocketToFD() {
    //add child sockets to set
    for (int i = 0; i < MAX_CLIENT; i++) {
        //socket descriptor
        sd = cSockets.client[i].socket;

        //if valid socket descriptor then add to read list
        if (sd > 0)
            FD_SET(sd, &socketDs);

        //highest file descriptor number, need it for the select function
        if (sd > max_sd) {
            max_sd = sd;
            log_debug("Max_SD : %d", max_sd);
        }

    }
}

void *accpetSocketThreadFun(void *arg) {
    ClientsSockets *data = (ClientsSockets *) arg;

    while (!data->end) {

        if (data->count >= MAX_CLIENT) {
            continue;
        }
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                                 (socklen_t *) &addrlen)) < 0) {
            log_error("Accept Error");
            continue;
        }

        pthread_mutex_lock(&data->lock);

        for (int i = 0; i < MAX_CLIENT; i++) {

            if (data->client[i].socket == 0) {
                data->client[i].socket = new_socket;
                data->count++;
//                setSocketToFD();
                log_info("New Socket Added Count %d Id socket %d ", data->count, new_socket);
                break;
            }
        }
        pthread_mutex_unlock(&data->lock);

    };

    log_info("Exit socketThread");
    pthread_exit(NULL);
}


void closeSocket() {

    pthread_mutex_lock(&cSockets.lock);
    cSockets.end = true;

    for (int i = 0; i < MAX_CLIENT; i++) {
        close(cSockets.client[i].socket);
        cSockets.count--;
    }

    pthread_mutex_unlock(&cSockets.lock);

    close(server_fd);

    pthread_mutex_destroy(&cSockets.lock);

}