#ifndef __READER_H
#define __READER_H

#include <stdint.h>
#include <stdbool.h>

#define ST 0
#define TH 1
#define CH3 2

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

void servo_reader_SCT_interrupt_handler(void);
void init_reader(void);
void read_all_channels(void);

#endif /* __READER_H */
