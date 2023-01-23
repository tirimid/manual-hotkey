#include <stdbool.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

#include "input/keys.h"
#include "util/log.h"

static bool running = true;

static void bskh(void)
{
    if (keys_is_pressed(KEY_LEFTCTRL))
        running = false;
    else if (keys_is_pressed(KEY_LEFTSHIFT)) {
        keys_press(KEY_H);
        keys_release(KEY_H);
    }
}

int main(void)
{
    log_init();
    
    if (getuid() != 0)
        ERROR("you must run the program as root!");

    keys_init();
    keys_set_press_handler(KEY_BACKSPACE, bskh);
    while (running);
    keys_quit();
    return 0;
}
