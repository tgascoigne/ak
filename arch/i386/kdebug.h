#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <kernel/io/dev.h>

extern iodev_t ConsoleDev;
extern ssize_t console_write(iodev_t *dev, const void *buf, size_t count);

#define kdebugf(...) {\
        char buf[256]; \
        int count = sprintf(buf, __VA_ARGS__); \
        console_write(&ConsoleDev, buf, (size_t)count);   \
    }
