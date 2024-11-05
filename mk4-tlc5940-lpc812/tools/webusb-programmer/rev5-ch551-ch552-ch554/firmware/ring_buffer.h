#pragma once

#include <stdint.h>
#include <stdbool.h>


void RING_BUFFER_init(void);
void RING_BUFFER_write_uint8(uint8_t data);
uint8_t RING_BUFFER_read_uint8(void);
bool RING_BUFFER_is_empty(void);
bool RING_BUFFER_is_full(void);
bool RING_BUFFER_is_at_least_half_full(void);
