#ifndef UTIL_H
#define UTIL_H

#include <cstdio>

#define ASSERTCRASH() *(int *)(uintptr_t)0xbbadbeef = 0
#define ASSERT(cond)                                        \
    do                                                      \
    {                                                       \
        if (cond)                                           \
            break;                                          \
        fprintf(stderr, "%s:%d: failed assertion \"%s\"\n", \
                __FILE__, __LINE__, #cond);                 \
        ASSERTCRASH();                                      \
    } while (false)

#endif