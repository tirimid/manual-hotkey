#include "keys.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "dynstr.h"

#define DEVICES_FILE "/proc/bus/input/devices"
#define DEVICES_BUF_SIZE 4096
#define INPUT_DIR "/dev/input/"
#define DEVICE_EV "120013"

int open_key_input(void)
{
    char *devs_conts = NULL;
    int devs_fd = open(DEVICES_FILE, O_RDONLY), rc = -1;

    if (devs_fd == -1)
        goto exit;

    devs_conts = malloc(DEVICES_BUF_SIZE);
    devs_conts[DEVICES_BUF_SIZE - 1] = '\0';
    if (read(devs_fd, devs_conts, DEVICES_BUF_SIZE - 1) == -1)
        goto exit;

    char *ev = devs_conts, *handlers;
    do {
        handlers = strstr(ev, "Handlers=");
        ev = strstr(handlers, "EV=");
        if (strncmp(ev + 3, DEVICE_EV, strlen(DEVICE_EV)) == 0)
            break;
    } while (handlers != NULL && ev != NULL);

    if (handlers == NULL || ev == NULL)
        goto exit;
    
    char *event = strstr(handlers, "event");
    if (event == NULL)
        goto exit;

    struct dynstr event_file = dynstr_create();
    dynstr_push_c_str(&event_file, INPUT_DIR);
    dynstr_push_c_str_not_nt(&event_file, event, '\n');

    char *evf = dynstr_as_c_str(&event_file);
    rc = open(evf, O_RDWR);
    
    free(evf);
    dynstr_destroy(&event_file);
    
exit:
    close(devs_fd);
    if (devs_conts != NULL)
        free(devs_conts);

    return rc;
}
