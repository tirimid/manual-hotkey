#include "input/mouse.h"

#include <stddef.h>

#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#include "util/log.h"

#define MICE_FILE "/dev/input/mice"

static uint8_t btn_data = 0;
static int64_t mouse_pos[2] = {0, 0};
static void (*press_handlers[3])(void), (*release_handlers[3])(void);
static void (*move_handler)(void), (*rel_move_handler)(void);
static int mouse_fd;
static pthread_t mouse_thread;

static void main_loop(void *args)
{
    for (;;) {
        uint8_t data[3];
        read(mouse_fd, data, sizeof(data));

        LOG_F("%x,%x,%x", data[0], data[1], data[2]);
    }
}

void mouse_init(void)
{
    if ((mouse_fd = open(MICE_FILE, O_RDWR)) == -1)
        ERROR("failed to get mice file descriptor!");

    void *(*ml)(void *) = (void *(*)(void *))main_loop;
    int rc = pthread_create(&mouse_thread, NULL, ml, NULL);
    if (rc != 0)
        ERROR_F("failed to create mouse thread! error %d", rc);
}

void mouse_quit(void)
{
    int rc = pthread_cancel(mouse_thread);
    if (rc != 0)
        ERROR_F("failed to cancel mouse thread! error %d", rc);
    
    if ((rc = pthread_join(mouse_thread, NULL)) != 0)
        ERROR_F("failed to join mouse thread! error %d", rc);
    
    close(mouse_fd);
}

bool mouse_is_pressed(enum mouse_btn btn)
{
}

void mouse_set_press_handler(enum mouse_btn btn, void (*handler)(void))
{
}

void mouse_set_release_handler(enum mouse_btn btn, void (*handler)(void))
{
}

void mouse_set_move_handler(void (*handler)(void))
{
}

void mouse_set_rel_move_handler(void (*handler)(void))
{
}

void mouse_position(uint16_t *out_x, uint16_t *out_y)
{
}

void mouse_press(enum mouse_btn btn)
{
}

void mouse_release(enum mouse_btn btn)
{
}

void mouse_click(enum mouse_btn btn)
{
}

void mouse_move(uint16_t x, uint16_t y)
{
}

void mouse_move_rel(uint16_t x, uint16_t y)
{
}
