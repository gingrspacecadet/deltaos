#include <kernel/device.h>

#define MAX_DEVICES 32
//^^ needs to be changed later to be dynamic but there's no MM yet sooooo yeahh

static struct device *devices[MAX_DEVICES];
static uint32 device_num = 0;

static bool streq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a++ != *b++) return false;
    }
    return *a == *b;
}

void device_register(struct device *dev) {
    if (!dev || device_num >= MAX_DEVICES) return;
    devices[device_num++] = dev;
}

struct device *device_find(const char *name) {
    if (!name) return NULL;
    
    for (uint32 i = 0; i < device_num; i++) {
        if (devices[i] && streq(devices[i]->name, name)) {
            return devices[i];
        }
    }
    return NULL;
}

struct device *device_find_type(enum device_type type) {
    for (uint32 i = 0; i < device_num; i++) {
        if (devices[i] && devices[i]->type == type) {
            return devices[i];
        }
    }
    return NULL;
}

struct device *device_find_subtype(enum device_subtype subtype) {
    for (uint32 i = 0; i < device_num; i++) {
        if (devices[i] && devices[i]->subtype == subtype) {
            return devices[i];
        }
    }
    return NULL;
}

struct device *device_next(struct device *prev) {
    if (!prev) {
        return device_num > 0 ? devices[0] : NULL;
    }
    
    for (uint32 i = 0; i < device_num; i++) {
        if (devices[i] == prev && i + 1 < device_num) {
            return devices[i + 1];
        }
    }
    return NULL;
}

uint32 device_count(void) {
    return device_num;
}
