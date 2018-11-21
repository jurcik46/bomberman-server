//
// Created by Jurco on 20. 11. 2018.
//

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
     * Init new thread for accepting new clients connnections
     */
    cSocket.end = false;
    cSocket.count = 0;
    pthread_mutex_init(&cSocket.lock, NULL);

    pthread_create(&acceptSocketThread, NULL, &accpetSocketThreadFun, &cSocket);

}

void startCommunication() {


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
        data->count++;
        data->socket[data->count] = new_socket;
        pthread_mutex_unlock(&data->lock);
        log_debug("New Socket Added Count %d", data->count);

    };

    log_debug("Exit socketThread");
    pthread_exit(NULL);
}


void closeSocket() {

    close(server_fd);
    pthread_mutex_lock(&cSocket.lock);
    cSocket.end = true;
    while (cSocket.count > 0) {
        close(cSocket.socket[cSocket.count]);
        cSocket.count--;
    }
    pthread_mutex_unlock(&cSocket.lock);

    pthread_mutex_destroy(&cSocket.lock);
}