/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>
#include <uart0.h>


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

static volatile uint32_t systick_count;
static bool diagnostics_output_enabled;

// ****************************************************************************
static void init_hardware(void)
{

#if __SYSTEM_CLOCK != 12000000
#error Clock initialization code expexts __SYSTEM_CLOCK to be set to 1200000
#endif

    // Turn on brown-out detection and reset
    LPC_SYSCON->BODCTRL = (1 << 4) | (1 << 2) | (3 << 0);


    // Set flash wait-states to 1 system clock
    LPC_FLASHCTRL->FLASHCFG = 0;


    // Turn on peripheral clocks for SCTimer, IOCON, SPI0
    // (GPIO, SWM alrady enabled after reset)
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 8) | (1 << 18) | (1 << 11);


    // ------------------------
    // IO configuration

    // Enable reset, all other special functions disabled
    LPC_SWM->PINENABLE0 = 0xffffffbf;

    // Configure the UART input and output
    diagnostics_output_enabled = true;
    if (config.flags.slave_output || config.flags.preprocessor_output ||
            config.flags.winch_output) {
        diagnostics_output_enabled = false;
    }
    if (config.mode == MASTER_WITH_SERVO_READER) {
        // Turn the UART output on unless a servo output is requested
        if (config.flags.steering_wheel_servo_output ||
                config.flags.gearbox_servo_output) {
            diagnostics_output_enabled = false;
        }
        else {
            // U0_TXT_O=PIO0_12
            LPC_SWM->PINASSIGN0 = (0xff << 24) |
                                  (0xff << 16) |
                                  (0xff << 8) |
                                  (GPIO_BIT_OUT << 0);
        }
    }
    else {
        // U0_TXT_O=PIO0_4 (TH), U0_RXD_I=PIO0_0 (ST)
        LPC_SWM->PINASSIGN0 = (0xff << 24) |
                              (0xff << 16) |
                              (GPIO_BIT_ST << 8) |
                              (GPIO_BIT_TH << 0);
    }

    // Make the open drain ports PIO0_10, PIO0_11 outputs and pull to ground
    // to prevent them from floating.
    // Make the switched light output PIO0_9 an output and shut it off.
    GPIO_SWITCHED_LIGHT_OUTPUT = 0;
    LPC_GPIO_PORT->W0[10] = 0;
    LPC_GPIO_PORT->W0[11] = 0;
    LPC_GPIO_PORT->DIR0 |= (1 << 10) | (1 << 11) |
                           (1 << GPIO_BIT_SWITCHED_LIGHT_OUTPUT);


    // Enable glitch filtering on the IOs
    // GOTCHA: ICONCLKDIV0 is actually the last register in the array!
    LPC_SYSCON->IOCONCLKDIV[6] = 255;       // Glitch filter 0: Main clock divided by 255
    LPC_SYSCON->IOCONCLKDIV[5] = 1;         // Glitch filter 1: Main clock divided by 1

    // NOTE: for some reason it is absolutely necessary to enable glitch
    // filtering on the IOs used for the capture timer. One clock cytle of the
    // main clock is enough, but with none weird things happen.

    GPIO_IOCON_ST |= (1 << 5) |         // Enable Hysteresis
                     (0x1 << 13) |      // Glitch filter 1
                     (0x1 << 11);       // Reject 1 clock cycle of glitch filter

    GPIO_IOCON_TH |= (1 << 5) |         // Enable Hysteresis
                     (0x1 << 13) |      // Glitch filter 1
                     (0x1 << 11);       // Reject 1 clock cycle of glitch filter

    GPIO_IOCON_CH3 |= (1 << 5) |        // Enable Hysteresis
                      (0x1 << 13) |     // Glitch filter 1
                      (0x1 << 11);      // Reject 1 clock cycle of glitch filter


    // ------------------------
    // Configure SCTimer globally for two 16-bit counters
    LPC_SCT->CONFIG = 0;


    // ------------------------
    // SysTick configuration
    SysTick->LOAD = __SYSTEM_CLOCK * __SYSTICK_IN_MS / 1000;
    SysTick->VAL = __SYSTEM_CLOCK * __SYSTICK_IN_MS / 1000;
    SysTick->CTRL = (1 << 0) |              // Enable System Tick counter
                    (1 << 1) |              // System Tick interrupt enable
                    (1 << 2);               // Use system clock


    // Wait for 100ms to have the supply settle down before initializing the
    // rest of the system. This is especially important for the TLC5940,
    // which misbehaves (certain LEDs don't work) when being addressed before
    // power is stable.
    while (systick_count <  (100 / __SYSTICK_IN_MS));
    systick_count = 0;
}


// ****************************************************************************
static void init_hardware_final(void)
{
    // Turn off peripheral clock for IOCON and SWM to preserve power
    LPC_SYSCON->SYSAHBCLKCTRL &= ~((1 << 18) | (1 << 7));
}


// ****************************************************************************
void SysTick_handler(void)
{
    if (SysTick->CTRL & (1 << 16)) {       // Read and clear Countflag
        ++systick_count;
    }
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

    // Disable the SysTick interrupt. Use memory barriers to ensure that no
    // interrupt is pending in the pipeline.
    // More info:
    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHHFHJD.html
    SysTick->CTRL &= ~(1 << 1);
    __DSB();
    __ISB();
    --systick_count;
    SysTick->CTRL |= (1 << 1);      // Re-enable the system tick interrupt
}


#ifndef NODEBUG
// ****************************************************************************
static void stack_check(void)
{
    #define CANARY 0xcafebabe

    static uint32_t *last_found = (uint32_t *)(0x10001000 - 48);
    uint32_t *now;


    if (!diagnostics_enabled()) {
        return;
    }


    if (last_found == (uint32_t *)0x10000000) {
        return;
    }

    now = last_found;
    while (*now != CANARY && now > (uint32_t *)0x10000000) {
        --now;
    }

    if (now != last_found) {
        last_found = now;
        uart0_send_cstring("Stack down to 0x");
        uart0_send_uint32_hex((uint32_t)now);
        uart0_send_linefeed();
    }
}
#endif


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
    global_flags.no_signal = true;
    init_hardware();
    init_uart0();
    load_persistent_storage();
    init_servo_reader();
    init_uart_reader();
    init_servo_output();
    init_lights();
    init_hardware_final();

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
        stack_check();
#endif
    }
}
