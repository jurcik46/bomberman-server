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

                    log_debug("Sending to client id : %d , name %s , data : %s ",
                              sd, cSockets.client[i].name, buffer);
                    send(sd, buffer, BUFFER_SIZE, 0);
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
    }
    memset(buffer, '\0', sizeof buffer);

    sprintf(buffer, "%d %d", LOGIN, a);

}

void createGameFromClient(ClientInfo *client){

    memset(buffer, '\0', sizeof buffer);

    sprintf(buffer, "%d %d", CREATE_GAME, CREATED);

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