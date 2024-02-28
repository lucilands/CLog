#include <stdio.h>
#include "clog.h"



int main(void) {
    clog(CLOG_DEBUG,   "Hello from CLog!\n");
    clog(CLOG_TRACE,   "Hello from CLog!\n");
    clog(CLOG_INFO,    "Hello from CLog!\n");
    clog(CLOG_WARNING, "Hello from CLog!\n");
    clog(CLOG_ERROR,   "Hello from CLog!\n");
    clog(CLOG_FATAL,   "Hello from CLog!\n");

    return 0;
}