/*
The MIT License (MIT)

Copyright (c) 2024 TheHerowither

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _CLOG_H
#define _CLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef CLOG_USE_SYSTEM_TIME
#include <time.h>
#elif defined(_WIN32) && !defined(CLOG_NO_TIME)
#include <Windows.h>
#else
#ifndef CLOG_NO_TIME
#warning CLOG_TIME is not implemented for your operating system
#define CLOG_NO_TIME
#endif
#endif

#define CLOG_BUF_LIMIT 2048


// TODO: Find a way to remove CLOG_INIT

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
#ifndef CLOG_NO_TIME
#define clog_set_time_fmt(fmt) clog_time_fmt = fmt
#endif

#define clog(level, ...) if (level >= clog_muted_level) __clog(level, __FILE__, __LINE__, __VA_ARGS__)

extern ClogLevel clog_muted_level;
extern FILE *clog_output_fd;
extern char *clog_fmt;
extern const char *clog_fmt_default;
extern char *clog_time_fmt;


void __clog(ClogLevel level, const char *file, int line, const char *fmt, ...);
const char * __cdecl clog_get_level_string(ClogLevel level);
const char * __cdecl clog_get_level_color(ClogLevel level);
#ifndef CLOG_NO_TIME
void __cdecl clog_get_timestamp(char *tm);
#else
void __cdecl clog_get_timestamp(char *tm) {(void)tm;};
#endif


#ifdef CLOG_IMPLEMENTATION
FILE *clog_output_fd = 0;
ClogLevel clog_muted_level = CLOG_NONE;
const char *clog_fmt_default = "%t: %f:%l -> %c[%L]%r: %m";
#ifndef CLOG_NO_TIME
    char *clog_fmt = (char*)"%t: %f:%l -> %c[%L]%r: %m";
    char *clog_time_fmt = (char*)"%h:%m:%s.%u";
#else
    char *clog_fmt = (char*)"%f:%l -> %c[%L]%r: %m";
#endif

const char * __cdecl clog_get_level_string(ClogLevel level) {
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

const char * __cdecl clog_get_level_color(ClogLevel level) {
    switch (level) {
        case CLOG_DEBUG:   return "\e[32m";
        case CLOG_TRACE:   return "\e[90m";
        case CLOG_INFO:    return "\e[97m";
        case CLOG_WARNING: return "\e[33m";
        case CLOG_ERROR:   return "\e[31m";
        case CLOG_FATAL:   return "\e[1m\e[91m";
        default: case CLOG_NONE:{
            clog(CLOG_WARNING, "Invalid log level supplied");
            return "";
        }
    }
}

void __cdecl __clog(ClogLevel level, const char *file, int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char target[CLOG_BUF_LIMIT];
    size_t len = 0;

    for (size_t i = 0; i < strlen(clog_fmt); i++) {                      
        char c = clog_fmt[i];
        if (c == '%') {
        char c = clog_fmt[++i];
        char b[50];
        switch (c) {
            case 'c':
                if (clog_output_fd == stdout || clog_output_fd == stderr) {
                    len += sprintf(target + len, "%s", clog_get_level_color(level));
                }
                break;
            case 'L':
                len += sprintf(target + len, "%s", clog_get_level_string(level));
                break;
            case 'r':
                if (clog_output_fd == stdout || clog_output_fd == stderr) {
                    len += sprintf(target + len, "\e[0m");
                }
                break;
            case 'm':
                len += vsprintf(target + len, fmt, args);
                break;
            case '%':
                len += sprintf(target + len, "%c", '%');
                break;
            case 'f':
                len += sprintf(target + len, "%s", file);
                break;
            case 't':
                clog_get_timestamp(b);
                len += sprintf(target + len, "%s", b);
                break;
            case 'l':
                len += sprintf(target + len, "%d", line);
                break;
            default:
                len += sprintf(target + len, "%c", c);
                break;
            }
        }
        else len += sprintf(target + len, "%c", c);
    }                                                                         
    if (clog_output_fd == stdout || clog_output_fd == stderr) fprintf(clog_output_fd, "%s\e[0m\n", target);
    else fprintf(clog_output_fd, "%s\n", target);
}

#ifndef CLOG_NO_TIME
void __cdecl clog_get_timestamp(char *tm) {
    char buf[50] = {0};
    int hour, minute, second, millisecond;
    #ifdef _WIN32
        SYSTEMTIME t;
        GetSystemTime(&t);
        hour = t.wHour+1;
        minute = t.wMinute;
        second = t.wSecond;
        millisecond = t.wMilliseconds;
    #endif


    for (size_t i = 0; i < strlen(clog_time_fmt); ++i) {
        char c = clog_time_fmt[i];
        if (c == '%') {
            c = clog_time_fmt[++i];
            char tmp[10] = {0};
            switch (c) {
                case 'h': 
                    tmp[0] = '\0';
                    sprintf(tmp, "%02d", hour);
                    strncat(buf, tmp, 2);
                    break;
                case 'm': 
                    tmp[0] = '\0';
                    sprintf(tmp, "%02d", minute);
                    strncat(buf, tmp, 2);
                    break;
                case 's': 
                    tmp[0] = '\0';
                    sprintf(tmp, "%02d", second);
                    strncat(buf, tmp, 2);
                    break;
                case 'u':
                    tmp[0] = '\0';
                    sprintf(tmp, "%03d", millisecond);
                    strncat(buf, tmp, 3);
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
#endif //CLOG_NO_TIME
#endif //CLOG_IMPLEMENTATION
#endif //_CLOG_H