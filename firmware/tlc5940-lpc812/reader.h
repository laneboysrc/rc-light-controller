#ifndef __READER_H
#define __READER_H

#include <stdint.h>
#include <stdbool.h>

struct channel_s {
    uint32_t raw_data;
    int32_t normalized;
    uint32_t absolute;
    uint32_t centre;
    uint32_t ep_l;
    uint32_t ep_h;
    bool reversed;
};

extern struct channel_s channel[3];

enum startup_mode_e {
    STARTUP_MODE_RUNNING = 0,
    STARTUP_MODE_NEUTRAL = (1 << 3)
};

extern enum startup_mode_e startup_mode;

extern bool new_channel_data;

void servo_reader_SCT_interrupt_handler(void);
void init_reader(void);
void read_all_channels(void);

#endif /* __READER_H */
