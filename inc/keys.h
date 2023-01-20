#ifndef KEYS_H__
#define KEYS_H__

#include <stdint.h>
#include <stdbool.h>

void keys_init(void);
void keys_quit(void);
bool keys_is_initialized(void);

bool keys_is_pressed(uint16_t code);
void keys_set_press_handler(uint16_t code, void (*handler)(void));
void keys_set_release_handler(uint16_t code, void (*handler)(void));

#endif
