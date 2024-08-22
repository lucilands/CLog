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
#ifndef CLOG_NO_TIME
#ifdef _WIN32
#include <windows.h>
#elif defined(__unix__)
#include <time.h>
#include <sys/time.h>
#endif // _WIN32
#else
#ifndef CLOG_NO_TIME
#warning CLOG_TIME is not implemented for your operating system
#define CLOG_NO_TIME
#endif // CLOG_NO_TIME
#endif // CLOG_NO_TIME

#define CLOG_COLOR_BLACK      "\e[30m"
#define CLOG_COLOR_RED        "\e[31m"
#define CLOG_COLOR_GREEN      "\e[32m"
#define CLOG_COLOR_YELLOW     "\e[33m"
#define CLOG_COLOR_BLUE       "\e[34m"
#define CLOG_COLOR_MAGENTA    "\e[35m"
#define CLOG_COLOR_CYAN       "\e[36m"
#define CLOG_COLOR_WHITE      "\e[37m"
#define CLOG_COLOR_DEFAULT    "\e[39m"

#define CLOG_COLOR_BLACK_BG   "\e[40m"
#define CLOG_COLOR_RED_BG     "\e[41m"
#define CLOG_COLOR_GREEN_BG   "\e[42m"
#define CLOG_COLOR_YELLOW_BG  "\e[43m"
#define CLOG_COLOR_BLUE_BG    "\e[44m"
#define CLOG_COLOR_MAGENTA_BG "\e[45m"
#define CLOG_COLOR_CYAN_BG    "\e[46m"
#define CLOG_COLOR_WHITE_BG   "\e[47m"
#define CLOG_COLOR_DEFAULT_BG "\e[49m"

#define CLOG_COLOR_BOLD       "\e[1m"
#define CLOG_COLOR_FAINT      "\e[2m"
#define CLOG_COLOR_ITALIC     "\e[3m"

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS 
        #ifdef _MSC_VER
            #define __FUNCTION_NAME__ __FUNCSIG__
        #else
            #define __FUNCTION_NAME__   __PRETTY_FUNCTION__ 
        #endif //__MSC_VER
    #else          //*NIX
        #define __FUNCTION_NAME__   __PRETTY_FUNCTION__ 
    #endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif //__cplusplus

#ifndef CLOG_BUF_LIMIT
#define CLOG_BUF_LIMIT 1024
#endif
#define CLOG_REGISTER_LEVEL(name_, color_, severity_) (const ClogLevel) {.name = name_, .color = color_, .severity = severity_}


typedef struct ClogLevel {
    const char *name;
    const char *color;
    const int severity;
} ClogLevel;


#define clog_mute_level(lvl) clog_muted_level = lvl.severity
#define clog_set_output(output_fd) clog_output_fd = output_fd
#define clog_set_fmt(fmt) clog_fmt = (char*)fmt
#ifndef CLOG_NO_TIME
#define clog_set_time_fmt(fmt) clog_time_fmt = (char*)fmt
#endif // CLOG_NO_TIME

#define clog(level, ...) __clog(level, __FILE__, __LINE__, __FUNCTION_NAME__, __VA_ARGS__)

extern const ClogLevel CLOG_NONE;
extern const ClogLevel CLOG_DEBUG;
extern const ClogLevel CLOG_TRACE;
extern const ClogLevel CLOG_INFO;
extern const ClogLevel CLOG_WARNING;
extern const ClogLevel CLOG_ERROR;
extern const ClogLevel CLOG_FATAL;


extern FILE *clog_output_fd;
extern char *clog_fmt;
extern const char *clog_fmt_default;
extern char *clog_time_fmt;
extern int clog_muted_level;

void __clog(ClogLevel level, const char *file, int line, const char *func, const char *fmt, ...);
#ifndef CLOG_NO_TIME
void clog_get_timestamp(char *output);
#else
void clog_get_timestamp(char *output) {(void)output;};
#endif

#define clog_assert(expr) if (!(expr)) {clog(CLOG_FATAL, "Assertion failed!! "#expr" exiting..."); exit(1);}

#ifdef CLOG_IMPLEMENTATION
const ClogLevel CLOG_DEBUG   = CLOG_REGISTER_LEVEL("DEBUG",   CLOG_COLOR_GREEN,                  0);
const ClogLevel CLOG_TRACE   = CLOG_REGISTER_LEVEL("TRACE",   CLOG_COLOR_WHITE CLOG_COLOR_FAINT, 1);
const ClogLevel CLOG_INFO    = CLOG_REGISTER_LEVEL("INFO",    CLOG_COLOR_WHITE,                  2);
const ClogLevel CLOG_WARNING = CLOG_REGISTER_LEVEL("WARNING", CLOG_COLOR_YELLOW,                 3);
const ClogLevel CLOG_ERROR   = CLOG_REGISTER_LEVEL("ERROR",   CLOG_COLOR_RED,                    4);
const ClogLevel CLOG_FATAL   = CLOG_REGISTER_LEVEL("FATAL",   CLOG_COLOR_BOLD CLOG_COLOR_RED,    5);

int clog_muted_level = -1;

int __clog_errno = 0;

FILE *clog_output_fd = 0;
#ifndef CLOG_NO_TIME
    const char *clog_fmt_default = "%t: %f:%l (%F) -> %c[%L]%r: %m";
    char *clog_fmt = (char*)"%t: %f:%l (%F) -> %c[%L]%r: %m";
    char *clog_time_fmt = (char*)"%h:%m:%s.%u";
#else
    const char *clog_fmt_default = (char*)"%f:%l (%F) -> %c[%L]%r: %m";
    char *clog_fmt = (char*)"%f:%l (%F) -> %c[%L]%r: %m";
#endif

size_t __clog_buffer_size(const char *fmt, va_list args) {
    int res = vsnprintf(NULL, 0, fmt, args);
    return res + 1;
}

size_t __clog_sprintf(char *target, size_t cur_len, size_t max_len, const char *fmt, ...) {
    if (__clog_errno == 1) return 0;

    va_list args;
    va_start(args, fmt);

    size_t len = __clog_buffer_size(fmt, args);
    va_end(args);
    va_start(args, fmt);

    if (cur_len + len >= max_len) {
        size_t ret = vsnprintf(target, max_len - len - 5, fmt, args);
        strncat(target, "...", 4);
        ret += 4;
        va_end(args);

        __clog_errno = 1;
        return ret;
    }
    else {
        size_t ret = vsprintf(target, fmt, args);
        va_end(args);
        return ret;
    }
}



size_t __clog_vsprintf(char *target, size_t cur_len, size_t max_len, const char *fmt, size_t len, va_list args) {
    if (__clog_errno == 1) return 0;
    if (cur_len + len >= max_len) {
        size_t ret = vsnprintf(target, max_len - cur_len - 5, fmt, args);
        strncat(target, "...", 4);
        ret += 4;
        va_end(args);

        __clog_errno = 1;
        return ret;
    }
    else {
        size_t ret = vsprintf(target, fmt, args);
        return ret;
    }
}


void __clog(ClogLevel level, const char *file, int line, const char *func, const char *fmt, ...) {
    if (level.severity > clog_muted_level) {
        __clog_errno = 0;
        va_list args;
        va_start(args, fmt);

        if (!clog_output_fd) clog_output_fd = stdout;
        va_end(args);
        va_start(args, fmt);


        char *target = malloc(CLOG_BUF_LIMIT);
        memset(target, '\0', CLOG_BUF_LIMIT);

        size_t len = 0;

        for (size_t i = 0; i < strlen(clog_fmt); i++) {
            char c = clog_fmt[i];
            if (c == '%') {
            char c = clog_fmt[++i];
            char b[50];
            memset(b, '\0', 50);
            switch (c) {
                case 'c':
                    if (clog_output_fd == stdout || clog_output_fd == stderr) {
                        len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%s", level.color);
                    }
                    break;
                case 'L':
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%s", level.name);
                    break;
                case 'r':
                    if (clog_output_fd == stdout || clog_output_fd == stderr) {
                        len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "\e[0m");
                    }
                    break;
                case 'm':
                    size_t msg_len = __clog_buffer_size(fmt, args);
                    va_start(args, fmt);
                    len += __clog_vsprintf(target + len, len, CLOG_BUF_LIMIT, fmt, msg_len, args);
                    break;
                case '%':
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%c", '%');
                    break;
                case 'f':
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%s", file);
                    break;
                case 't':
                    clog_get_timestamp(b);
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, b);
                    break;
                case 'l':
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%d", line);
                    break;
                case 'F':
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%s", func);
                    break;
                default:
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%c", c);
                    break;
                }
            }
            else len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%c", c);


        }
        if (clog_output_fd == stdout || clog_output_fd == stderr) fprintf(clog_output_fd, "%s\e[0m\n", target);
        else fprintf(clog_output_fd, "%s\n", target);
        free(target);
    }
}

#ifndef CLOG_NO_TIME
void clog_get_timestamp(char *output) {
    char buf[50];
    memset(buf, 0, 50);

    int buf_idx = 0;

    int hour, minute, second = 0;
    unsigned long int millisecond = 0;
    #ifdef _WIN32
        SYSTEMTIME t;
        GetSystemTime(&t);
        hour = t.wHour+1;
        minute = t.wMinute;
        second = t.wSecond;
        millisecond = t.wMilliseconds;
    #elif defined(__unix__)
        time_t t = time(NULL);
        struct tm *time = localtime(&t);

        struct timeval time_val;
        gettimeofday(&time_val, NULL);

        hour = time->tm_hour;
        minute = time->tm_min;
        second = time->tm_sec;
        millisecond = (time_val.tv_sec * 1000 + time_val.tv_usec / 1000) % 1000;
    #endif


    for (size_t i = 0; i < strlen(clog_time_fmt); ++i) {
        char c = clog_time_fmt[i];
        if (c == '%') {
            c = clog_time_fmt[++i];
            switch (c) {
                case 'h':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%02.2i", hour);
                    break;
                case 'm':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%02.2i", minute);
                    break;
                case 's':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%02.2i", second);
                    break;
                case 'u':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%03.3lu", millisecond);
                    break;

                default: break;
            }
        }
        else {
            buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%c", c);
        }
    }
    strncat(output, buf, strlen(buf));
}


#endif //CLOG_NO_TIME
#endif //CLOG_IMPLEMENTATION

#if defined(__cplusplus)
}
#endif //__cplusplus

#endif //_CLOG_
