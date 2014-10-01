/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <LPC8xx.h>
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


void init_servo_output(void) {
    if (!config.flags.steering_wheel_servo_output &&
        !config.flags.gearbox_servo_output) {
        return;
    }

    LPC_SCT->CONFIG |= (1u << 18);   // Auto-limit on counter H
    LPC_SCT->CTRL_H |= (1u << 3) |   // Clear the counter H
                       (11u << 5);   // PRE_H[12:5] = 12-1 (SCTimer H clock 1 MHz)
    LPC_SCT->MATCHREL[0].H = 20000 - 1;     // 20 ms per overflow (50 Hz)
    LPC_SCT->MATCHREL[4].H = 1500;          // Servo pulse 1.5 ms intially

    LPC_SCT->EVENT[0].STATE = 0xFFFF;       // Event 0 happens in all states
    LPC_SCT->EVENT[0].CTRL = (0 << 0) |     // Match register 0
                             (1u << 4) |     // Select H counter
                             (0x1u << 12);   // Match condition only

    LPC_SCT->EVENT[4].STATE = 0xFFFF;       // Event 4 happens in all states
    LPC_SCT->EVENT[4].CTRL = (4 << 0) |     // Match register 4
                             (1u << 4) |     // Select H counter
                             (0x1u << 12);   // Match condition only

    // We've chosen CTOUT_1 because CTOUT_0 resides in PINASSIGN6, which
    // changing may affect CTIN_1..3 that we need.
    // CTOUT_1 is in PINASSIGN7, where no other function is needed for our
    // application.
    LPC_SCT->OUT[1].SET = (1u << 0);         // Event 0 will set CTOUT_1
    LPC_SCT->OUT[1].CLR = (1u << 4);         // Event 4 will clear CTOUT_1

    // CTOUT_1 = PIO0_12
    LPC_SWM->PINASSIGN7 = 0xffffff0c;

    LPC_SCT->CTRL_H &= ~(1u << 2);           // Start the SCTimer H
}


void gearbox_action(uint8_t ch3_clicks)
{
    if (config.flags.gearbox_servo_output) {
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
    if (!config.flags.gearbox_servo_output  &&
        !config.flags.steering_wheel_servo_output) {
        return;
    }

    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_OFF) {
        servo_epl = -120;
        servo_centre = 0;
        servo_epr = 120;
        if (config.flags.gearbox_servo_output) {
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
    else if (config.flags.gearbox_servo_output) {
        servo_pulse = (global_flags.gear == GEAR_1) ? servo_epl : servo_epr;
    }

    // FIXME: output servo pulse
}


