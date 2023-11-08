/******************************************************************************

    Outputs combined Steering, Throttle and CH3/AUX/AUX2/AUX3 information in
    the UART Tx.

    This allows running a single servo extension wire to a peripheral doing its
    own processing of the servo signals.
    Channel data is output "normalized", which means the values go from
    -100 to +100%.
    There is also a flag sent out that indicates when the preprocessor is
    initializing and reading the 0-position of steering and throttle.

    For legacy reasons bit 0 in byte 3 indicates whether CH3 is 0 or 1.

    Since Nov 2023 the Pre-processor output always outputs 5 channels.
    For 3 channel devices (e.g. 3 channel Pre-processor) AUX2 and AUX3 are
    always sending the value 0.

******************************************************************************/
#include <stdint.h>

#include <globals.h>
#include <printf.h>
#include <hal.h>

#define SLAVE_MAGIC_BYTE 0x87

#define PACKET_LENGTH 4
#define PACKET_LENGTH_MULTI 7

static bool ch3_2pos = false;
static uint8_t tx_data[7] = {SLAVE_MAGIC_BYTE};
static uint8_t next_tx_index = 0xff;


// ****************************************************************************
void output_preprocessor(void)
{
    if (!config.flags.preprocessor_output) {
        return;
    }

    if (global_flags.new_channel_data) {
        if (ch3_2pos) {
            if (channel[2].normalized < -AUX_HYSTERESIS) {
                ch3_2pos = false;
            }
        }
        else {
            if (channel[2].normalized > AUX_HYSTERESIS) {
                ch3_2pos = true;
            }
        }

        // tx_data[0] = SLAVE_MAGIC_BYTE;
        tx_data[1] = channel[ST].normalized;
        tx_data[2] = channel[TH].normalized;
        tx_data[3] = (ch3_2pos ? (1 << 0) : 0) |
                     (config.flags2.multi_aux ? (1 << 3) : 0) |
                     (global_flags.initializing ? (1 << 4) : 0);
        tx_data[4] = channel[AUX].normalized;
        tx_data[5] = channel[AUX2].normalized;
        tx_data[6] = channel[AUX3].normalized;

        next_tx_index = 0;
    }

    if (next_tx_index < PACKET_LENGTH_MULTI) {
        HAL_putc(STDOUT, tx_data[next_tx_index++]);
    }
}

