#include "lang/lex.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// quick and simple notation for defining lexer rules.
// the following variables are provided to the block of a lexer rule:
// `char const *src`: the source code as a string.
// `size_t src_len`: the length of the source code.
// `size_t start`: index to the start of the token in `src`.
#define LEXER_RULE(rule_name) static struct token rule_name(char const *src, \
                                                            size_t src_len, \
                                                            size_t start)

static char const *token_type_names[TOKEN_TYPE_LAST__] = {
    "identifier",
    "integer_literal",
    "float_literal",
    "string_literal",
    "char_literal",
    "int",
    "float",
    "char",
    "string",
    "key",
    "bool",
    "defun",
    "return",
    "mhk",
    "mhkc",
    "register",
    "key_press",
    "key_release",
    "init",
    "quit",
    "backtick",
    "tilde",
    "exclamation",
    "at",
    "octothorpe",
    "dollar",
    "percent",
    "caret",
    "ampersand",
    "asterisk",
    "hyphen",
    "equals",
    "plus",
    "semicolon",
    "colon",
    "pipe",
    "comma",
    "period",
    "slash",
    "question",
    "linefeed",
    "left_paren",
    "right_paren",
    "left_bracket",
    "right_bracket",
    "left_brace",
    "right_bracket",
    "left_angle",
    "right_angle",
};

void token_destroy(struct token *tok)
{
    free(tok->conts);
}

void token_print(struct token const *tok)
{
    printf("token.type  = %s\n", token_type_names[tok->type]);
    printf("token.line  = %d\n", tok->line);
    printf("token.col   = %d\n", tok->col);
    printf("token.len   = %d\n", tok->len);
    printf("token.conts = \"%s\"\n", tok->conts);
}

struct token token_clone(struct token const *tok)
{
    struct token new_tok = {
        .type = tok->type,
        .line = tok->line,
        .col = tok->col,
        .len = tok->len,
        .conts = malloc(tok->len + 1),
    };

    memcpy(new_tok.conts, tok->conts, tok->len);
    new_tok.conts[tok->len] = '\0';
    
    return new_tok;
}

static void get_token_position(char const *src, size_t pos, size_t *out_line,
                               size_t *out_col)
{
    *out_line = *out_col = 1;
    
    for (size_t i = 0; i < pos; ++i) {
        ++*out_col;
        if (src[i] == '\n') {
            ++*out_line;
            *out_col = 1;
        }
    }
}

static struct token build_token(enum token_type type, char const *src,
                                size_t start, size_t end)
{
    size_t line, col;
    get_token_position(src, start, &line, &col);
    
    struct token tok = {
        .type = type,
        .line = line,
        .col = col,
        .len = end - start,
        .conts = malloc(end - start + 1),
    };

    memcpy(tok.conts, src + start, tok.len);
    tok.conts[tok.len] = '\0';
    
    return tok;
}

LEXER_RULE(lex_identifier)
{
    size_t end = start + 1;
    for (; end < src_len && (isalnum(src[end]) || src[end] == '_'); ++end);
    return build_token(TOKEN_TYPE_IDENTIFIER, src, start, end);
}

struct dynarr lex(char const *src, size_t src_len)
{
}
