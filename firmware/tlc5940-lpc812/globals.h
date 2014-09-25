#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

struct global_flags_s {
    unsigned int soft_timer : 1;             // Set for one mainloop every 20 ms
    unsigned int new_channel_data : 1;       // Set for one mainloop every time servo pulses were received
    unsigned int startup_mode_neutral : 1;
    unsigned int blink_flag : 1;             // Toggles with 1.5 Hz
    unsigned int blink_hazard : 1;           // Hazard lights active
    unsigned int blink_indicator_left : 1;   // Left indicator active
    unsigned int blink_indicator_right : 1;  // Right indicator active
    unsigned int forward : 1;                // Set when the car is driving forward
    unsigned int braking : 1;                // Set when the brakes are enganged
    unsigned int reversing : 1;              // Set when the car is reversing
};

extern struct global_flags_s global_flags;


#endif // __GLOBALS_H
