#include "dynarr.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STARTING_CAP 1

struct dynarr dynarr_create(size_t stride)
{
    return (struct dynarr){
        .data = malloc(stride * STARTING_CAP),
        .size = 0,
        .cap = STARTING_CAP,
        .stride = stride,
    };
}

void dynarr_destroy(struct dynarr *da)
{
    free(da->data);
}

void dynarr_push(struct dynarr *da, void const *item)
{
    if (da->size >= da->cap) {
        da->cap *= 2;
        da->data = realloc(da->data, da->cap * da->stride);
    }

    memcpy((uint8_t *)da->data + da->size++ * da->stride, item, da->stride);
}

void const *dynarr_get(struct dynarr const *da, size_t ind)
{
    return (uint8_t const *)da->data + ind * da->stride;
}
