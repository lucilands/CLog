#ifndef _CLOG_H
#define _CLOG_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    CLOG_NONE    = -1,
    CLOG_DEBUG   =  0,
    CLOG_TRACE   =  1,
    CLOG_INFO    =  2,
    CLOG_WARNING =  3,
    CLOG_ERROR   =  4,
    CLOG_FATAL   =  5,
} ClogLevel;


#define CLOG_INIT clog_output_fd = stdout

#define clog_mute_level(lvl) clog_muted_level = lvl
#define clog_set_output(output_fd) clog_output_fd = output_fd

#define clog(level, ...) if (level > clog_muted_level && clog_output_fd == stdout) { \
                            printf("%s[%s]: \e[0m", clog_get_level_color(level), clog_get_level_string(level)); \
                            printf(__VA_ARGS__); \
                        } \
                        else if (level > clog_muted_level && clog_output_fd != stdout) { \
                            fprintf(clog_output_fd, "[%s]: ", clog_get_level_string(level)); \
                            fprintf(clog_output_fd, __VA_ARGS__); \
                        }

extern ClogLevel clog_muted_level;
extern FILE *clog_output_fd;

FILE *clog_output_fd = 0;
ClogLevel clog_muted_level = CLOG_NONE;

const char *clog_get_level_string(ClogLevel level);
const char *clog_get_level_color(ClogLevel level);

const char *clog_get_level_string(ClogLevel level) {
    switch (level) {
        case CLOG_DEBUG:   return "DEBUG";
        case CLOG_TRACE:   return "TRACE";
        case CLOG_INFO:    return "INFO";
        case CLOG_WARNING: return "WARNING";
        case CLOG_ERROR:   return "ERROR";
        case CLOG_FATAL:   return "FATAL";
        default: case CLOG_NONE: {
            fprintf(stderr, "[FATAL]: Invalid ClogLevel supplied");
            exit(1);
        }
    }
}

const char *clog_get_level_color(ClogLevel level) {
    switch (level) {
        case CLOG_DEBUG:   return "\e[32m";
        case CLOG_TRACE:   return "\e[90m";
        case CLOG_INFO:    return "\e[97m";
        case CLOG_WARNING: return "\e[33m";
        case CLOG_ERROR:   return "\e[31m";
        case CLOG_FATAL:   return "\e[1m\e[91m";
        default: case CLOG_NONE:{
            fprintf(stderr, "[FATAL]: Invalid ClogLevel supplied");
            exit(1);
        }
    }
}

#endif //_CLOG_H