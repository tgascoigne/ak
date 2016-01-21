#pragma once

#include <stdint.h>

void io_put8(uint16_t port, uint8_t value);
void io_put16(uint16_t port, uint16_t value);

uint8_t io_get8(uint16_t port);
uint16_t io_get16(uint16_t port);
