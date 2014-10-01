/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>
#include <uart0.h>

// ****************************************************************************
// IO pins: (LPC812 in TSSOP16 package)
//
// PIO0_0   (16, TDO, ISP-Rx)   Steering input / Rx
// PIO0_1   (9,  TDI)           TLC5940 GSCLK
// PIO0_2   (6,  TMS, SWDIO)    TLC5940 XLAT
// PIO0_3   (5,  TCK, SWCLK)    TLC5940 SCLK
// PIO0_4   (4,  TRST, ISP-Tx)  Throttle input / Tx
// PIO0_5   (3,  RESET)         NC (test point)
// PIO0_6   (15)                TLC5940 BLANK
// PIO0_7   (14)                TLC5940 SIN
// PIO0_8   (11, XTALIN)        NC
// PIO0_9   (10, XTALOUT)       WS2812b data out
// PIO0_10  (8,  Open drain)    NC
// PIO0_11  (7,  Open drain)    NC
// PIO0_12  (2,  ISP-entry)     Servo out / ISP
// PIO0_13  (1)                 CH3 input
//
// GND      (13)
// 3.3V     (12)
// ****************************************************************************


volatile uint32_t systick_count;

// The entropy variable is incremented every mainloop. It can therefore serve
// as a random value in practical RC car application,
// Certainly not suitable for secure implementations...
uint32_t entropy;

CHANNEL_T channel[3];
GLOBAL_FLAGS_T global_flags;


void SysTick_handler(void);


// ****************************************************************************
static void init_hardware(void)
{

#if __SYSTEM_CLOCK != 12000000
#error Clock initialization code expexts __SYSTEM_CLOCK to be set to 1200000
#endif
    // Set flash wait-states to 1 system clock
    LPC_FLASHCTRL->FLASHCFG = 0;

    // Turn on peripheral clocks for SCTimer, IOCON, SPI0
    // (GPIO, SWM alrady enabled after reset)
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 8) | (1 << 18) | (1 << 11);


    // ------------------------
    // IO configuration
    LPC_SWM->PINENABLE0 = 0xffffffbf;   // Enable reset, all other special functions disabled


    // Make the open drain ports PIO0_10, PIO0_11 outputs and pull to ground
    // to prevent them from floating.
    LPC_GPIO_PORT->W0[10] = 0;
    LPC_GPIO_PORT->W0[11] = 0;
    LPC_GPIO_PORT->DIR0 |= (1u << 10) | (1u << 11);


    // Enable glitch filtering on the IOs
    // GOTCHA: ICONCLKDIV0 is actually the last register in the array!
    LPC_SYSCON->IOCONCLKDIV[6] = 255;       // Glitch filter 0: Main clock divided by 255
    LPC_SYSCON->IOCONCLKDIV[5] = 1;         // Glitch filter 0: Main clock divided by 1

    // NOTE: for some reason it is absolutely necessary to enable glitch
    // filtering on the IOs used for the capture timer. One clock cytle of the
    // main clock is enough, but with none weird things happen.

    LPC_IOCON->PIO0_0 |= (1 << 5) |         // Enable Hysteresis
                         (0x1 << 13) |      // Glitch filter 1
                         (0x1 << 11);       // Reject 1 clock cycle of glitch filter

    LPC_IOCON->PIO0_4 |= (1 << 5) |         // Enable Hysteresis
                         (0x1 << 13) |      // Glitch filter 1
                         (0x1 << 11);       // Reject 1 clock cycle of glitch filter

    LPC_IOCON->PIO0_13 |= (1 << 5) |        // Enable Hysteresis
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


// ****************************************************************************
int main(void)
{
    init_hardware();
    init_uart0();
    load_persistent_storage();
    init_servo_reader();
    init_uart_reader();
    init_servo_output();
    init_lights();
    init_hardware_final();

    uart0_send_cstring("Light controller initialized\n");

    while (1) {
        service_systick();

        read_all_servo_channels();
        read_preprocessor();
        process_ch3_clicks();
        process_drive_mode();
        process_indicators();
        process_channel_reversing_setup();

        process_servo_output();
        process_winch();
        process_lights();
        output_preprocessor();

        //if (global_flags.systick) {
        //     uart0_send_cstring("tick\n");
        // }

        if (global_flags.new_channel_data) {
            static int16_t st = 999;
            static int16_t th = 999;

            // if (global_flags.blink_indicator_left) {
            //     uart0_send_cstring("blink left\n");
            // }
            // if (global_flags.blink_indicator_right) {
            //     uart0_send_cstring("blink right\n");
            // }
            // if (global_flags.blink_hazard) {
            //     uart0_send_cstring("hazard\n");
            // }

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
}
