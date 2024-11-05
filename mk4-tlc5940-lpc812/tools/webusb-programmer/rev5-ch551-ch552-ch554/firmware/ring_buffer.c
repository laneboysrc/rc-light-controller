#include <stdint.h>
#include <stdbool.h>
#include "ring_buffer.h"


typedef uint8_t RING_BUFFER_SIZE_T;

// RING_BUFFER_SIZE must be a power of 2!
#define RING_BUFFER_SIZE 64
#define RING_BUFFER_MASK (RING_BUFFER_SIZE - 1)

static RING_BUFFER_SIZE_T begin;
static RING_BUFFER_SIZE_T end;
static RING_BUFFER_SIZE_T count;
static __xdata uint8_t buffer[RING_BUFFER_SIZE];


// ****************************************************************************
void RING_BUFFER_init(void)
{
    begin = 0;
    end = 0;
    count = 0;
}


// ****************************************************************************
void RING_BUFFER_write_uint8(uint8_t value)
{
    if (count < RING_BUFFER_SIZE) {
        buffer[end] = value;
        end = (end + 1) & RING_BUFFER_MASK;
        ++count;
    }
}


// ****************************************************************************
uint8_t RING_BUFFER_read_uint8(void)
{
    uint8_t data;

    if (count) {
        data = buffer[begin];
        begin = (begin + 1) & RING_BUFFER_MASK;
        --count;
        return data;
    }
    return 0;
}


// ****************************************************************************
bool RING_BUFFER_is_empty(void)
{
    return (count == 0);
}


// ****************************************************************************
bool RING_BUFFER_is_full(void)
{
    return (count >= RING_BUFFER_SIZE);
}


// ****************************************************************************
bool RING_BUFFER_is_at_least_half_full(void)
{
    return (count >= RING_BUFFER_SIZE / 2);
}
