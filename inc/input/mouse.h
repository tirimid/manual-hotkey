#ifndef MOUSE_H__
#define MOUSE_H__

#include <stdint.h>
#include <stdbool.h>

enum mouse_btn {
    MOUSE_BTN_LEFT,
    MOUSE_BTN_RIGHT,
    MOUSE_BTN_MIDDLE,
};

void mouse_init(void);
void mouse_quit(void);

bool mouse_is_pressed(enum mouse_btn btn);
void mouse_set_press_handler(enum mouse_btn btn, void (*handler)(void));
void mouse_set_release_handler(enum mouse_btn btn, void (*handler)(void));
void mouse_position(uint16_t *out_x, uint16_t *out_y);

void mouse_press(enum mouse_btn btn);
void mouse_release(enum mouse_btn btn);
void mouse_click(enum mouse_btn btn);
void mouse_move(uint16_t x, uint16_t y);
void mouse_move_rel(uint16_t x, uint16_t y);

#endif
