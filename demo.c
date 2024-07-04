#include <stdio.h>

#define CLOG_IMPLEMENTATION
#include "clog.h"




void demo(void) {
    clog(CLOG_DEBUG,   "Hello from CLog!");
    clog(CLOG_TRACE,   "Hello from CLog!");
    clog(CLOG_INFO,    "Hello from CLog!");
    clog(CLOG_WARNING, "Hello from CLog!");
    clog(CLOG_ERROR,   "Hello from CLog!");
    clog(CLOG_FATAL,   "Hello from CLog!");
}

int main(void) {
    demo();

    FILE *f = fopen("log.log", "w");

    clog_set_output(f);
    demo();

    fclose(f);

    return 0;
}
