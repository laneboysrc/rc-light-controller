/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <hal.h>
#include <globals.h>
#include <printf.h>


GLOBAL_FLAGS_T global_flags;

CHANNEL_T channel[5] = {
    {   // STEERING
        .normalized = 0,
        .absolute = 0,
        .reversed = false,
        .endpoint = {
            .left = 1250,
            .centre = 1500,
            .right = 1750,
        }
    },
    {   // THROTTLE
        .normalized = 0,
        .absolute = 0,
        .reversed = false,
        .endpoint = {
            .left = 1250,
            .centre = 1500,
            .right = 1750,
        }
    },
    {   // AUX
        .normalized = 0,
        .absolute = 0,
        .reversed = false,
        .endpoint = {
            .left = 1000,
            .centre = 1500,
            .right = 2000,
        }
    },
    {   // AUX2
        .normalized = 0,
        .absolute = 0,
        .reversed = false,
        .endpoint = {
            .left = 1000,
            .centre = 1500,
            .right = 2000,
        }
    },
    {   // AUX3
        .normalized = 0,
        .absolute = 0,
        .reversed = false,
        .endpoint = {
            .left = 1000,
            .centre = 1500,
            .right = 2000,
        }
    }
};

static uint32_t next_tick;


// ****************************************************************************
static void service_systick(void)
{
    uint32_t now = milliseconds;

    ++entropy;

    global_flags.systick = 0;
    if (now > next_tick) {
        next_tick += __SYSTICK_IN_MS;
        global_flags.systick = 1;
    }
}


// ****************************************************************************
static void check_no_signal(void)
{
    static uint16_t no_signal_timeout = 0;

    if (config.mode == STAND_ALONE) {
        global_flags.no_signal = false;
        return;
    }

    if (global_flags.new_channel_data) {
        global_flags.no_signal = false;
        no_signal_timeout = config.no_signal_timeout;
    }

    if (global_flags.systick) {
        --no_signal_timeout;
        if (no_signal_timeout == 0) {
            global_flags.no_signal = true;
        }
    }
}


// ****************************************************************************
static void output_channel_diagnostics(void)
{
    static bool send_config = true;

    if (global_flags.no_signal) {
        send_config = true;
    }

    if (global_flags.new_channel_data) {
        static int16_t st = 999;
        static int16_t th = 999;

        if (st != channel[ST].normalized  ||
            th != channel[TH].normalized) {

            st = channel[ST].normalized;
            th = channel[TH].normalized;

            fprintf(STDOUT_DEBUG, "ST: %4d  TH: %4d\n",
                channel[ST].normalized, channel[TH].normalized);
        }

        if (send_config) {
            send_config = false;

            /*
            Send AUX configuration values to allow the preprocessor-simulator
            to adjust its functionality automatically -- which is pretty
            important as otherwise it is easy to mess up with all the
            different AUX switch types

            We send the configuration once after a no_signal event, and after
            power on. In practice this means that the preprocessor-simulator
            always sees the configuration information immediately after it
            is talking to the light controller.
             */
            fprintf(STDOUT_DEBUG, "CONFIG %d %d %d %d %d %d %d %d %d\n",
                config.flags2.multi_aux,
                config.flags.ch3_is_momentary, config.flags.ch3_is_two_button,
                config.aux_type, config.aux_function,
                config.aux2_type, config.aux2_function,
                config.aux3_type, config.aux3_function);
        }
    }
}


// ****************************************************************************
int main(void)
{
    bool is_servo_reader;

    is_servo_reader = (config.mode == MASTER_WITH_SERVO_READER);

    global_flags.servo_output_enabled =
        config.flags.steering_wheel_servo_output ||
        config.flags.gearbox_servo_output;

    if (config.flags2.multi_aux) {
        global_flags.servo_output_enabled |=
            (config.aux_function == SERVO) ||
            (config.aux2_function == SERVO) ||
            (config.aux3_function == SERVO);
    }

    global_flags.uart_output_enabled =
        config.flags.slave_output ||
        config.flags.preprocessor_output ||
        config.flags.winch_output;

    if (config.mode != STAND_ALONE) {
        global_flags.no_signal = true;
        global_flags.initializing = true;
    }

    HAL_hardware_init(is_servo_reader, global_flags.servo_output_enabled, global_flags.uart_output_enabled);
    load_persistent_storage();
    HAL_uart_init(config.baudrate);
    init_printf(STDOUT, HAL_putc);

    // Wait for 100ms to have the supply settle down before initializing the
    // rest of the system. This is especially important for the TLC5940,
    // which misbehaves (certain LEDs don't work) when being addressed before
    // power is stable.
    //
    // It is also important in terms of servo reader function, because some
    // RC electronics like the Hobbywing Quicrun 1080 are outputing serial
    // data after startup, which interferes with the initialization procedure
    while (milliseconds <  100);

    init_servo_reader();
    init_uart_reader();
    init_servo_output();

    init_lights();
    HAL_hardware_init_final();

    next_tick = milliseconds + __SYSTICK_IN_MS;

    fprintf(STDOUT_DEBUG, "\n\n**********\nLight controller v%d\n", config.firmware_version);

    while (1) {
        global_flags.gear_changed = 0;
        if (global_flags.gear_change_requested) {
            global_flags.gear_change_requested = 0;
            global_flags.gear_changed = 1;
        }

        service_systick();

        global_flags.new_channel_data = false;
        read_all_servo_channels();
        read_preprocessor();
        process_aux();
        process_drive_mode();
        process_indicators();
        process_channel_reversing_setup();
        check_no_signal();
        process_shelf_queen_mode();

        process_servo_output();
        process_winch();
        process_lights();
        output_preprocessor();
        output_channel_diagnostics();

        HAL_service();
    }
}
