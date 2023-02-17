#include <stdbool.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

#include "input/keys.h"
#include "input/map.h"
#include "util/log.h"

static bool running = true;

static void backspace_handler(void)
{
    if (keys_is_pressed(KEY_LEFTCTRL))
        running = false;
}

int main(void)
{
    log_init();
    
    if (getuid() != 0)
        ERROR("you must run the program as root!");

    map_init();
    keys_init();

    map_load_scancode_map("scancode-maps/colemak");
    keys_type_string("hello world, this is very cool i think");
    
    keys_quit();
    map_quit();
    return 0;
}
