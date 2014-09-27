/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>

static bool next = false;
static uint16_t servo_pulse;

static uint16_t servo_setup_centre;
static uint16_t servo_setup_epl;
static uint16_t servo_setup_epr;

// FIXME: move somewhere else?
static uint16_t servo_centre;
static uint16_t servo_epl;
static uint16_t servo_epr;


static uint16_t gearbox_servo_active_counter;
static uint16_t gearbox_servo_idle_counter;

// FIXME: make configurable
#define GEARBOX_SWITCH_TIME 1

void gearbox_action(uint8_t ch3_clicks)
{
    if (!config.flags.gearbox_servo_enabled) {
        return;
    }

    if (ch3_clicks == 1) {
        global_flags.gear = GEAR_1;
    }
    else {
        global_flags.gear = GEAR_2;
    }

    // FIXME: we need to ensure this is active for one mainloop!
    global_flags.gear_changed = true;

    gearbox_servo_active_counter = GEARBOX_SWITCH_TIME;
    gearbox_servo_idle_counter = 0;
}

void servo_output_setup_action(uint8_t ch3_clicks)
{
    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_OFF) {
        servo_epl = -120;
        servo_centre = 0;
        servo_epr = 120;
        if (config.flags.gearbox_servo_enabled) {
            global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_LEFT;
        }
        else {
            global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_CENTRE;
        }
    }
    else {
        if (ch3_clicks == 1) {
            // 1 click: next setup step
            next = true;            //global_flags.steering_wheel_servo_setup_mode = STEERING_WHEEL_SERVO_SETUP_MODE_NEXT;
        }
        else {
            // More than 1 click: cancel setup
            global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_OFF;
            load_persistent_storage();
        }
    }
}


// This function calculates:
//
//       (right - centre) * abs(steering)
//       -------------------------------- + centre
//                 100
//
// To ease calculation we first do right - centre, then calculate its absolute
// value but store the sign. After multiplication and division using the
// absolute value we re-apply the sign, then add centre.
//
// Note: this function is needed by Process_servo_setup, so it can't be
// removed e.g. if only a gearbox servo is used.
void calculate_servo_pulse(void)
{

    if (channel[ST].normalized < 0) {
        servo_pulse = servo_centre -
            (((servo_centre - servo_epl) * channel[ST].absolute) / 100);
    }
    else {
        servo_pulse = servo_centre +
            (((servo_epr - servo_centre) * channel[ST].absolute) / 100);
    }
}


// ****************************************************************************
// Process_steering_wheel_servo
//
// ****************************************************************************
void process_servo_output(void)
{
    if (global_flags.servo_output_setup != SERVO_OUTPUT_SETUP_OFF) {
        calculate_servo_pulse();
        if (next) {
            next = false;

            switch (global_flags.servo_output_setup) {
                case SERVO_OUTPUT_SETUP_CENTRE:
                    servo_setup_centre = servo_pulse;
                    global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_LEFT;
                    break;

                case SERVO_OUTPUT_SETUP_LEFT:
                    servo_setup_epl = servo_pulse;
                    global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_RIGHT;
                    break;

                case SERVO_OUTPUT_SETUP_RIGHT:
                    servo_epr = servo_setup_epr = servo_pulse;
                    servo_centre = servo_setup_centre;
                    servo_epl = servo_setup_epl;
                    write_persistent_storage();
                    global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_OFF;
                    break;

                default:
                    break;
            }
        }
    }
    else if (config.flags.gearbox_servo_enabled) {
        servo_pulse = (global_flags.gear == GEAR_1) ? servo_epl : servo_epr;
    }

    // FIXME: output servo pulse
}


