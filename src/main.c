#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <unistd.h>
#include <linux/input.h>

#include "input/keys.h"
#include "input/map.h"
#include "input/mouse.h"
#include "util/log.h"
#include "lang/pp.h"
#include "lang/lex.h"
#include "lang/parse.h"

int main(int argc, char const *argv[])
{
    log_init();

    map_init();
    keys_init();
    mouse_init();
    
    if (getuid() != 0)
        ERROR("you must run the program as root!");

    // get arguments.
    if (argc != 2)
        ERROR("usage: manual-hotkey <script file>");

    // read script file.
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL)
        ERROR_F("invalid file provided: '%s'!", argv[1]);

    fseek(fp, 0, SEEK_END);
    size_t file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *file_conts = malloc(file_len);
    fread(file_conts, file_len, 1, fp);
    fclose(fp);

    // parse script.
    struct dynstr src = preprocess(file_conts, file_len);
    free(file_conts);

    struct dynarr toks = lex(src.data, src.len);
    dynstr_destroy(&src);

    struct node ast = parse(&toks);
    destroy_token_dynarr(&toks);
    node_print(&ast);

    node_destroy(&ast);

    // run the program.
    // ...
    
    mouse_quit();
    keys_quit();
    map_quit();
    
    return 0;
}
