/*
 * rtkgpio.c
 *
 *  Created on: Jan 7, 2021
 *      Author: de23a4
 */

#include "rtkgpio/rtkgpio.h"
#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const char CH_GPIO_MODE_INPUT = 'I';
const char CH_GPIO_MODE_OUTPUT = 'O';

const char CH_GPIO_READ = '?';
const char CH_GPIO_ANALOG = 'A';
const char CH_GPIO_VALUE_HIGH = '1';
const char CH_GPIO_VALUE_LOW = '0';

const char CH_GPIO_PULL_DOWN = 'D';
const char CH_GPIO_PULL_UP = 'U';
const char CH_GPIO_PULL_NONE = 'N';

const int BOARDPINS[] = {-1, // 0
                         -1, // 1
                         -1, // 2
                         2, // 3
                         -1, // 4
                         3, // 5
                         -1, // 6,
                         4, // 7
                         14, // 8
                         -1, // 9
                         15, // 10
                         17, // 11
                         18, // 12
                         27, //13
                         -1, // 14
                         22, //15
                         23, //16
                         -1, //17
                         24, //18
                         10, //19
                         -1, // 20
                         9, //21
                         25, //22
                         11, //23
                         8, //24
                         -1, //25
                         7, //26
                         0, //27
                         1, //28
                         5, //29
                         -1, //30
                         6, //31
                         12, //32
                         13, //33
                         -1, //34
                         19, //35
                         16, //36
                         26, //37
                         20, //38
                         -1, //39
                         21 //40
};

struct rtkgpio {
    struct sp_port *port;
    int boardmode;
};

static char *find_port_name(void);

static int resolve_pin(struct rtkgpio *rtkgpio, int pin);

static int getc_pin(int pin, char *out);

static int getc_value(enum rtkgpio_value value, char *out);

static int getc_mode(enum rtkgpio_mode mode, char *out);

static int getc_pud(enum rtkgpio_pud pud, char *out);

static int parse_value(char ch, enum rtkgpio_value *value);

static int serial_read(struct rtkgpio *rtkgpio, void *buffer, size_t maxsize,
                       int minsize, const char *terminator);

static int serial_write(struct rtkgpio *rtkgpio, const void *buf, size_t count);

int resolve_pin(struct rtkgpio *rtkgpio, int pin) {
    if (rtkgpio->boardmode) {
        if (pin < 0) {
            return -1;
        }
        const size_t idx = (size_t) pin;
        if (idx < sizeof(BOARDPINS) / sizeof(BOARDPINS[0])) {
            return BOARDPINS[idx];
        } else {
            return -1;
        }
    } else {
        if (pin >= 0 && pin < 27) {
            return pin;
        } else {
            return -1;
        }
    }
}

int getc_pin(int pin, char *out) {
    *out = (char) ('a' + pin);
    return 0;
}

int getc_value(enum rtkgpio_value value, char *out) {
    switch (value) {
        case RTKGPIO_VALUE_LOW:
            *out = CH_GPIO_VALUE_LOW;
            return 0;
        case RTKGPIO_VALUE_HIGH:
            *out = CH_GPIO_VALUE_HIGH;
            return 0;
        default:
            return -1;
    }
}

int getc_mode(enum rtkgpio_mode mode, char *out) {
    switch (mode) {
        case RTKGPIO_MODE_INPUT:
            *out = CH_GPIO_MODE_INPUT;
            return 0;
        case RTKGPIO_MODE_OUTPUT:
            *out = CH_GPIO_MODE_OUTPUT;
            return 0;
        default:
            return -1;
    }
}

int getc_pud(enum rtkgpio_pud pud, char *out) {
    switch (pud) {
        case RTKGPIO_PUD_DOWN:
            *out = CH_GPIO_PULL_DOWN;
            return 0;
        case RTKGPIO_PUD_UP:
            *out = CH_GPIO_PULL_UP;
            return 0;
        case RTKGPIO_PUD_OFF:
            *out = CH_GPIO_PULL_NONE;
            return 0;
        default:
            return -1;
    }
}

int parse_value(char ch, enum rtkgpio_value *value) {
    if (ch == CH_GPIO_VALUE_LOW) {
        *value = RTKGPIO_VALUE_LOW;
        return 0;
    } else if (ch == CH_GPIO_VALUE_HIGH) {
        *value = RTKGPIO_VALUE_HIGH;
        return 0;
    } else {
        return -1;
    }
}

int serial_read(struct rtkgpio *rtkgpio, void *buffer, size_t maxsize,
                int minsize, const char *terminator) {
    if (maxsize > INT_MAX) {
        fprintf(stderr, "maxsize exceed limit");
        return -1;
    }
    const int int_maxsize = (int) maxsize;

    char *buff = (char *) buffer;

    if (minsize == -1) {
        minsize = int_maxsize;
    }

    const size_t readsz = terminator == NULL ? maxsize : 1;
    int used = 0;

    while (used < minsize) {
        int rv = sp_blocking_read_next(rtkgpio->port, buff + used, readsz, 10000);
        if (rv > 0) {
            used += rv;
            if (terminator != NULL) {
                if (buff[used - 1] == *terminator) {
                    break;
                }
            }
        } else if (rv < 0) {
            fprintf(stderr, "something went wrong on read.\n");
            return -1;
        } else {
            fprintf(stderr, "timeout on read.\n");
            return -1;
        }
    }
    return used;
}

int serial_write(struct rtkgpio *rtkgpio, const void *buf, size_t count) {
    const int rv = sp_blocking_write(rtkgpio->port, buf, count, 10000);
    if (rv < 0) {
        fprintf(stderr, "something went wrong on write.\n");
        return -1;
    }
    const size_t strv = (size_t) rv;
    if (strv == count) {
        return (int) count;
    } else {
        fprintf(stderr, "timeout on write.\n");
        return -1;
    }
}

char *find_port_name(void) {
    char *rtkgpio_port_name = NULL;
    struct sp_port **port_list;
    enum sp_return result = sp_list_ports(&port_list);
    if (result != SP_OK) {
        return NULL;
    }
    int i;
    for (i = 0; rtkgpio_port_name == NULL && port_list[i] != NULL; i++) {
        struct sp_port *port = port_list[i];
        const char *port_name = sp_get_port_name(port);
        if (NULL == port_name) {
            continue;
        }
        if (SP_TRANSPORT_USB != sp_get_port_transport(port)) {
            continue;
        }

        int usb_vid, usb_pid;
        if (SP_OK != sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid)) {
            continue;
        }
        if (usb_vid != 0x1A86 || usb_pid != 0x7523) {
            continue;
        }
        rtkgpio_port_name = strdup(port_name);
    }
    sp_free_port_list(port_list);
    return rtkgpio_port_name;
}

int rtkgpio_create(const char *port_name, struct rtkgpio **rtkgpio) {
    *rtkgpio = malloc(sizeof(struct rtkgpio));
    if (NULL == port_name) {
        char *port_name_found = find_port_name();
        if (NULL != port_name_found) {
            sp_get_port_by_name(port_name_found, &(*rtkgpio)->port);
        }
        free(port_name_found);
    } else {
        sp_get_port_by_name(port_name, &(*rtkgpio)->port);
    }
    if (NULL == (*rtkgpio)->port || SP_OK != sp_open((*rtkgpio)->port, SP_MODE_READ_WRITE)
        || SP_OK != sp_set_baudrate((*rtkgpio)->port, 230400)
        || SP_OK != sp_set_bits((*rtkgpio)->port, 8)
        || SP_OK != sp_set_parity((*rtkgpio)->port, SP_PARITY_NONE)
        || SP_OK != sp_set_stopbits((*rtkgpio)->port, 1)
        || SP_OK != sp_set_flowcontrol((*rtkgpio)->port, SP_FLOWCONTROL_NONE)) {
        rtkgpio_free(*rtkgpio);
        *rtkgpio = NULL;
    } else {
        (*rtkgpio)->boardmode = 0;
    }
    return rtkgpio != NULL ? 0 : -1;
}

void rtkgpio_free(struct rtkgpio *rtkgpio) {
    if (NULL != rtkgpio) {
        if (NULL != rtkgpio->port) {
            sp_free_port(rtkgpio->port);
            rtkgpio->port = NULL;
        }
        free(rtkgpio);
    }
}

void rtkgpio_set_boardmode(struct rtkgpio *rtkgpio, int boardmode) {
    rtkgpio->boardmode = boardmode;
}

int rtkgpio_setup(struct rtkgpio *rtkgpio, int channel, enum rtkgpio_mode mode,
                  enum rtkgpio_pud pud, enum rtkgpio_value value) {
    const int pin = resolve_pin(rtkgpio, channel);
    if (pin == -1) {
        return -1;
    }

    char data[2];
    if (getc_pin(pin, &data[0]) != 0) {
        return -1;
    }
    if (getc_mode(mode, &data[1]) != 0) {
        return -1;
    }
    if (serial_write(rtkgpio, data, sizeof(data)) < 0) {
        return -1;
    }
    if (getc_pud(pud, &data[1]) != 0) {
        return -1;
    }
    if (serial_write(rtkgpio, data, sizeof(data)) < 0) {
        return -1;
    }
    if (getc_value(value, &data[1]) != 0) {
        return -1;
    }
    if (serial_write(rtkgpio, data, sizeof(data)) < 0) {
        return -1;
    }
    return 0;
}

int rtkgpio_input(struct rtkgpio *rtkgpio, int channel, enum rtkgpio_value *value) {
    const int pin = resolve_pin(rtkgpio, channel);
    if (pin == -1) {
        return -1;
    }

    char data[2];
    if (getc_pin(pin, &data[0]) != 0) {
        return -1;
    }
    data[1] = CH_GPIO_READ;
    if (serial_write(rtkgpio, data, sizeof(data)) < 0) {
        return -1;
    }

    char buffer[4];
    if (serial_read(rtkgpio, buffer, 4, -1, "\n") < 0) {
        return -1;
    }
    if (parse_value(buffer[1], value) != 0) {
        return -1;
    }

    return 0;
}

int rtkgpio_output(struct rtkgpio *rtkgpio, int channel, enum rtkgpio_value value) {
    const int pin = resolve_pin(rtkgpio, channel);
    if (pin == -1) {
        return -1;
    }

    char data[2];
    if (getc_pin(pin, &data[0]) != 0) {
        return -1;
    }
    if (getc_value(value, &data[1]) != 0) {
        return -1;
    }
    if (serial_write(rtkgpio, data, sizeof(data)) < 0) {
        return -1;
    }

    return 0;
}
