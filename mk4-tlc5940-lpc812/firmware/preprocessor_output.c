/******************************************************************************

    Outputs combined Steering, Throttle and CH3/AUX information in the UART Tx.

    This allows running a single servo extension wire to a peripheral doing its
    own processing of the servo signals.
    Data is output "normalized", which means the values go from -100 to +100%.
    CH3 is either 0 or 1.
    There is also a flag sent out that indicates when the preprocessor is
    initializing and reading the 0-position of steering and throttle.

******************************************************************************/
#include <stdint.h>

#include <globals.h>
#include <printf.h>

#define SLAVE_MAGIC_BYTE 0x87
#define CH3_HYSTERESIS 5

static bool ch3_2pos = false;
static uint8_t tx_data[4];
static uint8_t next_tx_index = 0xff;


// ****************************************************************************
void output_preprocessor(void)
{
    if (!config.flags.preprocessor_output) {
        return;
    }

    if (global_flags.new_channel_data) {
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
        tx_data[1] = channel[ST].normalized;
        tx_data[2] = channel[TH].normalized;
        tx_data[3] = (ch3_2pos ? (1 << 0) : 0) |
                     (global_flags.initializing ? (1 << 4) : 0);

        next_tx_index = 0;
    }

    if (next_tx_index < sizeof(tx_data)) {
        printf("%c", tx_data[next_tx_index++]);
    }
}

