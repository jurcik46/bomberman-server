#include <stdio.h>
#include "logging/log.h"

int main() {

    log_set_quiet(0);
    log_set_level(LOG_DEBUG);
    FILE *logFile = fopen("logs.log","w+");
    log_set_fp(logFile);

    if (logFile == NULL){
        printf("Error! opening file");
        // Program exits if the file pointer returns NULL.
        return 1;
    }

//    log_trace(const char *fmt, ...);
//    log_debug(const char *fmt, ...);
//    log_info(const char *fmt, ...);
//    log_warn(const char *fmt, ...);
//    log_error(const char *fmt, ...);
//    log_fatal(const char *fmt, ...);

    log_debug("Hello %s", "world");
    log_debug("Hello %s", "world");
    log_debug("Hello %s", "world");
    log_debug("Hello %s", "world");
    log_debug("Hello %s", "world");

    fclose(logFile);
    return 0;
}