/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <hal.h>
#include <globals.h>
#include <printf.h>


static bool next = false;
static int16_t servo_position = 0;
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
    uint8_t pin = HAL_GPIO_NO_PIN;

    if (!config.flags2.servo_output_enabled) {
        return;
    }

    servo_pulse = servo_output_endpoint.centre;

    // Initialize the servo output on the configured pin
    if (config.flags2.servo_on_th) {
        pin = HAL_GPIO_TH.pin;
    }
    if (config.flags2.servo_on_out) {
        pin = HAL_GPIO_OUT.pin;
    }
    HAL_servo_output_init(pin);

    if (config.flags.gearbox_servo_output) {
        global_flags.gear = GEAR_1;
        activate_gearbox_servo();
    }
}


// ****************************************************************************
void set_servo_pulse(uint16_t value)
{
    servo_pulse = value;
}

// ****************************************************************************
void set_servo_position(int16_t value)
{
    // Ensure the servo position is in the range of -100 .. 0 ,, +100
    value = MAX(value, -100);
    value = MIN(value, 100);

    servo_position = value;
}

// ****************************************************************************
int8_t get_servo_position(void)
{
    return servo_position;
}

// ****************************************************************************
void set_gear(uint8_t new_gear) {
    // Ensure that new_gear is in the valid range
    if (new_gear < 1) {
        new_gear = 1;
    }
    if (new_gear > config.number_of_gears) {
        new_gear = config.number_of_gears;
    }

    if (global_flags.gear == new_gear) {
        return;
    }

    global_flags.gear = new_gear;
    global_flags.gear_change_requested = true;
    printf("gear %d\n", global_flags.gear);
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
            set_gear(GEAR_1);
        }
        else {
            set_gear(GEAR_2);
        }
    }

    // 3-speed gearbox: one click switches gear up, two clicks gear down
    else {
        if (ch3_clicks == 1) {
            if (global_flags.gear == GEAR_1) {
                set_gear(GEAR_2);
            }
            else {
                set_gear(GEAR_3);
            }
        }
        else {
            if (global_flags.gear == GEAR_3) {
                set_gear(GEAR_2);
            }
            else {
                set_gear(GEAR_1);
            }
        }
    }
}


// ****************************************************************************
static void diag(const char *s)
{
    printf("servo setup %s\n", s);
}


// ****************************************************************************
void servo_output_setup_action(uint8_t ch3_clicks)
{
    if (!config.flags2.servo_output_enabled) {
        return;
    }

    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_OFF) {
        servo_output_endpoint.left = 900;
        servo_output_endpoint.centre = 1500;
        servo_output_endpoint.right = 2100;
        global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_LEFT;
        diag("left");
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
            diag("cancelled");
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
static void calculate_servo_pulse(int16_t value)
{
    if (value < 0) {
        servo_pulse = servo_output_endpoint.centre -
            (((servo_output_endpoint.centre - servo_output_endpoint.left) *
                (-value)) / 100);
    }
    else {
        servo_pulse = servo_output_endpoint.centre +
            (((servo_output_endpoint.right - servo_output_endpoint.centre) *
                value) / 100);
    }
}


// ****************************************************************************
void process_servo_output(void)
{
    if (!config.flags2.servo_output_enabled) {
        return;
    }

    // --------------------------------
    // Servo output setup is active
    if (global_flags.servo_output_setup != SERVO_OUTPUT_SETUP_OFF) {
        HAL_servo_output_enable();
        calculate_servo_pulse(channel[ST].normalized);
        if (next) {
            next = false;

            switch (global_flags.servo_output_setup) {
                case SERVO_OUTPUT_SETUP_LEFT:
                    servo_setup_endpoint.left = servo_pulse;
                    global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_CENTRE;
                    diag("center");
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

                        activate_gearbox_servo();
                        global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_OFF;
                        diag("done");
                    }
                    else {
                        global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_RIGHT;
                        diag("right");
                    }
                    break;

                case SERVO_OUTPUT_SETUP_RIGHT:
                    servo_setup_endpoint.right = servo_pulse;

                    servo_output_endpoint.right = servo_setup_endpoint.right;
                    servo_output_endpoint.centre = servo_setup_endpoint.centre;
                    servo_output_endpoint.left = servo_setup_endpoint.left;
                    write_persistent_storage();

                    if (config.flags.gearbox_servo_output) {
                        activate_gearbox_servo();
                    }

                    global_flags.servo_output_setup = SERVO_OUTPUT_SETUP_OFF;
                    diag("done");
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
        calculate_servo_pulse(channel[ST].normalized);
    }

    // --------------------------------
    // Light progrqm servo output is avtive
    else if (config.flags2.light_program_servo_output) {
        calculate_servo_pulse(servo_position);
    }

    HAL_servo_output_set_pulse(servo_pulse);
}


