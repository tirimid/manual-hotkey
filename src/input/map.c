#include "input/map.h"

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>

#include "util/log.h"

#define SC_MAP_BUF_SIZE (UINT16_MAX + 1)
#define DEF_SEQ "<->"

static char *sc_map;

void map_init(void)
{
    sc_map = malloc(SC_MAP_BUF_SIZE);
    memset(sc_map, '\0', SC_MAP_BUF_SIZE);
}

void map_quit(void)
{
    free(sc_map);
}

void map_load_scancode_map(char const *path)
{
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *fconts = malloc(fsize);
    fread(fconts, 1, fsize, fp);
    fclose(fp);

    for (size_t i = 0; i < fsize / 7; ++i) {
        char ch = fconts[7 * i], sc_buf[5];
        memcpy(sc_buf, fconts + 7 * i + 1, 5);
        uint16_t sc = atoi(sc_buf) & 0xffff;
        sc_map[sc] = ch;
    }

    free(fconts);
}

char map_scancode(uint16_t code)
{
    return sc_map[code];
}

void map_print_mapped_scancodes(void)
{
    for (size_t i = 0; i < SC_MAP_BUF_SIZE; ++i) {
        if (sc_map[i] != '\0')
            printf("sc=%d(d),%x(x);ch=%c\n", i, i, sc_map[i]);
    }
}

uint16_t map_char(char ch)
{
    for (uint16_t i = 0; i < SC_MAP_BUF_SIZE; ++i) {
        if (sc_map[i] == ch)
            return i;
    }
    
    return MAP_INVALID;
}

struct dynarr map_char_all(char ch)
{
    struct dynarr da = dynarr_create(sizeof(uint16_t));
    
    for (size_t i = 0; i < SC_MAP_BUF_SIZE; ++i) {
        if (sc_map[i] == ch) {
            uint16_t u16 = i;
            dynarr_push(&da, &u16);
        }
    }

    return da;
}
