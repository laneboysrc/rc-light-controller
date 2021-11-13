
#include <stdio.h>
#include <stdbool.h>

#include <hal.h>
#include <globals.h>


#define SERVO_PULSE_CLAMP_LOW 800
#define SERVO_PULSE_CLAMP_HIGH 2300


static enum {
    WAIT_FOR_FIRST_PULSE,
    WAIT_FOR_TIMEOUT,
    NORMAL_OPERATION
} servo_reader_state = WAIT_FOR_FIRST_PULSE;

static uint32_t servo_reader_timer;


// ****************************************************************************
void init_servo_reader(void)
{
#ifdef ENABLE_SERVO_READER
    if (config.mode != MASTER_WITH_SERVO_READER) {
        return;
    }

    HAL_servo_reader_init();
#endif
}


// ****************************************************************************
static void normalize_channel(CHANNEL_T *c)
{
    if (c->raw_data < config.servo_pulse_min  ||  c->raw_data > config.servo_pulse_max) {
        if (c->auto_endpoint) {
            c->normalized = 0;
            c->absolute = 0;
        }
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
        if (c->auto_endpoint) {
            if (c->raw_data < c->endpoint.left) {
                c->endpoint.left = c->raw_data;
            }
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
        if (c->auto_endpoint) {
            if (c->raw_data > c->endpoint.right) {
                c->endpoint.right = c->raw_data;
            }
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
static void normalize_all_channels(void) {
    uint8_t index;

    for (index = 0; index < 5; index++) {
        normalize_channel(&channel[index]);
    }
}


// ****************************************************************************
void read_all_servo_channels(void)
{
    uint32_t raw_data[5];

    if (global_flags.systick) {
        if (servo_reader_timer) {
            --servo_reader_timer;
        }
    }

    if (config.mode == MASTER_WITH_SERVO_READER) {
#ifdef ENABLE_SERVO_READER
        if (!HAL_servo_reader_get_new_channels(raw_data)) {
            return;
        }
#endif
    }
#ifdef ENABLE_IBUS_READER
    else if (config.mode == MASTER_WITH_IBUS_READER) {
        if (!ibus_reader_get_new_channels(raw_data)) {
            return;
        }
    }
#endif
    else {
        // Neither SERVO READER or IBUS READER: bail out!
        return;
    }


    channel[ST].raw_data = raw_data[0];
    channel[TH].raw_data = raw_data[1];
    channel[AUX].raw_data = raw_data[2];
    channel[AUX2].raw_data = raw_data[3];
    channel[AUX3].raw_data = raw_data[4];

    switch (servo_reader_state) {
        case WAIT_FOR_FIRST_PULSE:
            servo_reader_timer = config.startup_time;
            servo_reader_state = WAIT_FOR_TIMEOUT;
            break;

        case WAIT_FOR_TIMEOUT:
            if (servo_reader_timer == 0) {
                servo_reader_state = NORMAL_OPERATION;
                global_flags.initializing = false;

                initialize_channel(&channel[ST]);
                initialize_channel(&channel[TH]);
                break;
            }
            global_flags.new_channel_data = true;
            break;

        case NORMAL_OPERATION:
            normalize_all_channels();
            global_flags.new_channel_data = true;
            break;

        default:
            servo_reader_state = WAIT_FOR_FIRST_PULSE;
            break;
    }
}
