//
// Created by Jurco on 19. 11. 2018.
//

#ifndef BOMBERMAN_SERVER_DATABASE_H
#define BOMBERMAN_SERVER_DATABASE_H

#include <sqlite3.h>

#define DB_NAME "../database.db"
#define DB_SQL_VALUES_COMMA "','"
#define DB_SQL_VALUES_END "');"

sqlite3 *db;
char *zErrMsg;

int callback(void *NotUsed, int argc, char **argv, char **azColName);

void initTables();

int registration(char *nick, char *heslo);

int login(char *nick, char *heslo);


#endif //BOMBERMAN_SERVER_DATABASE_H
