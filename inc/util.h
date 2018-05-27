//
// Created by romain on 26/05/18.
//

#ifndef MYIRC_UTIL_H
#define MYIRC_UTIL_H

#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

#define not_split(x)    (!(x) || !(*(x)) || !(x)[1])

char **charset_split(char const *str, size_t len, char const *delimiter);

size_t str_array_length(char **array);

bool charset_ends_with(char const *str, size_t len, char const *needle);

void str_free_array(char **array);

#endif //MYIRC_UTIL_H
