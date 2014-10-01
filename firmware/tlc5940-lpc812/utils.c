#include <stdint.h>
#include <globals.h>

#include <utils.h>


// ****************************************************************************
// next16
//
// 16-bit Linear Feedback Shift Register implementation
// ****************************************************************************
static void next16(uint16_t *lfsr)
{
    // Feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1
    unsigned bit = ((*lfsr >> 0) ^ (*lfsr >> 2) ^ (*lfsr >> 3) ^ (*lfsr >> 5)) & 1;
    *lfsr =  (*lfsr >> 1) | (uint16_t)(bit << 15);
}


// ****************************************************************************
// random_min_max
//
// Returns a random value between min and max (both inclusive).
// ****************************************************************************
uint16_t random_min_max(uint16_t min, uint16_t max)
{
    static uint16_t lfsr = 0;

    // Ensure 1 <= min < max
    if (min == 0) {
        min = 1;
    }

    if (min >= max) {
        return min;
    }

    // If it is the first call to random_min_max we initialize with the
    // entropy value (provided externally).
    if (lfsr == 0) {
        lfsr = (uint16_t)entropy;
    }

    next16(&lfsr);
    return (uint16_t)(min + (lfsr % (max - min + 1)));
}
