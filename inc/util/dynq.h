#ifndef DYNQ_H__
#define DYNQ_H__

#include <stddef.h>

struct dynq {
    void *data;
    size_t size, cap, stride;
};

struct dynq dynq_create(size_t stride);
void dynq_destroy(struct dynq *dq);
void dynq_enqueue(struct dynq *dq, void const *item);
void *dynq_dequeue(struct dynq *dq); // remember to `free()`.
void const *dynq_peek(struct dynq const *dq);

#endif
