/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <hal.h>
#include <globals.h>

static bool next = false;
static uint16_t servo_pulse;

static uint16_t gearbox_servo_counter;
static bool gearbox_servo_active;

static SERVO_ENDPOINTS_T servo_setup_endpoint;
SERVO_ENDPOINTS_T servo_output_endpoint;


// ****************************************************************************
static void activate_gearbox_servo(void)
{
    gearbox_servo_active = true;
    gearbox_servo_counter = config.gearbox_servo_active_time;

    HAL_servo_output_enable();
}


// ****************************************************************************
void init_servo_output(void) {
    if (!global_flags.servo_output_enabled) {
        return;
    }

    HAL_servo_output_init();

    global_flags.gear = GEAR_1;
    activate_gearbox_servo();
}


// ****************************************************************************
void gearbox_action(uint8_t ch3_clicks)
{
    if (!config.flags.gearbox_servo_output) {
        return;
    }

    // 2-speed gearbox: one click is Gear 1; two clicks is Gear 2
    if (config.number_of_gears == 2) {
        if (ch3_clicks == 1) {
            global_flags.gear = GEAR_1;
        }
        else {
            global_flags.gear = GEAR_2;
        }
    }

    // 3-speed gearbox: one click switches gear up, two clicks gear down
    else {
        if (ch3_clicks == 1) {
            if (global_flags.gear == GEAR_1) {
                global_flags.gear = GEAR_2;
            }
            else {
                global_flags.gear = GEAR_3;
            }
        }
        else {
            if (global_flags.gear == GEAR_3) {
                global_flags.gear = GEAR_2;
            }
            else {
                global_flags.gear = GEAR_1;
            }
        }
    }

    // FIXME: we need to ensure this is active for one mainloop!
    global_flags.gear_changed = true;
    activate_gearbox_servo();
}


// ****************************************************************************
void servo_output_setup_action(uint8_t ch3_clicks)
{
    if (!global_flags.servo_output_enabled) {
        return;
    }

    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_OFF) {
        servo_output_endpoint.left = 900;
        servo_output_endpoint.centre = 1500;
        servo_output_endpoint.right = 2100;
        global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_LEFT;
    }
    else {
        if (ch3_clicks == 1) {
            // 1 click: next setup step
            next = true;
            //global_flags.steering_wheel_servo_setup_mode = STEERING_WHEEL_SERVO_SETUP_MODE_NEXT;
        }
        else {
            // More than 1 click: cancel setup
            global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_OFF;
            load_persistent_storage();
        }
    }
}


/******************************************************************************

    This function calculates:

          (right - centre) * abs(steering)
          -------------------------------- + centre
                    100

    To ease calculation we first do right - centre, then calculate its absolute
    value but store the sign. After multiplication and division using the
    absolute value we re-apply the sign, then add centre.

    Note: this function is needed by Process_servo_setup, so it can't be
    removed e.g. if only a gearbox servo is used.

******************************************************************************/
static void calculate_servo_pulse(void)
{
    if (channel[ST].normalized < 0) {
        servo_pulse = servo_output_endpoint.centre -
            (((servo_output_endpoint.centre - servo_output_endpoint.left) *
                channel[ST].absolute) / 100);
    }
    else {
        servo_pulse = servo_output_endpoint.centre +
            (((servo_output_endpoint.right - servo_output_endpoint.centre) *
                channel[ST].absolute) / 100);
    }
}


// ****************************************************************************
void process_servo_output(void)
{
    if (!global_flags.servo_output_enabled) {
        return;
    }

    // --------------------------------
    // Servo output setup is active
    if (global_flags.servo_output_setup != SERVO_OUTPUT_SETUP_OFF) {
        calculate_servo_pulse();
        if (next) {
            next = false;

            switch (global_flags.servo_output_setup) {
                case SERVO_OUTPUT_SETUP_LEFT:
                    servo_setup_endpoint.left = servo_pulse;
                    global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_CENTRE;
                    break;

                case SERVO_OUTPUT_SETUP_CENTRE:
                    servo_setup_endpoint.centre = servo_pulse;

                    // In case we are dealing with a 2-speed gearbox we only
                    // configure left and center endpoint
                    if (config.flags.gearbox_servo_output &&
                            config.number_of_gears == 2) {
                        servo_output_endpoint.centre = servo_setup_endpoint.centre;
                        servo_output_endpoint.left = servo_setup_endpoint.left;
                        write_persistent_storage();

                        global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_OFF;
                    }
                    else {
                        global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_RIGHT;
                    }
                    break;

                case SERVO_OUTPUT_SETUP_RIGHT:
                    servo_setup_endpoint.right = servo_pulse;

                    servo_output_endpoint.right = servo_setup_endpoint.right;
                    servo_output_endpoint.centre = servo_setup_endpoint.centre;
                    servo_output_endpoint.left = servo_setup_endpoint.left;
                    write_persistent_storage();

                    global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_OFF;
                    break;

                default:
                    break;
            }
        }
    }

    // --------------------------------
    // Gearbox servo is active
    else if (config.flags.gearbox_servo_output) {

        if (config.gearbox_servo_idle_time && global_flags.systick) {
            if (gearbox_servo_counter) {
                --gearbox_servo_counter;
            }
            else {
                if (gearbox_servo_active) {
                    HAL_servo_output_disable();

                    gearbox_servo_counter = config.gearbox_servo_idle_time;
                    gearbox_servo_active = false;
                }
                else {
                    activate_gearbox_servo();
                }
            }
        }

        if (global_flags.gear == GEAR_1) {
            servo_pulse = servo_output_endpoint.left;
        }
        else if (global_flags.gear == GEAR_2) {
            servo_pulse = servo_output_endpoint.centre;
        }
        else if (global_flags.gear == GEAR_3) {
            servo_pulse = servo_output_endpoint.right;
        }
    }

    // --------------------------------
    // Steering wheel servo output is avtive
    else if (config.flags.steering_wheel_servo_output) {
        calculate_servo_pulse();
    }

    HAL_servo_output_set_pulse(servo_pulse);
}


