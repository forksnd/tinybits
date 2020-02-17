// simple logger. likely slow, though.
// - rlyeh, public domain.
//
// - [x] colors based on content.
// - [x] no logging categories. throughput flood configurable by percentage (see LOG_LEVEL var).
// - [x] print fatal errors always. with optional callstack (see LOG_PRINT_STACK var).

#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#  if !defined LOG_LEVEL && (defined NDEBUG || defined SHIPPING)
#define LOG_LEVEL 0   //   0% - no messages logged (only fatal errors will)
#elif !defined LOG_LEVEL
#define LOG_LEVEL 100 // 100% - all messages logged
#endif

#ifndef LOG_PRINT_STACK
#define LOG_PRINT_STACK()
#endif

#  if !defined LOG_ENABLE_ANSI && defined _WIN32
#include <winsock2.h>
#define LOG_ENABLE_ANSI() for(unsigned mode=0;!mode;mode=1) SetConsoleMode(GetStdHandle(-11), (GetConsoleMode(GetStdHandle(-11), &mode), mode|4))
#elif !defined LOG_ENABLE_ANSI
#define LOG_ENABLE_ANSI()
#endif

#define LOG(...) do { \
    static char must_log = -1; \
    static enum { undefined, base, r, g, y, p, t, } color = undefined; \
    const char *colors[] = { "", "\033[0m","\033[31m","\033[32m","\033[33m","\033[35m","\033[36m", }; /*b:34,w:37*/ \
    char buf[2048]; snprintf(buf, 2048, "" __VA_ARGS__); \
    if( color == undefined ) { \
        char low[2048]; int i; for(i=0;buf[i];++i) low[i] = 32|buf[i]; low[i]='\0'; \
        /**/ if( strstr(low,"fatal")|| strstr(low,"panic") ) color=r,must_log=1; \
        else if( strstr(low,"fail") || strstr(low,"error") ) color=r; \
        else if( strstr(low,"warn") || strstr(low,"alert") ) color=y; /*beware,caution*/ \
        else if( strstr(low,"info") || strstr(low,"succe") ) color=g; /*ok, no error*/ \
        else if( strstr(low,"debug") ) color=t; \
        else if( strstr(low,"trace") ) color=p; \
        if( must_log < 0 ) { /* original splitmix64 by Sebastiano Vigna (CC0)*/ \
            uint64_t z = (__LINE__ + __COUNTER__ + UINT64_C(0x9E3779B97F4A7C15)); \
            z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9); \
            must_log = (unsigned)((z ^ (z >> 31)) % 100) < LOG_LEVEL; } } \
    if( must_log ) { \
        LOG_ENABLE_ANSI(); \
        fprintf(stderr, "%s%s (%s:%d)\n", colors[color], buf, __FILE__, __LINE__); \
        if(color == r) LOG_PRINT_STACK(); \
        fprintf(stderr, "%s", colors[base]); /*restore color*/ } \
} while(0)

#if 0 // demo
int main() {
    LOG("Test 1 - normal message: hello world %d", 123);
    LOG("Test 2 - trace message");
    LOG("Test 3 - debug message");
    LOG("Test 4 - info message");
    LOG("Test 5 - warning message");
    LOG("Test 6 - error message (errors are always printed, despite LOG_LEVEL var)");
}
#endif
