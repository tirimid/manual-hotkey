#ifndef DYNSTR_H__
#define DYNSTR_H__

#include <stddef.h>

struct dynstr {
    char *data;
    size_t len, cap;
};

struct dynstr dynstr_create(void);
void dynstr_destroy(struct dynstr *s);
void dynstr_push_c_str(struct dynstr *s, char const *cs);
void dynstr_push_c_str_not_nt(struct dynstr *s, char const *cs, char term);
void dynstr_push_char(struct dynstr *s, char c);
char *dynstr_as_c_str(struct dynstr const *s); // must be freed.
void dynstr_print(struct dynstr const *s);

#endif
