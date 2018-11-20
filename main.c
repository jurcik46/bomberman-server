#include <stdio.h>
#include <sqlite3.h>

#include "database.h"
#include "logging/log.h"

int main() {

    log_set_quiet(0);
    log_set_level(LOG_DEBUG);
    FILE *logFile = fopen("logs.log", "w+");
    log_set_fp(logFile);
//    initTables();
//    registration("aaaa", "bbb");
//    int a = login("aaaa", "bbb");
//    log_debug("%d", a);
    if (logFile == NULL) {
        printf("Error! opening file");
        // Program exits if the file pointer returns NULL.
        return 1;
    }

    fclose(logFile);
    return 0;
}