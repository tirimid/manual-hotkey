#ifndef PARSE_H__
#define PARSE_H__

#include "lang/lex.h"
#include "util/dynarr.h"

enum node_type {
    // symbolizes the root node of an AST.
    // this type of node is returned from `parse()`.
    NODE_TYPE_ROOT,
    
    NODE_TYPE_FUNCTION,
    NODE_TYPE_REGISTRY,
    NODE_TYPE_BLOCK,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_EXPRESSION,

    // expression node types.
    NODE_TYPE_EXPR_FUNCTION_CALL,
    NODE_TYPE_EXPR_VARIABLE_LOOKUP,
    NODE_TYPE_EXPR_UNOP_NEGATE,
    NODE_TYPE_EXPR_UNOP_LOG_NEGATE,
    NODE_TYPE_EXPR_BINOP_POWER,
    NODE_TYPE_EXPR_BINOP_MULTIPLY,
    NODE_TYPE_EXPR_BINOP_DIVIDE,
    NODE_TYPE_EXPR_BINOP_ADD,
    NODE_TYPE_EXPR_BINOP_SUBTRACT,
    NODE_TYPE_EXPR_BINOP_AND,
    NODE_TYPE_EXPR_BINOP_OR,
    NODE_TYPE_EXPR_BINOP_EQUAL,
    NODE_TYPE_EXPR_BINOP_NOT_EQUAL,
    NODE_TYPE_EXPR_BINOP_GREATER,
    NODE_TYPE_EXPR_BINOP_GREATER_EQUAL,
    NODE_TYPE_EXPR_BINOP_LESS,
    NODE_TYPE_EXPR_BINOP_LESS_EQUAL,
    NODE_TYPE_EXPR_BINOP_ASSIGN,

    NODE_TYPE_LAST__, // see `TOKEN_TYPE_LAST__` in `lang/lex.h`.
};

struct node {
    enum node_type type;
    struct dynarr children, toks;
};

struct node node_create(enum node_type type);
void node_destroy(struct node *n);
void node_add_child(struct node *n, struct node const *child);
void node_add_token(struct node *n, struct token const *tok);
void node_print(struct node const *n);

struct node parse(struct dynarr const *toks);

#endif
