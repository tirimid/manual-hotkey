#ifndef PP_H__
#define PP_H__

#include <stddef.h>

#include "util/dynstr.h"

struct dynstr preprocess(char const *src, size_t src_len);

#endif
