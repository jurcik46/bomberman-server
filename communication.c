#include "communication.h"


void initSocket(u_int16_t port) {
    opt = 1;
    addrlen = sizeof(address);

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
    cSocket.end = false;
    cSocket.count = 0;
    pthread_mutex_init(&cSocket.lock, NULL);

    pthread_create(&acceptSocketThread, NULL, &accpetSocketThreadFun, &cSocket);

}

void startCommunication() {
    int a = 1;
    while (a) {
        FD_ZERO(&socketDs);

        setSocketToFD();
        if (cSocket.count == 0) {
            sleep(5);
            continue;
        }

        activity = select(max_sd + 1, &socketDs, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            log_error("Select Socket Activity error");
            continue;
        }

        //else its some IO operation on some other socket
        for (int i = 0; i < MAX_CLIENT; i++) {
            sd = cSocket.socket[i];

            if (cSocket.socket[i] != 0 && FD_ISSET(sd, &socketDs)) {
                //Check if it was for closing , and also read the
                //incoming message
                if (read(sd, buffer, BUFFER_SIZE) == 0) {

                    //Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *) &address, \
                        (socklen_t *) &addrlen);
                    log_warn("Host disconnected , ip %s , port %d ",
                             inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    pthread_mutex_lock(&cSocket.lock);
                    close(cSocket.socket[i]);
                    cSocket.socket[i] = 0;
                    cSocket.count--;
                    pthread_mutex_unlock(&cSocket.lock);

                } else {
                    log_debug(buffer);
                    char *hello = "Hello from Server";
                    send(sd, hello, strlen(hello), 0);
                    //set the string terminating NULL byte on the end
                    //of the data read
//                    buffer[valread] = '\0';
//                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }
}

void setSocketToFD() {
    //add child sockets to set
    for (int i = 0; i < MAX_CLIENT; i++) {
        //socket descriptor
        sd = cSocket.socket[i];

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

            if (data->socket[i] == 0) {
                data->socket[i] = new_socket;
                data->count++;

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

    pthread_mutex_lock(&cSocket.lock);
    cSocket.end = true;

    for (int i = 0; i < MAX_CLIENT; i++) {
        close(cSocket.socket[i]);
        cSocket.count--;
    }

    pthread_mutex_unlock(&cSocket.lock);

    close(server_fd);

    pthread_mutex_destroy(&cSocket.lock);

}