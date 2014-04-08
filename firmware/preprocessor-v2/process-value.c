#include "processor.h"
#include <stdint.h>

uint8_t receivedValues[3];

extern struct {
    unsigned locked : 1;
    unsigned dataChanged : 1;
} flags;

#define TIMING_OFFSET 0x20
#define SYNC_VALUE_LIMIT 0x880
#define SYNC_VALUE_NOMINAL 0xa40

// FIXME: Work with HK310 expansion protocol, and without

void Process_value(void) 
{
    uint16_t value;
    uint8_t data;
    static uint8_t oldData = 0;
    static uint8_t valueCount = 0;

    flags.dataChanged = 0;
    value = (TMR1H << 8) + TMR1L;
    
    // Remove the offset we added in the transmitter to ensure the minimum
    // pulse does not go down to zero.
    if (value < SYNC_VALUE_LIMIT) {
        value -= TIMING_OFFSET;
    }
    // If we are dealing with a sync value we clamp it to the nominal value
    // so that our check whether the value has changed does not trigger 
    // wrongly when jitter moves between e.g. 0xa40 and 0xa3f
    else { // === if (value >= SYNC_VALUE_LIMIT)
        value = SYNC_VALUE_NOMINAL;
    }
    
    data = value >> 5;

    if (oldData != data) {
        if (value < SYNC_VALUE_LIMIT) {
            if (valueCount < 3) {
                receivedValues[valueCount] = data;
            }
            ++valueCount;
        }
        else {
            if (valueCount == 3) {
                flags.dataChanged = 1;
            }
            valueCount = 0;
        }        
    }
    oldData = data;
}

