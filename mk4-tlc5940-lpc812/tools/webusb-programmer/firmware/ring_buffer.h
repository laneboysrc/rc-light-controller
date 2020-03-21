#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t RING_BUFFER_SIZE_T;

typedef struct  {
    uint32_t begin;
    uint32_t end;
    RING_BUFFER_SIZE_T size;
    uint8_t *data;
} RING_BUFFER_T;

void RING_BUFFER_init(RING_BUFFER_T *ring, uint8_t *buf, RING_BUFFER_SIZE_T size);
RING_BUFFER_SIZE_T RING_BUFFER_write(RING_BUFFER_T *ring, uint8_t *data, RING_BUFFER_SIZE_T size);
RING_BUFFER_SIZE_T RING_BUFFER_write_uint8(RING_BUFFER_T *ring, uint8_t data);
RING_BUFFER_SIZE_T RING_BUFFER_read(RING_BUFFER_T *ring, uint8_t *data, RING_BUFFER_SIZE_T size);
RING_BUFFER_SIZE_T RING_BUFFER_read_uint8(RING_BUFFER_T *ring, uint8_t *data);
bool RING_BUFFER_is_empty(RING_BUFFER_T *ring);
