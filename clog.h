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
#include <Windows.h>
#elif defined(__unix__)
#include <time.h>
#endif // _WIN32
#else
#ifndef CLOG_NO_TIME
#warning CLOG_TIME is not implemented for your operating system
#define CLOG_NO_TIME
#endif // CLOG_NO_TIME
#endif // CLOG_NO_TIME

#if defined(__cplusplus)
extern "C" {
#endif //__cplusplus

#define CLOG_BUF_LIMIT 1024
#define CLOG_REGISTER_LEVEL(name_, color, severity_) (const ClogLevel) {.name = name_, .color_escape_char = color, .severity = severity_}


typedef struct ClogLevel {
    const char *name;
    const char *color_escape_char;
    const int severity;
} ClogLevel;


#define clog_mute_level(lvl) clog_muted_level = lvl
#define clog_set_output(output_fd) clog_output_fd = output_fd
#define clog_set_fmt(fmt) clog_fmt = (char*)fmt
#ifndef CLOG_NO_TIME
#define clog_set_time_fmt(fmt) clog_time_fmt = (char*)fmt
#endif // CLOG_NO_TIME

#define clog(level, ...) if (level.severity >= clog_muted_level.severity) __clog(level, __FILE__, __LINE__, __VA_ARGS__)

ClogLevel clog_muted_level;
const ClogLevel CLOG_NONE;
const ClogLevel CLOG_DEBUG;
const ClogLevel CLOG_TRACE;
const ClogLevel CLOG_INFO;
const ClogLevel CLOG_WARNING;
const ClogLevel CLOG_ERROR;
const ClogLevel CLOG_FATAL;


FILE *clog_output_fd;
char *clog_fmt;
const char *clog_fmt_default;
char *clog_time_fmt;


void __clog(ClogLevel level, const char *file, int line, const char *fmt, ...);
#ifndef CLOG_NO_TIME
void clog_get_timestamp(char *tm);
#else
void clog_get_timestamp(char *tm) {(void)tm;};
#endif


#ifdef CLOG_IMPLEMENTATION
const ClogLevel CLOG_NONE    = CLOG_REGISTER_LEVEL("",        "",           -1);
const ClogLevel CLOG_DEBUG   = CLOG_REGISTER_LEVEL("DEBUG",   "\e[32m",      0);
const ClogLevel CLOG_TRACE   = CLOG_REGISTER_LEVEL("TRACE",   "\e[90m",      1);
const ClogLevel CLOG_INFO    = CLOG_REGISTER_LEVEL("INFO",    "\e[97m",      2);
const ClogLevel CLOG_WARNING = CLOG_REGISTER_LEVEL("WARNING", "\e[33m",      3);
const ClogLevel CLOG_ERROR   = CLOG_REGISTER_LEVEL("ERROR",   "\e[31m",      4);
const ClogLevel CLOG_FATAL   = CLOG_REGISTER_LEVEL("FATAL",   "\e[1m\e[91m", 5);


FILE *clog_output_fd = 0;
ClogLevel clog_muted_level = CLOG_NONE;
const char *clog_fmt_default = "%t: %f:%l -> %c[%L]%r: %m";
#ifndef CLOG_NO_TIME
    char *clog_fmt = (char*)"%t: %f:%l -> %c[%L]%r: %m";
    char *clog_time_fmt = (char*)"%h:%m:%s.%u";
#else
    char *clog_fmt = (char*)"%f:%l -> %c[%L]%r: %m";
#endif

size_t __clog_buffer_size(const char *fmt, va_list args) {
    int res = vsnprintf(NULL, 0, fmt, args);
    return res + 1;
}

size_t __clog_sprintf(char *target, size_t cur_len, size_t max_len, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    size_t len = __clog_buffer_size(fmt, args);
    va_end(args);
    va_start(args, fmt);

    if (cur_len + len >= max_len) {
        clog(CLOG_WARNING, "CLog message too large!!");
        size_t ret = vsnprintf(target, max_len - cur_len - 5, fmt, args);
        strncat(target, "...", 4);
        ret += 4;
        va_end(args);

        return ret;
    }
    else {
        size_t ret = vsprintf(target, fmt, args);
        va_end(args);
        return ret;
    }
}



size_t __clog_vsprintf(char *target, size_t cur_len, size_t max_len, const char *fmt, size_t len, va_list args) {
    if (cur_len + len >= max_len) {
        clog(CLOG_WARNING, "CLog message too large");
        size_t ret = vsnprintf(target, max_len - cur_len - 5, fmt, args);
        strncat(target, "...", 4);
        ret += 4;

        return ret;
    }
    else {
        size_t ret = vsprintf(target, fmt, args);
        return ret;
    }
}


void __clog(ClogLevel level, const char *file, int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (!clog_output_fd) clog_output_fd = stdout;

    char *target = malloc(CLOG_BUF_LIMIT);

    size_t len = 0;

    for (size_t i = 0; i < strlen(clog_fmt); i++) {
        char c = clog_fmt[i];
        if (c == '%') {
        char c = clog_fmt[++i];
        char b[50];
        switch (c) {
            case 'c':
                if (clog_output_fd == stdout || clog_output_fd == stderr) {
                    len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%s", level.color_escape_char);
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
                len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%s", b);
                break;
            case 'l':
                len += __clog_sprintf(target + len, len, CLOG_BUF_LIMIT, "%d", line);
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

#ifndef CLOG_NO_TIME
void clog_get_timestamp(char *tm) {
    char buf[50];
    memset(buf, 0, 50);

    int buf_idx = 0;

    int hour, minute, second, millisecond = 0;
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
        hour = time->tm_hour;
        minute = time->tm_min;
        second = time->tm_sec;
        millisecond = 0;
    #endif


    for (size_t i = 0; i < strlen(clog_time_fmt); ++i) {
        char c = clog_time_fmt[i];
        if (c == '%') {
            c = clog_time_fmt[++i];
            switch (c) {
                case 'h':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%02i", hour);
                    break;
                case 'm':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%02i", minute);
                    break;
                case 's':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%02i", second);
                    break;
                case 'u':
                    buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%03i", millisecond);
                    break;

                default: break;
            }
        }
        else {
            buf_idx += __clog_sprintf(buf + buf_idx, buf_idx, 50, "%c", c);
        }
    }
    strncpy(tm, buf, strlen(buf));
}


#endif //CLOG_NO_TIME
#endif //CLOG_IMPLEMENTATION

#if defined(__cplusplus)
}
#endif //__cplusplus

#endif //_CLOG_
