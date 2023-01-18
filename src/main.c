#include <unistd.h>

#include "keys.h"

int main(void)
{
    int kbd_fd = open_key_input();
    close(kbd_fd);
    return 0;
}
