#ifndef MYIRC_LIST_H
#define MYIRC_LIST_H

#include <sys/types.h>
#include <stdbool.h>

typedef struct list_s list_t;
typedef struct iter_s iter_t;
typedef void *ptr_t;
typedef void (*free_callback_t)(ptr_t);

#define iter_begin(list_ptr)    ((list_ptr)->begin)
#define iter_next(it_ptr)       ((it) = (it)->next)
#define iter_value(it_ptr)      ((it)->data)
#define list_init               (list_t) {NULL, NULL, 0}

#define ERROR_OUTOFBOUND    ("list_at error: index %zu out of bound, list size: %zu\n")
#define ERROR_NULL          ("data_null_check: NULL pointer cant be added or removed from a list\n")

struct iter_s {
    ptr_t data;
    iter_t *next;
};

struct list_s {
    iter_t *begin;
    iter_t *end;
    size_t size;
};

list_t  *list_new();
void    list_add(list_t *list, ptr_t elem);
void    list_remove(list_t *list, ptr_t elem);
ptr_t   list_at(list_t *list, size_t index);
void    list_clear(list_t *list, free_callback_t free_it);
bool    list_exists(list_t *list, ptr_t elem);


#endif //MYIRC_LIST_H
