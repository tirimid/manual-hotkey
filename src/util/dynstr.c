#include "util/dynstr.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STARTING_CAP 1

struct dynstr dynstr_create(void)
{
    return (struct dynstr){
        .data = malloc(STARTING_CAP),
        .cap = STARTING_CAP,
        .len = 0,
    };
}

void dynstr_destroy(struct dynstr *s)
{
    free(s->data);
}

void dynstr_push_c_str(struct dynstr *s, char const *cs)
{
    for (size_t i = 0; i < strlen(cs); ++i)
        dynstr_push_char(s, cs[i]);
}

void dynstr_push_c_str_not_nt(struct dynstr *s, char const *cs, char term)
{
    for (size_t i = 0; cs[i] != term; ++i)
        dynstr_push_char(s, cs[i]);
}

void dynstr_push_char(struct dynstr *s, char c)
{
    if (s->len >= s->cap) {
        s->cap *= 2;
        s->data = realloc(s->data, s->cap);
    }

    s->data[s->len++] = c;
}

char *dynstr_as_c_str(struct dynstr const *s)
{
    char *cs = malloc(s->len + 1);
    memcpy(cs, s->data, s->len);
    cs[s->len] = '\0';
    return cs;
}

void dynstr_print(struct dynstr const *s)
{
    for (size_t i = 0; i < s->len; ++i)
        putc(s->data[i], stdout);
}
