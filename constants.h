//
// Created by Jurco on 20. 11. 2018.
//

#ifndef BOMBERMAN_SERVER_RESULT_CODE_H
#define BOMBERMAN_SERVER_RESULT_CODE_H

enum result_code {
    ZERO = 0, OK = 200, CREATED = 201, UNAUTHORIZED = 401, INTERNAL_SERVER_ERROR = 500
};

enum communication_type {
    LOGIN = 0,
};

#endif //BOMBERMAN_SERVER_RESULT_CODE_H
