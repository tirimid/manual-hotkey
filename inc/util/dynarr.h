#ifndef DYNARR_H__
#define DYNARR_H__

#include <stddef.h>

struct dynarr {
        void *data;
        size_t size, cap, stride;
};

struct dynarr dynarr_create(size_t stride);
void dynarr_destroy(struct dynarr *da);
void dynarr_push(struct dynarr *da, void const *item);
void const *dynarr_get(struct dynarr const *da, size_t ind);

#endif
