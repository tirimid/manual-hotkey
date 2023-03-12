#ifndef DYNSTK_H__
#define DYNSTK_H__

#include <stddef.h>

struct dynstk {
    void *data;
    size_t size, cap, stride;
};

struct dynstk dynstk_create(size_t stride);
void dynstk_destroy(struct dynstk *ds);
void dynstk_push(struct dynstk *ds, void const *item);
void *dynstk_pop(struct dynstk *ds); // remember to `free()`.
void const *dynstk_peek(struct dynstk const *ds);

#endif
