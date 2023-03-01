#ifndef LEX_H__
#define LEX_H__

#include <stddef.h>

#include "util/dynarr.h"

enum token_type {
    TOKEN_TYPE_IDENTIFIER = 0,

    // literal types.
    TOKEN_TYPE_INTEGER_LITERAL,
    TOKEN_TYPE_FLOAT_LITERAL,
    TOKEN_TYPE_STRING_LITERAL,
    TOKEN_TYPE_CHAR_LITERAL,

    TOKEN_TYPE_KEYWORD_FIRST__, // see `TOKEN_TYPE_LAST__`.

    // keywords.
    TOKEN_TYPE_KEYWORD_INT = TOKEN_TYPE_KEYWORD_FIRST__,
    TOKEN_TYPE_KEYWORD_FLOAT,
    TOKEN_TYPE_KEYWORD_CHAR,
    TOKEN_TYPE_KEYWORD_STRING,
    TOKEN_TYPE_KEYWORD_KEY,
    TOKEN_TYPE_KEYWORD_BOOL,
    TOKEN_TYPE_KEYWORD_DEFUN,
    TOKEN_TYPE_KEYWORD_RETURN,
    TOKEN_TYPE_KEYWORD_MHK,
    TOKEN_TYPE_KEYWORD_MHKC,
    TOKEN_TYPE_KEYWORD_REGISTER,
    TOKEN_TYPE_KEYWORD_KEY_PRESS,
    TOKEN_TYPE_KEYWORD_KEY_RELEASE,
    TOKEN_TYPE_KEYWORD_INIT,
    TOKEN_TYPE_KEYWORD_QUIT,
    TOKEN_TYPE_KEYWORD_IF,

    // see `TOKEN_TYPE_LAST__`.
    TOKEN_TYPE_KEYWORD_LAST__,
    TOKEN_TYPE_SPECIAL_FIRST__ = TOKEN_TYPE_KEYWORD_LAST__,

    // unpaired special characters.
    TOKEN_TYPE_BACKTICK = TOKEN_TYPE_SPECIAL_FIRST__,
    TOKEN_TYPE_TILDE,
    TOKEN_TYPE_EXCLAMATION,
    TOKEN_TYPE_AT,
    TOKEN_TYPE_OCTOTHORPE,
    TOKEN_TYPE_DOLLAR,
    TOKEN_TYPE_PERCENT,
    TOKEN_TYPE_CARET,
    TOKEN_TYPE_AMPERSAND,
    TOKEN_TYPE_ASTERISK,
    TOKEN_TYPE_HYPHEN,
    TOKEN_TYPE_EQUALS,
    TOKEN_TYPE_PLUS,
    TOKEN_TYPE_SEMICOLON,
    TOKEN_TYPE_COLON,
    TOKEN_TYPE_PIPE,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_PERIOD,
    TOKEN_TYPE_SLASH,
    TOKEN_TYPE_QUESTION,
    TOKEN_TYPE_LINEFEED,

    // paired special characters.
    TOKEN_TYPE_LEFT_PAREN,
    TOKEN_TYPE_RIGHT_PAREN,
    TOKEN_TYPE_LEFT_BRACKET,
    TOKEN_TYPE_RIGHT_BRACKET,
    TOKEN_TYPE_LEFT_BRACE,
    TOKEN_TYPE_RIGHT_BRACE,
    TOKEN_TYPE_LEFT_ANGLE,
    TOKEN_TYPE_RIGHT_ANGLE,

    // this must be the last token.
    // the idea here is to stop human error when adding new token types, since
    // having a way to enforce the length of `token_type_names` will allow the
    // compiler to give an error upon not implementing a token type fully.
    TOKEN_TYPE_LAST__,
};

struct token {
    enum token_type type;
    size_t line, col, len;
    char *conts;
};

void token_destroy(struct token *tok);
void token_print(struct token const *tok);
struct token token_clone(struct token const *tok); // makes deep copy of `tok`.

void print_token_dynarr(struct dynarr const *tl);
void destroy_token_dynarr(struct dynarr *tl);

// returns a `dynarr` of `token`.
// assign output of `lex()` to a variable of type `token_list` for convenience.
// find `token_list` defined in `lang/toklist.h`.
struct dynarr lex(char const *src, size_t src_len);

#endif
