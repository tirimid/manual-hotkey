#include "lang/lex.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "util/log.h"

#define KEYWORD_COUNT (TOKEN_TYPE_KEYWORD_LAST__ - TOKEN_TYPE_KEYWORD_FIRST__)
#define SPECIAL_COUNT (TOKEN_TYPE_LAST__ - TOKEN_TYPE_SPECIAL_FIRST__ + 1)

// use this macro to make it explicit that a function is a lexer rule.
// to use it, simply place it as the return type of the relevant function.
#define LEXER_RULE static struct token

static char const specials[SPECIAL_COUNT] = "`~!@#$%^&*-=+;:|,./?\n()[]{}<>\0";

static char const *token_type_names[TOKEN_TYPE_LAST__] = {
    "sof",
    "eof",
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
    "if",
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
    "right_brace",
    "left_angle",
    "right_angle",
};

static char const *keywords[KEYWORD_COUNT] = {
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
    "if",
};

char const *token_type_name(enum token_type type)
{
    if (type < 0 || type >= TOKEN_TYPE_LAST__)
        ERROR_F("cannot get name of `token_type` with value %d!", type);

    return token_type_names[type];
}

void token_destroy(struct token *tok)
{
    free(tok->conts);
}

void token_print(struct token const *tok)
{
    printf("token.type  = %s\n", token_type_name(tok->type));
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

void print_token_dynarr(struct dynarr const *tl)
{
    for (size_t i = 0; i < tl->size; ++i)
        token_print(dynarr_get(tl, i));
}

void destroy_token_dynarr(struct dynarr *tl)
{
    for (size_t i = 0; i < tl->size; ++i)
        token_destroy(dynarr_get_mut(tl, i));

    dynarr_destroy(tl);
}

LEXER_RULE lex_identifier(char const *src, size_t src_len, size_t start)
{
    size_t end = start + 1;
    for (; end < src_len && (isalnum(src[end]) || src[end] == '_'); ++end);
    return build_token(TOKEN_TYPE_IDENTIFIER, src, start, end);
}

LEXER_RULE lex_string_literal(char const *src, size_t src_len, size_t start)
{
    size_t end = start + 1;
    for (; end < src_len && (src[end] != '"' || src[end - 1] == '\\'); ++end);
    return build_token(TOKEN_TYPE_STRING_LITERAL, src, start, end + 1);
}

LEXER_RULE lex_char_literal(char const *src, size_t src_len, size_t start)
{
    size_t end = start + 1;
    for (; end < src_len && (src[end] != '\'' || src[end - 1] == '\\'); ++end);
    return build_token(TOKEN_TYPE_CHAR_LITERAL, src, start, end + 1);
}

LEXER_RULE lex_number_literal(char const *src, size_t src_len, size_t start)
{
    size_t end = start + 1;
    for (; end < src_len && (isdigit(src[end]) || src[end] == '.'); ++end);
    
    enum token_type type = TOKEN_TYPE_FLOAT_LITERAL;

    // if the next decimal point is either outside of the literal or does not
    // exist; then this is an integer literal, not a float.
    char const *dp_ptr = strchr(src + start, '.');
    size_t dp_offset = dp_ptr - src;
    if (dp_ptr == NULL || dp_offset > end - start)
        type = TOKEN_TYPE_INTEGER_LITERAL;
    
    return build_token(type, src, start, end);
}

LEXER_RULE lex_special_char(char const *src, size_t src_len, size_t start)
{
    size_t end = start + 1;
    size_t special_ind = strchr(specials, src[start]) - specials;
    enum token_type type = TOKEN_TYPE_SPECIAL_FIRST__ + special_ind;
    return build_token(type, src, start, end);
}

static enum token_type disambiguate_identifier(char const *tok_conts)
{
    for (size_t i = 0; i < KEYWORD_COUNT; ++i) {
        if (strcmp(tok_conts, keywords[i]) == 0)
            return TOKEN_TYPE_KEYWORD_FIRST__ + i;
    }

    return TOKEN_TYPE_IDENTIFIER;
}

struct dynarr lex(char const *src, size_t src_len)
{
    struct dynarr tl = dynarr_create(sizeof(struct token));

    // add SOF.
    struct token seek = {
        .type = TOKEN_TYPE_SOF,
        .line = 0,
        .col = 0,
        .len = 0,
        .conts = NULL,
    };
    dynarr_push(&tl, &seek);
    
    // first pass: get source into tokens.
    size_t i = 0;
    while (i < src_len) {
        struct token new_tok;
        
        if (src[i] == '"')
            new_tok = lex_string_literal(src, src_len, i);
        else if (src[i] == '\'')
            new_tok = lex_char_literal(src, src_len, i);
        else if (isdigit(src[i]))
            new_tok = lex_number_literal(src, src_len, i);
        else if (isalpha(src[i]) || src[i] == '_')
            new_tok = lex_identifier(src, src_len, i);
        else if (strchr(specials, src[i]) != NULL)
            new_tok = lex_special_char(src, src_len, i);
        else if (isspace(src[i])) {
            ++i;
            continue;
        } else {
            size_t line, col;
            get_token_position(src, i, &line, &col);
            ERROR_F("(l=%d,c=%d) unknown token!", line, col);
        }

        i += new_tok.len;
        dynarr_push(&tl, &new_tok);
    }

    // second pass: disambiguate keywords.
    for (i = 0; i < tl.size; ++i) {
        struct token *tok = dynarr_get_mut(&tl, i);
        if (tok->type == TOKEN_TYPE_IDENTIFIER)
            tok->type = disambiguate_identifier(tok->conts);
    }

    // add EOF.
    seek = (struct token){
        .type = TOKEN_TYPE_EOF,
        .line = 0,
        .col = 0,
        .len = 0,
        .conts = NULL,
    };
    dynarr_push(&tl, &seek);

    return tl;
}
