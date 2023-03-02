#include "lang/parse.h"

#include <stddef.h>
#include <stdio.h>

static char const *node_type_names[NODE_TYPE_LAST__] = {
    "root",
    "function",
    "registry",
    "block",
    "variable",
    "expression",
    "expr_function_call",
    "expr_variable_lookup",
    "expr_unop_negate",
    "expr_binop_power",
    "expr_binop_multiply",
    "expr_binop_divide",
    "expr_binop_add",
    "expr_binop_subtract",
    "expr_binop_and",
    "expr_binop_or",
    "expr_binop_assign",
};

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
    dynarr_push(&n->toks, tok);
}

static void node_print_real(struct node const *n, size_t depth)
{
    for (size_t i = 0; i < depth; ++i)
        putc('\t', stdout);

    printf("%s%s [ ", depth >= 1 ? "| " : "", node_type_names[n->type]);
    for (size_t i = 0; i < n->toks.size; ++i) {
        struct token const *tok = dynarr_get(&n->toks, i);
        printf("'%s' (%d) ", tok->conts, tok->type);
    }
    puts("]\n");

    for (size_t i = 0; i < n->children.size; ++i)
        node_print_real(dynarr_get(&n->children, i), depth + 1);
}

void node_print(struct node const *n)
{
    node_print_real(n, 0);
}

struct node parse(struct dynarr const *toks)
{
    return node_create(NODE_TYPE_ROOT);
}
