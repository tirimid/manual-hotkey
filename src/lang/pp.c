#include "lang/pp.h"

struct dynstr preprocess(char const *src, size_t src_len)
{
    struct dynstr ds = dynstr_create();
    
    // stage 1: remove comments.
    for (size_t i = 0; i < src_len - 1; ++i) {
        // do not remove `//`s in string literals.
        if (src[i] == '"') {
            for (++i; i < src_len && src[i] != '"'; ++i);
            continue;
        }
        
        if (src[i] == '/' && src[i + 1] == '/')
            for (; i < src_len && src[i] != '\n'; ++i);

        dynstr_push_char(&ds, src[i]);
    }

    return ds;
}
