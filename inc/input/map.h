#ifndef MAP_H__
#define MAP_H__

#include <stdbool.h>
#include <stdint.h>

#include "util/dynarr.h"

#define MAP_INVALID 0xffff

void map_init(void);
void map_quit(void);

void map_load_scancode_map(char const *path);
char map_scancode(uint16_t code);
void map_print_mapped_scancodes(void);

// `MAP_INVALID` returned if char not mapped.
// if multiple scancodes are mapped to `ch`, the lowest valued one is returned.
uint16_t map_char(char ch);

// returns a dynamic array containing all scancodes mapped to `ch`.
// returned `dynarr` has a `stride` of `sizeof(uint16_t)`.
// remember to call `dynarr_destroy()` on returned resource.
struct dynarr map_char_all(char ch);

#endif
