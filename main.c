#include <stdio.h>
#include "clog.h"



void demo(void) {
    clog(CLOG_DEBUG,   "Hello from CLog!\n");
    clog(CLOG_TRACE,   "Hello from CLog!\n");
    clog(CLOG_INFO,    "Hello from CLog!\n");
    clog(CLOG_WARNING, "Hello from CLog!\n");
    clog(CLOG_ERROR,   "Hello from CLog!\n");
    clog(CLOG_FATAL,   "Hello from CLog!\n");
}

int main(void) {
    CLOG_INIT;
    demo();
    
    FILE *f = fopen("log.log", "w");

    clog_set_output(f);
    demo();

    fclose(f);

    return 0;
}