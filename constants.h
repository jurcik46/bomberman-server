//
// Created by Jurco on 20. 11. 2018.
//

#ifndef BOMBERMAN_SERVER_RESULT_CODE_H
#define BOMBERMAN_SERVER_RESULT_CODE_H

enum result_code {
    ZERO = 0, OKEJ = 200, CREATED = 201, LAST_DATA = 204, UNAUTHORIZED = 401, INTERNAL_SERVER_ERROR = 500,
    SERVICE_UNAVAILABLE = 503

};

enum communication_type {
    LOGIN, CREATE_GAME, REGISTRATION,  GET_LOBBY_PLAYER, FIND_SERVERS
};

#endif //BOMBERMAN_SERVER_RESULT_CODE_H
