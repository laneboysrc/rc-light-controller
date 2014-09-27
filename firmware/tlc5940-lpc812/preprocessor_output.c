#include <stdint.h>

#include <globals.h>
#include <uart0.h>

#define SLAVE_MAGIC_BYTE 0x87
#define CH3_HYSTERESIS 5

static bool ch3_2pos = false;
static uint8_t tx_data[4];
static uint8_t next_tx_index = 0xff;

// ****************************************************************************
void output_preprocessor(void)
{
    if (config.flags.preprocessor_output)
    {
        return;
    }

    // FIXME: add 5th byte for hk310 expansion support
    if (!global_flags.new_channel_data) {
        if (ch3_2pos) {
            if (channel[2].normalized < -CH3_HYSTERESIS) {
                ch3_2pos = false;
            }
        }
        else {
            if (channel[2].normalized > CH3_HYSTERESIS) {
                ch3_2pos = true;
            }
        }

        tx_data[0] = SLAVE_MAGIC_BYTE;
        tx_data[1] = channel[0].normalized;
        tx_data[2] = channel[1].normalized;
        tx_data[3] = (ch3_2pos ? (1 << 0) : 0)|
                     (global_flags.startup_mode_neutral ? (1 << 4) : 0);

        next_tx_index = 0;
    }

    if (next_tx_index < sizeof(tx_data)  &&  uart0_send_is_ready()) {
        uart0_send_char(tx_data[next_tx_index++]);
    }
}

