#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <ring_buffer.h>

// Ring buffer implementation from open-bldc's libgovernor
// https://github.com/open-bldc/open-bldc/tree/master/source/libgovernor
//
// Heavily modified: all functions return the number of bytes read/written


// ****************************************************************************
void RING_BUFFER_init(RING_BUFFER_T *ring, uint8_t *buf, RING_BUFFER_SIZE_T size)
{
    ring->data = buf;
    ring->size = size;
    ring->begin = 0;
    ring->end = 0;
}


// ****************************************************************************
RING_BUFFER_SIZE_T RING_BUFFER_write_uint8(RING_BUFFER_T *ring, uint8_t value)
{
    if (((ring->end + 1) % ring->size) != ring->begin) {
        ring->data[ring->end] = value;
        ring->end = (ring->end + 1) % ring->size;
        return 1;
    }

    return 0;
}


// ****************************************************************************
RING_BUFFER_SIZE_T RING_BUFFER_write(RING_BUFFER_T *ring, uint8_t *data, RING_BUFFER_SIZE_T size)
{
    RING_BUFFER_SIZE_T i;

    for (i = 0; i < size; i++) {
        if (RING_BUFFER_write_uint8(ring, *data) == 0) {
            break;
        }
        ++data;
    }

    return i;
}


// ****************************************************************************
RING_BUFFER_SIZE_T RING_BUFFER_read_uint8(RING_BUFFER_T *ring, uint8_t *data)
{
    if (data != NULL) {
        if (ring->begin != ring->end) {
            *data = ring->data[ring->begin];
            ring->begin = (ring->begin + 1) % ring->size;
            return 1;
        }
    }

    return 0;
}


// ****************************************************************************
RING_BUFFER_SIZE_T RING_BUFFER_read(RING_BUFFER_T *ring, uint8_t *data, RING_BUFFER_SIZE_T size)
{
    RING_BUFFER_SIZE_T i;

    for (i = 0; i < size; i++) {
        if (RING_BUFFER_read_uint8(ring, data) == 0) {
            break;
        }
        ++data;
    }

    return i;
}


// ****************************************************************************
bool RING_BUFFER_is_empty(RING_BUFFER_T *ring)
{
    return (ring->begin == ring->end);
}
