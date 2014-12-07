/******************************************************************************

    This module reads the servo pulses for steering, throttle and CH3/AUX
    from a CPPM output of a receiver.

    It populates the global channel[] array with the read data.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>


#define SERVO_PULSE_MIN 600
#define SERVO_PULSE_MAX 2500
#define SERVO_PULSE_CLAMP_LOW 800
#define SERVO_PULSE_CLAMP_HIGH 2300
#define STARTUP_TIME 2000           // Time at startup until neutral is initialized


static enum {
    WAIT_FOR_FIRST_PULSE,
    WAIT_FOR_TIMEOUT,
    NORMAL_OPERATION
} servo_reader_state = WAIT_FOR_FIRST_PULSE;

static volatile bool new_raw_channel_data = false;
static uint32_t servo_reader_timer;


// ****************************************************************************
void init_cppm_reader(void)
{
    if (config.mode != MASTER_WITH_CPPM_READER) {
        return;
    }

    global_flags.initializing = 1;

    // SCTimer setup
    // At this point we assume that SCTimer has been setup in the following way:
    //
    //  * Split 16-bit timers
    //  * Event 1 is available for our use
    //  * Register 1 is available for our use
    //  * CTIN_1 is available for our use

    LPC_SCT->CTRL_L |= (1 << 3) |   // Clear the counter L
                       (5 << 5);    // PRE_L[12:5] = 6-1 (SCTimer L clock 2 MHz)


    // Configure registers 1 to capture CPPM pulses on SCTimer L
    LPC_SCT->REGMODE_L |= (1 << 1);         // Register 1 is capture register

    LPC_SCT->EVENT[1].STATE = 0xFFFF;       // Event 1 happens in all states
    LPC_SCT->EVENT[1].CTRL = (0 << 5) |     // OUTSEL: select input elected by IOSEL
                             (1 << 6) |     // IOSEL: CTIN_1
                             (0x1 << 10) |  // IOCOND: rising edge
                             (0x2 << 12);   // COMBMODE: Uses the specified I/O condition only
    LPC_SCT->CAPCTRL[1].L = (1 << 1);       // Event 1 loads capture register 1
    LPC_SCT->EVEN |= (1 << 1);              // Event 1 generates an interrupt


    // SCT CTIN_1 at PIO0.0
    LPC_SWM->PINASSIGN6 = 0xffffff00;

    LPC_SCT->CTRL_L &= ~(1 << 2);           // Start the SCTimer L
    NVIC_EnableIRQ(SCT_IRQn);
}


// ****************************************************************************
// FIXME: we need to multiplex this with the servo reader
void SCT_irq_handler_FIXME(void)
{
    // static uint32_t start[3] = {0, 0, 0};
    // static uint32_t result[3] = {0, 0, 0};
    // static uint32_t channel_flags = 0;
    uint32_t capture_value;

    // Event 1: Capture CTIN_1
    if (LPC_SCT->EVFLAG & (1 << 1)) {
        capture_value = LPC_SCT->CAP[1].L;

        // if (start[1 - 1] > capture_value) {
        //     // Compensate for wrap-around
        //     capture_value += LPC_SCT->MATCHREL[0].L + 1;
        // }
        // result[1 - 1] = capture_value - start[i - 1];

        LPC_SCT->EVFLAG = (1 << 1);
    }
}


// ****************************************************************************
static void normalize_channel(CHANNEL_T *c)
{
    if (c->raw_data < SERVO_PULSE_MIN  ||  c->raw_data > SERVO_PULSE_MAX) {
        c->normalized = 0;
        c->absolute = 0;
        return;
    }

    if (c->raw_data < SERVO_PULSE_CLAMP_LOW) {
        c->raw_data = SERVO_PULSE_CLAMP_LOW;
    }

    if (c->raw_data > SERVO_PULSE_CLAMP_HIGH) {
        c->raw_data = SERVO_PULSE_CLAMP_HIGH;
    }

    if (c->raw_data == c->endpoint.centre) {
        c->normalized = 0;
    }
    else if (c->raw_data < c->endpoint.centre) {
        if (c->raw_data < c->endpoint.left) {
            c->endpoint.left = c->raw_data;
        }
        // In order to acheive a stable 100% value we actually calculate the
        // percentage up to 101%, and then clamp to 100%.
        c->normalized = (c->endpoint.centre - c->raw_data) * 101 /
            (c->endpoint.centre - c->endpoint.left);
        if (c->normalized > 100) {
            c->normalized = 100;
        }
        if (!c->reversed) {
            c->normalized = -c->normalized;
        }
    }
    else {
        if (c->raw_data > c->endpoint.right) {
            c->endpoint.right = c->raw_data;
        }
        c->normalized = (c->raw_data - c->endpoint.centre) * 101 /
            (c->endpoint.right - c->endpoint.centre);
        if (c->normalized > 100) {
            c->normalized = 100;
        }
        if (c->reversed) {
            c->normalized = -c->normalized;
        }
    }

    if (c->normalized < 0) {
        c->absolute = -c->normalized;
    }
    else {
        c->absolute = c->normalized;
    }
}


// ****************************************************************************
static void initialize_channel(CHANNEL_T *c) {
    c->endpoint.centre = c->raw_data;
    c->endpoint.left = c->raw_data - config.initial_endpoint_delta;
    c->endpoint.right = c->raw_data + config.initial_endpoint_delta;
}


// ****************************************************************************
void read_all_cppm_channels(void)
{
    if (config.mode != MASTER_WITH_CPPM_READER) {
        return;
    }

    if (global_flags.systick) {
        if (servo_reader_timer) {
            --servo_reader_timer;
        }
    }

    global_flags.new_channel_data = false;

    if (!new_raw_channel_data) {
        return;
    }
    new_raw_channel_data = false;

    switch (servo_reader_state) {
        case WAIT_FOR_FIRST_PULSE:
            servo_reader_timer = STARTUP_TIME / __SYSTICK_IN_MS;
            servo_reader_state = WAIT_FOR_TIMEOUT;
            break;

        case WAIT_FOR_TIMEOUT:
            if (servo_reader_timer == 0) {
                initialize_channel(&channel[ST]);
                initialize_channel(&channel[ST]);
                if (!config.flags.ch3_is_local_switch) {
                    initialize_channel(&channel[CH3]);
                }

                servo_reader_state = NORMAL_OPERATION;
                global_flags.initializing = 0;
            }
            global_flags.new_channel_data = true;
            break;

        case NORMAL_OPERATION:
            normalize_channel(&channel[ST]);
            normalize_channel(&channel[TH]);
            if (!config.flags.ch3_is_local_switch) {
                // FIXME: does this need to be different for CH3?!!
                normalize_channel(&channel[CH3]);
            }
            global_flags.new_channel_data = true;
            break;

        default:
            servo_reader_state = WAIT_FOR_FIRST_PULSE;
            break;
    }
}
