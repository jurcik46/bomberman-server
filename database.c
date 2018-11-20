#include "database.h"
#include "logging/log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <sqlite3.h>



void initTables() {
    sqlite3_open(DB_NAME, &db);


    char *sql = "CREATE TABLE `Users` ("\
                 "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT," \
                 "`nick` TEXT NOT NULL UNIQUE," \
                 "`password` TEXT NOT NULL);";

    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        log_debug("Nepodarilo sa vytvorit tabulku Users:  %s", zErrMsg);
    } else {

        log_debug("Tabulka Users bola uspesne vytvorena.");
    }


    sql = "CREATE TABLE `Maps` ("\
            "`id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"\
            "`name` TEXT NOT NULL UNIQUE,"\
            "`path` TEXT NOT NULL UNIQUE"\
            ");";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        log_debug("Nepodarilo sa vytvorit tabulku Maps:  %s", zErrMsg);
    } else {

        log_debug("Tabulka Maps bola uspesne vytvorena.");
    }

    sql = "CREATE TABLE `LeaderBoard` ("\
           "`id` INTEGER NOT NULL UNIQUE,"\
           "`games` INTEGER DEFAULT 0,"\
           "`wins` INTEGER DEFAULT 0,"\
           "`loses` INTEGER DEFAULT 0,"\
           "`deads` INTEGER DEFAULT 0,"\
           "`kills` INTEGER DEFAULT 0,"\
           "FOREIGN KEY(`id`) REFERENCES `Users`(`id`),"\
           "PRIMARY KEY(`id`)"\
           ");";
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        log_debug("Nepodarilo sa vytvorit tabulku LeaderBoard:  %s", zErrMsg);
    } else {

        log_debug("Tabulka LeaderBoard bola uspesne vytvorena.");
    }

    sqlite3_free(zErrMsg);
    sqlite3_close(db);
}

int registration(char *nick, char *heslo) {
    int a = sqlite3_open(DB_NAME, &db);

    if (a) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return (0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    char sql[1000] = "INSERT INTO Users (`nick`,`password`) VALUES ('";
    strcat(sql, nick);
    strcat(sql, DB_SQL_VALUES_COMMA);
    strcat(sql, heslo);
    strcat(sql, DB_SQL_VALUES_END);

    log_debug("Registration SQL: \"%s\"", sql);

    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    int result = 0;
    if (rc != SQLITE_OK) {
        log_error("Nepodarilo sa registrovat hraca do tabulky Users Nick: %s  Error %s", nick, zErrMsg);
        sqlite3_free(zErrMsg);
    } else {

        log_debug("Podarilo sa registrovat hraca do tabulky Users Nick: %s ", nick);
        result = 1;
    }

    sqlite3_close(db);

    return result;
}

int login(char *nick, char *heslo) {
    sqlite3_open(DB_NAME, &db);
    sqlite3_stmt *stmt;
    int result = 0;
    char sql[1000] = "SELECT `id` as user_id,`nick`,  `password` as password FROM `Users` WHERE `nick` = '";
    strcat(sql, nick);
    strcat(sql, "';");

    log_debug("Login SQL: \"%s\"", sql);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_error("Chyba pri priprave sql prikazu:  %s", sqlite3_errmsg(db));
        result = 500;
    } else {
        log_debug("Login(select): sqlite3_preper succes");
    }
    do {


    } while ((rc = sqlite3_step(stmt)) == SQLITE_ROW);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

       
        int tmpId = sqlite3_column_int(stmt, 0);
        log_debug("Id %d", tmpId);
        const unsigned char *tmpNick = sqlite3_column_text(stmt, 1);
        log_debug("Nick %s", tmpNick);
        const unsigned char *tmpPswd = sqlite3_column_text(stmt, 2);
        log_debug("Password %s", tmpPswd);


        if (strcmp(heslo, (const char *) tmpPswd) == 0) {
            result = 200;
        } else {
            result = 401;
        }

    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}


int callback(void *NotUsed, int argc, char **argv, char **azColName) {

    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");

    return 0;

}


