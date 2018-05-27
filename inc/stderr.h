//
// Created by romain on 16/05/18.
//

#ifndef MYIRC_STDERR_H
#define MYIRC_STDERR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <asm/errno.h>
#include <errno.h>

#define eprintf(format, ...)    (fprintf(stderr, (format), ##__VA_ARGS__))

#define iprintf(from, errsav)   (fprintf(stderr, "%s error: %s\n", (from), strerror((errsav))))

#define raise_stacktrace(from)                                  \
do {                                                            \
    fprintf(stderr, "%s error: %s\n", (from), strerror(errno)); \
    abort();                                                    \
} while (0);                                                    \

#define raise_error(format, ...)                                \
do {                                                            \
    eprintf(format, ##__VA_ARGS__);                             \
    abort();                                                    \
} while (0);                                                    \

#define raise_errnum(from, errnum)                              \
do {                                                            \
    iprintf(from, errnum);                                      \
    abort();                                                    \
} while (0);                                                    \

#endif //MYIRC_STDERR_H