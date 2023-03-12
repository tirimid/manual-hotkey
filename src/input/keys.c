#include "input/keys.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <pthread.h>

#include "util/dynstr.h"
#include "util/log.h"
#include "input/map.h"

#define DEVICES_FILE "/proc/bus/input/devices"
#define DEVICES_BUF_SIZE 4096
#define INPUT_DIR "/dev/input/"
#define DEVICE_EV "120013"
#define KEY_DATA_BUF_SIZE ((UINT16_MAX + 1) / 8)
#define KEY_HANDLER_BUF_SIZE ((UINT16_MAX + 1) * sizeof(void (*)(void)))

static uint8_t *key_data;
static void (**press_handlers)(void), (**release_handlers)(void);
static int key_fd;
static pthread_t keys_thread;

static int open_key_input(int flags)
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
    evf[event_file.len - 1] = '\0'; // this is necessary for some reason.
    rc = open(evf, flags);
    
    free(evf);
    dynstr_destroy(&event_file);
    
exit:
    close(devs_fd);
    if (devs_conts != NULL)
        free(devs_conts);

    return rc;
}

static void main_loop(void *args)
{
    for (;;) {
        struct input_event ie;
        read(key_fd, &ie, sizeof(ie));
        
        if (ie.type != EV_KEY)
            continue;
        
        size_t byte = ie.code / 8, bit = ie.code % 8;
        uint8_t *datum = &key_data[byte];
        *datum &= ~(1 << bit);
        *datum |= (ie.value > 0) << bit;

        if (ie.value == 0 && release_handlers[ie.code] != NULL)
            release_handlers[ie.code]();
        else if (ie.value == 1 && press_handlers[ie.code] != NULL)
            press_handlers[ie.code]();
    }
}

void keys_init(void)
{
    if ((key_fd = open_key_input(O_RDWR)) == -1)
        ERROR("failed to get keyboard event file descriptor!");

    key_data = malloc(KEY_DATA_BUF_SIZE);
    press_handlers = malloc(KEY_HANDLER_BUF_SIZE);
    release_handlers = malloc(KEY_HANDLER_BUF_SIZE);
    memset(key_data, 0, KEY_DATA_BUF_SIZE);
    memset(press_handlers, 0, KEY_HANDLER_BUF_SIZE);
    memset(release_handlers, 0, KEY_HANDLER_BUF_SIZE);

    void *(*ml)(void *) = (void *(*)(void *))main_loop;
    int rc = pthread_create(&keys_thread, NULL, ml, NULL);
    if (rc != 0)
        ERROR_F("failed to create keys thread! error %d", rc);
}

void keys_quit(void)
{
    int rc = pthread_cancel(keys_thread);
    if (rc != 0)
        ERROR_F("failed to cancel keys thread! error %d", rc);
    
    if ((rc = pthread_join(keys_thread, NULL)) != 0)
        ERROR_F("failed to join keys thread! error %d", rc);
    
    free(key_data);
    free(press_handlers);
    free(release_handlers);
    close(key_fd);
}

bool keys_is_pressed(uint16_t code)
{
    size_t byte = code / 8, bit = code % 8;
    uint8_t datum = key_data[byte];
    return (datum & 1 << bit) > 0;
}

void keys_set_press_handler(uint16_t code, void (*handler)(void))
{
    press_handlers[code] = handler;
}

void keys_set_release_handler(uint16_t code, void (*handler)(void))
{
    release_handlers[code] = handler;
}

void keys_press(uint16_t code)
{
    struct input_event ie = {
        .code = code,
        .type = EV_KEY,
        .value = 1,
    };

    write(key_fd, &ie, sizeof(ie));
}

void keys_release(uint16_t code)
{
    struct input_event ie = {
        .code = code,
        .type = EV_KEY,
        .value = 0,
    };

    write(key_fd, &ie, sizeof(ie));
}

void keys_type_char(char ch)
{
    uint16_t code = map_char(ch);
    if (code == MAP_INVALID) {
        WARNING_F("cannot type character: '%c' / %d(d),%x(x)!", ch, code, code);
        return;
    }

    keys_press(code);
    keys_release(code);
}

void keys_type_string(char const *str)
{
    for (size_t i = 0; i < strlen(str); ++i)
        keys_type_char(str[i]);
}
