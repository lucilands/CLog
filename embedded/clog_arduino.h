#ifndef _CLOG_H
#define _CLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

// TODO: Find a way to remove CLOG_INIT
// TODO: Implement timestamps for milliseconds and less

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
#define clog_set_fmt(fmt) clog_fmt = fmt
#define clog_set_time_fmt(fmt) clog_time_fmt = fmt
char __buf[1024];
#define clog_ard_printf(...)  __buf[0] = '\0';\
                                  sprintf(__buf, __VA_ARGS__);\
                                  Serial.print(__buf)

char *get_file(char *path) {
  char *buf = {0};

  char *pch = strpbrk (path, "\\");
  while (pch != NULL)
  {
    buf = pch;
    pch = strpbrk (pch+1,"\\");
  }
  return buf;
}

#define clog(level, ...) for (size_t i = 0; i < strlen(clog_fmt); i++) {                       \
                         char c = clog_fmt[i];                                                 \
                         if (c == '%') {                                                       \
                            char c = clog_fmt[++i];                                            \
                            switch (c)                                                         \
                            {                                                                  \
                            case 'L':                                                          \
                                  clog_ard_printf("%s", clog_get_level_string(level));   \
                                break;                                                         \
                            case 'm':                                                          \
                                  clog_ard_printf(__VA_ARGS__);                          \
                                break;                                                         \
                            case '%':                                                          \
                                  clog_ard_printf("%c", '%');                            \
                                break;                                                         \
                            case 'f':                                                          \
                                  clog_ard_printf("%s", get_file(__FILE__));               \
                                break;                                                         \
                            case 't':                                                          \
                                char b[50];                                              \
                                clog_get_timestamp(b);                                         \
                                  clog_ard_printf("%s", b);                              \
                                break;                                                         \
                            case 'l':                                                          \
                                  clog_ard_printf("%i", __LINE__);                       \
                                break;                                                         \
                            default:                                                           \
                                  clog_ard_printf("%c", c);                              \
                                break;                                                         \
                            }                                                                  \
                        }                                                                      \
                        else {                                                                 \
                              clog_ard_printf("%c", c);                                  \
                        }                                                                      \
                    }                                                                          \
                    clog_ard_printf("\n");

extern ClogLevel clog_muted_level;
extern FILE *clog_output_fd;
extern char *clog_fmt;
extern const char *clog_fmt_default;
extern char *clog_time_fmt;

FILE *clog_output_fd = 0;
ClogLevel clog_muted_level = CLOG_NONE;
const char *clog_fmt_default = "%t: %f:%l -> [%L]: %m";
char *clog_fmt = "%t: %f:%l -> [%L]: %m";
char *clog_time_fmt = "%h:%m:%s";

const char *clog_get_level_string(ClogLevel level);
void clog_get_timestamp(char *tm);
char *clog_set_digits(const size_t num_digits, char *str);

const char *clog_get_level_string(ClogLevel level) {
    switch (level) {
        case CLOG_DEBUG:   return "DEBUG";
        case CLOG_TRACE:   return "TRACE";
        case CLOG_INFO:    return "INFO";
        case CLOG_WARNING: return "WARNING";
        case CLOG_ERROR:   return "ERROR";
        case CLOG_FATAL:   return "FATAL";
        default: case CLOG_NONE: {
            clog(CLOG_WARNING, "Invalid log level supplied");
            return "";
        }
    }
}

char *clog_set_digits(const size_t num_digits, char *str) {
    if (strlen(str) > num_digits) {
        str[num_digits+1] = '\0';
    }
    else if (strlen(str) < num_digits) {
        for (size_t i = 0; i < (num_digits - strlen(str)); i++) {
            char s[100] = {0};
            s[0] = '0';
            strcat(s, str);
            strcpy(str, s);
        }
        return str;
    }
    return str;
}

void clog_get_timestamp(char *tm) {
    char buf[50] = {0};
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime(&rawtime);


    for (size_t i = 0; i < strlen(clog_time_fmt); ++i) {
        char c = clog_time_fmt[i];
        if (c == '%') {
            c = clog_time_fmt[++i];
            char tmp[2] = {0};
            switch (c) {
                case 'h': 
                    tmp[0] = '\0';
                    itoa(timeinfo->tm_hour, tmp, 10);
                    strncat(buf, clog_set_digits(2, tmp), 2);
                    break;
                case 'm': 
                    tmp[0] = '\0';
                    itoa(timeinfo->tm_min, tmp, 10);
                    strncat(buf, clog_set_digits(2, tmp), 2);
                    break;
                case 's': 
                    tmp[0] = '\0';
                    itoa(timeinfo->tm_sec, tmp, 10);
                    strncat(buf, clog_set_digits(2, tmp), 2);
                    break;

                default: break;
            }
        }
        else {
            strncat(buf, &c, 1);
        }
    }
    strncpy(tm, buf, strlen(buf));
}

#endif //_CLOG_H