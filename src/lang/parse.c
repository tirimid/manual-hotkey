#include "lang/parse.h"

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "util/log.h"
#include "util/dynstr.h"
#include "util/gu.h"

// use this macro to make it explicit that a function is a parser rule.
// to use it, simply place it as the return type of the relevant function.
#define PARSER_RULE static struct node

static char const *node_type_names[NODE_TYPE_LAST__] = {
    "root",
    "function",
    "function_arglist",
    "registry",
    "block",
    "variable",
    "expression",
    "expr_function_call",
    "expr_mhk_function_call",
    "expr_variable_lookup",
    "expr_unop_negate",
    "expr_unop_log_negate",
    "expr_binop_power",
    "expr_binop_multiply",
    "expr_binop_divide",
    "expr_binop_add",
    "expr_binop_subtract",
    "expr_binop_and",
    "expr_binop_or",
    "expr_binop_assign",
};

char const *node_type_name(enum node_type type)
{
    if (type < 0 || type >= NODE_TYPE_LAST__)
        ERROR_F("cannot get name of `node_type` with value %d!", type);

    return node_type_names[type];
}

struct node node_create(enum node_type type)
{
    return (struct node){
        .type = type,
        .children = dynarr_create(sizeof(struct node)),
        .toks = dynarr_create(sizeof(struct token)),
    };
}

void node_destroy(struct node *n)
{
    for (size_t i = 0; i < n->children.size; ++i) 
        node_destroy(dynarr_get_mut(&n->children, i));
    
    dynarr_destroy(&n->children);
    destroy_token_dynarr(&n->toks);
}

void node_add_child(struct node *n, struct node const *child)
{
    dynarr_push(&n->children, child);
}

void node_add_token(struct node *n, struct token const *tok)
{
    struct token clone = token_clone(tok);
    dynarr_push(&n->toks, &clone);
}

static void node_print_real(struct node const *n, size_t depth)
{
    for (size_t i = 0; i < depth; ++i)
        putc('\t', stdout);

    printf("%s%s [ ", depth >= 1 ? "| " : "", node_type_name(n->type));
    for (size_t i = 0; i < n->toks.size; ++i) {
        struct token const *tok = dynarr_get(&n->toks, i);
        printf("'%s' (%s) ", tok->conts, token_type_name(tok->type));
    }
    puts("]");

    for (size_t i = 0; i < n->children.size; ++i)
        node_print_real(dynarr_get(&n->children, i), depth + 1);
}

void node_print(struct node const *n)
{
    node_print_real(n, 0);
}

static struct token const *consume(struct dynarr const *toks, size_t *tok_ind)
{
    return dynarr_get(toks, ++*tok_ind);
}

static struct token const *expect_many(struct dynarr const *toks,
                                       size_t *tok_ind,
                                       enum token_type const expected[],
                                       size_t expected_size)
{
    struct token const *tok = dynarr_get(toks, ++*tok_ind);
    for (size_t i = 0; i < expected_size; ++i) {
        if (tok->type == expected[i])
            return tok;
    }

    // this very obviously leaks memory.
    // not an issue because OS should clean it up on program termination.
    struct dynstr expected_str = dynstr_create();
    dynstr_push_c_str(&expected_str, "[ ");
    
    for (size_t i = 0; i < expected_size; ++i) {
        dynstr_push_c_str(&expected_str, token_type_name(expected[i]));
        dynstr_push_char(&expected_str, ' ');
    }
    
    dynstr_push_c_str(&expected_str, "]");
    char *exs = dynstr_as_c_str(&expected_str);
    
    ERROR_F("(l=%d,c=%d) expected one of %s but found %s!", tok->line, tok->col,
            exs, token_type_name(tok->type));
}

static struct token const *expect(struct dynarr const *toks, size_t *tok_ind,
                                  enum token_type expected)
{
    struct token const *tok = dynarr_get(toks, ++*tok_ind);
    if (tok->type != expected) {
        ERROR_F("(l=%d,c=%d) expected %s but found %s!", tok->line, tok->col,
                token_type_name(expected), token_type_name(tok->type));
    }

    return tok;
}

static struct token const *peek(struct dynarr const *toks, size_t tok_ind)
{
    return dynarr_get(toks, tok_ind + 1);
}

static struct token const *current(struct dynarr const *toks, size_t tok_ind)
{
    return dynarr_get(toks, tok_ind);
}

// tokens:
// [n for all even n >= 0]: type of argument.
// [n for all odd n >= 0]:  name of argument.
PARSER_RULE parse_function_arglist(struct dynarr const *toks, size_t *tok_ind)
{
    struct node node = node_create(NODE_TYPE_FUNCTION_ARGLIST);

    while (true) {
        enum token_type exp0[] = {
            TOKEN_TYPE_KEYWORD_INT,
            TOKEN_TYPE_KEYWORD_FLOAT,
            TOKEN_TYPE_KEYWORD_CHAR,
            TOKEN_TYPE_KEYWORD_STRING,
            TOKEN_TYPE_KEYWORD_KEY,
            TOKEN_TYPE_KEYWORD_BOOL,
        };
        
        struct token const *type, *var_name;
        type = expect_many(toks, tok_ind, exp0, ARRAY_SIZE(exp0));
        var_name = expect(toks, tok_ind, TOKEN_TYPE_IDENTIFIER);

        node_add_token(&node, type);
        node_add_token(&node, var_name);

        enum token_type exp1[] = {
            TOKEN_TYPE_COMMA,
            TOKEN_TYPE_RIGHT_PAREN,
        };

        switch (expect_many(toks, tok_ind, exp1, ARRAY_SIZE(exp1))->type) {
        case TOKEN_TYPE_COMMA:
            continue;
        case TOKEN_TYPE_RIGHT_PAREN:
            return node;
        }
    }
}

// children:
// [*]: either a variable declaration, control flow statement, or expression.
PARSER_RULE parse_block(struct dynarr const *toks, size_t *tok_ind)
{
    struct node node = node_create(NODE_TYPE_BLOCK);

    while (true) {
        switch (consume(toks, tok_ind)->type) {
        case TOKEN_TYPE_LEFT_BRACE: {
            struct node sub_block = parse_block(toks, tok_ind);
            node_add_child(&node, &sub_block);
            break;
        }
        case TOKEN_TYPE_RIGHT_BRACE:
            return node;
        default:
            break;
        }
    }
}

// tokens:
// [0]:   name of function.
// ([1]): return type of function.
// children:
// ([0]): argument list.
// [1]:   code block.
PARSER_RULE parse_function(struct dynarr const *toks, size_t *tok_ind)
{
    struct node node = node_create(NODE_TYPE_FUNCTION);

    node_add_token(&node, expect(toks, tok_ind, TOKEN_TYPE_IDENTIFIER));
    if (peek(toks, *tok_ind)->type == TOKEN_TYPE_LEFT_PAREN) {
        consume(toks, tok_ind);
        struct node arglist = parse_function_arglist(toks, tok_ind);
        node_add_child(&node, &arglist);
    }

    if (peek(toks, *tok_ind)->type == TOKEN_TYPE_HYPHEN) {
        expect(toks, tok_ind, TOKEN_TYPE_HYPHEN);
        expect(toks, tok_ind, TOKEN_TYPE_RIGHT_ANGLE);
        
        enum token_type exp[] = {
            TOKEN_TYPE_KEYWORD_INT,
            TOKEN_TYPE_KEYWORD_FLOAT,
            TOKEN_TYPE_KEYWORD_CHAR,
            TOKEN_TYPE_KEYWORD_STRING,
            TOKEN_TYPE_KEYWORD_KEY,
            TOKEN_TYPE_KEYWORD_BOOL,
        };
        
        node_add_token(&node, expect_many(toks, tok_ind, exp, ARRAY_SIZE(exp)));
    }

    expect(toks, tok_ind, TOKEN_TYPE_LEFT_BRACE);
    struct node block = parse_block(toks, tok_ind);
    node_add_child(&node, &block);
    
    return node;
}

// tokens:
// [0]: registry event.
// [1]: name of function being registered.
PARSER_RULE parse_registry(struct dynarr const *toks, size_t *tok_ind)
{
    struct node node = node_create(NODE_TYPE_REGISTRY);

    enum token_type exp[] = {
        TOKEN_TYPE_KEYWORD_KEY_PRESS,
        TOKEN_TYPE_KEYWORD_KEY_RELEASE,
        TOKEN_TYPE_KEYWORD_INIT,
        TOKEN_TYPE_KEYWORD_QUIT,
    };

    node_add_token(&node, expect_many(toks, tok_ind, exp, ARRAY_SIZE(exp)));
    node_add_token(&node, expect(toks, tok_ind, TOKEN_TYPE_IDENTIFIER));
    expect(toks, tok_ind, TOKEN_TYPE_LINEFEED);
    
    return node;
}

struct node parse(struct dynarr const *toks)
{
    struct node ast = node_create(NODE_TYPE_ROOT);
    size_t tok_ind = 0;

    while (true) {
        enum token_type exp[] = {
            TOKEN_TYPE_EOF,
            TOKEN_TYPE_LINEFEED,
            TOKEN_TYPE_KEYWORD_DEFUN,
            TOKEN_TYPE_KEYWORD_REGISTER,
        };

        size_t exp_size = ARRAY_SIZE(exp);
        struct token const *tok = expect_many(toks, &tok_ind, exp, exp_size);
        struct node new_node;

        switch (tok->type) {
        case TOKEN_TYPE_EOF:
            goto exit;
        case TOKEN_TYPE_LINEFEED:
            break;
        case TOKEN_TYPE_KEYWORD_DEFUN:
            new_node = parse_function(toks, &tok_ind);
            node_add_child(&ast, &new_node);
            break;
        case TOKEN_TYPE_KEYWORD_REGISTER:
            new_node = parse_registry(toks, &tok_ind);
            node_add_child(&ast, &new_node);
            break;
        }
    }

exit:
    return ast;
}
