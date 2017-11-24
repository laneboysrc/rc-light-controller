/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <hal.h>

#include <globals.h>
#include <uart.h>


GLOBAL_FLAGS_T global_flags;

CHANNEL_T channel[3] = {
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
    {   // CH3 (AUX)
        .normalized = 0,
        .absolute = 0,
        .reversed = false,
        .endpoint = {
            .left = 1250,
            .centre = 1500,
            .right = 1750,
        }
    }
};

volatile uint32_t systick_count;
static bool diagnostics_output_enabled;

// ****************************************************************************
static void init_hardware(void)
{

    // Wait for 100ms to have the supply settle down before initializing the
    // rest of the system. This is especially important for the TLC5940,
    // which misbehaves (certain LEDs don't work) when being addressed before
    // power is stable.
    while (systick_count <  (100 / __SYSTICK_IN_MS));
    systick_count = 0;
}




// ****************************************************************************
static void service_systick(void)
{
    ++entropy;

    if (!systick_count) {
        global_flags.systick = 0;
        return;
    }

    global_flags.systick = 1;
    --systick_count;

    // // Disable the SysTick interrupt. Use memory barriers to ensure that no
    // // interrupt is pending in the pipeline.
    // // More info:
    // // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHHFHJD.html
    // SysTick->CTRL &= ~(1 << 1);
    // __DSB();
    // __ISB();
    // --systick_count;
    // SysTick->CTRL |= (1 << 1);      // Re-enable the system tick interrupt
}



// ****************************************************************************
static void check_no_signal(void)
{
    static uint16_t no_signal_timeout = 0;

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
// This function returns TRUE if it is ok for the light controller to output
// human-readable diagnostics messages on the serial port.
// (i.e. if the UART output is not used by another function like slave output,
// preprocessor output, or winch output)
// ****************************************************************************
bool diagnostics_enabled(void)
{
    return diagnostics_output_enabled;
}


// ****************************************************************************
int main(void)
{
    bool is_servo_reader = (config.mode == MASTER_WITH_SERVO_READER);

    global_flags.servo_output_enabled =
        config.flags.steering_wheel_servo_output ||
        config.flags.gearbox_servo_output;

    global_flags.no_signal = true;

    diagnostics_output_enabled = true;
    if (config.flags.slave_output ||
            config.flags.preprocessor_output ||
            config.flags.winch_output) {
        diagnostics_output_enabled = false;
    }
    if (config.mode == MASTER_WITH_SERVO_READER) {
        if (config.flags.steering_wheel_servo_output ||
                config.flags.gearbox_servo_output) {
            diagnostics_output_enabled = false;
        }
    }

    hal_hardware_init(is_servo_reader, global_flags.servo_output_enabled);
    init_hardware();
    load_persistent_storage();
    uart_init();
    init_servo_reader();
    init_uart_reader();
    init_servo_output();
    init_lights();
    hal_hardware_init_final();

    if (diagnostics_enabled()) {
        uart0_send_cstring("Light controller initialized\n");
    }

    while (1) {
        service_systick();

        read_all_servo_channels();
        read_preprocessor();
        process_ch3_clicks();
        process_drive_mode();
        process_indicators();
        process_channel_reversing_setup();
        check_no_signal();

        process_servo_output();
        process_winch();
        process_lights();
        output_preprocessor();

        if (diagnostics_enabled()) {
            if (global_flags.new_channel_data) {
                static int16_t st = 999;
                static int16_t th = 999;

                if (st != channel[ST].normalized  ||
                    th != channel[TH].normalized) {

                   st = channel[ST].normalized;
                   th = channel[TH].normalized;

                   uart0_send_cstring("ST: ");
                   uart0_send_int32(channel[ST].normalized);
                   uart0_send_cstring("   TH: ");
                   uart0_send_int32(channel[TH].normalized);
                   uart0_send_linefeed();
                }
            }
        }

#ifndef NODEBUG
    if (diagnostics_enabled()) {
        uint32_t *now = hal_stack_check();
        if (now) {
            uart0_send_cstring("Stack down to 0x");
            uart0_send_uint32_hex((uint32_t)now);
            uart0_send_linefeed();
        }
    }
#endif
    }
}
