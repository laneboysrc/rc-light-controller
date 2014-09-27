#include <stdio.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>
#include <uart0.h>


volatile uint32_t systick_count;

// The entropy variable is incremented every mainloop. It can therefore serve
// as a random value in practical RC car application,
// Certainly not suitable for secure implementations...
uint32_t entropy;

struct channel_s channel[3];
uint16_t light_mode;
GLOBAL_FLAGS_T global_flags;

// FIXME: make baudrate configurable

// FIXME: not needed, rather strive for 1 binary that can handle everything!
// ****************************************************************************
// Declare a couple of "weak" functions that are overloaded if certain modules
// are linked in. This allows us to configure the light controller by
// just linking (or omitting) various object files to achieve optional
// functionality without having to resort to a mess with #defines
//
// __attribute__ is a GNU specific extension though.
//
// NOTE: one potential issue with this will be that if you misspell the
// function in the other file it will get undetected at compile time!
//
void process_lights(void) __attribute__ ((weak, alias ("dummy_function")));

void dummy_function(void)
{
    ;
}


// ****************************************************************************
void init_hardware()
{

#if __SYSTEM_CLOCK != 12000000
#error Clock initialization code expexts __SYSTEM_CLOCK to be set to 1200000
#endif
    // Set flash wait-states to 1 system clock
    LPC_FLASHCTRL->FLASHCFG = 0;

    // Turn on peripheral clocks for IOCON (GPIO, SWM alrady enabled after reset)
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 18);

    LPC_SWM->PINENABLE0 = 0xffffffbf;   // Enable reset, all other special functions disabled

    LPC_SWM->PINASSIGN0 = 0xffff0004;   // U0_TXT_O=pio4, U0_RXD_I=pio0
    LPC_SWM->PINASSIGN6 = 0x02030303;   // SCT CTOUT_0 at PIO0.2, CTIN_3 at PIO0.3, CTIN_2 at PIO0.3, CTIN_1 at PIO0.3

    LPC_GPIO_PORT->DIR0 |= (1 << 2);    // Make PIO0_2 an output

    // Make open-drain pins PIO0_10 and PIO0_11 outputs and set them to "0"
    // to prevent them from floating internally. See UM10601 section 6.3
    LPC_GPIO_PORT->DIR0 |= (1 << 10) | (1 << 11);
    LPC_GPIO_PORT->CLR0 = (1 << 10) | (1 << 11);

    // Enable glitch filtering on the IOs
    // GOTCHA: ICONCLKDIV0 is actually the last register in the array!
    LPC_SYSCON->IOCONCLKDIV[6] = 255;       // Glitch filter 0: Main clock divided by 255
    LPC_SYSCON->IOCONCLKDIV[5] = 1;         // Glitch filter 0: Main clock divided by 1

    LPC_IOCON->PIO0_1 |= (1 << 5) |         // Enable Hysteresis
                         (0x0 << 13) |      // Glitch filter 0
                         (0x3 << 11);       // Reject 3 clock cycles of glitch filter

    // NOTE: for some reason it is absolutely necessary to enable glitch
    // filtering on the IOs used for the capture timer. One clock cytle of the
    // main clock is enough, but with none weird things happen.

    LPC_IOCON->PIO0_3 |= (1 << 5) |         // Enable Hysteresis
                         (0x1 << 13) |      // Glitch filter 1
                         (0x1 << 11);       // Reject 1 clock cycle of glitch filter



    // Turn on peripheral clock for SCTimer
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 8);

    LPC_SCT->CONFIG = (1 << 18);    // Auto-limit on counter H
    LPC_SCT->CTRL_L |= (1 << 3) |   // Clear the counter L
                       (5 << 5);    // PRE_L[12:5] = 6-1 (SCTimer L clock 2 MHz)
    LPC_SCT->CTRL_H |= (1 << 3) |   // Clear the counter H
                       (11 << 5);   // PRE_H[12:5] = 12-1 (SCTimer H clock 1 MHz)

#if __SYSTICK_IN_MS != 20
#error  Code expexts __SYSTICK_IN_MS to be set to 20
#endif
    LPC_SCT->MATCHREL[0].H = (1000000 / 50) - 1; // 50 Hz (20ms per overflow)

    // SCTimer H is soft timer, trigger interrupt on reload
    LPC_SCT->EVENT[0].STATE = 0xFFFF;       // Event 0 happens in all states
    LPC_SCT->EVENT[0].CTRL = (0 << 0) |     // Match event 0
                             (1 << 4) |     // Select H counter
                             (0x1 << 12);   // Match condition only

    LPC_SCT->CTRL_L &= ~(1 << 2);           // Start the SCTimer L
    LPC_SCT->CTRL_H &= ~(1 << 2);           // Start the SCTimer H



    // Turn off peripheral clock for IOCON and SWM to preserve power
    LPC_SYSCON->SYSAHBCLKCTRL &= ~((1 << 18) | (1 << 7));

    NVIC_EnableIRQ(SCT_IRQn);
}


// ****************************************************************************
void SCT_irq_handler(void)
{
    // Event 0: Match (reload) event every 20 ms (SCTimer H)
    if (LPC_SCT->EVFLAG & (1 << 0)) {
        LPC_SCT->EVFLAG = (1 << 0);
        ++systick_count;
    }

    servo_reader_SCT_interrupt_handler();
}


// ****************************************************************************
void service_systick(void)
{
    ++entropy;

    if (!systick_count) {
        global_flags.systick = 0;
        return;
    }

    global_flags.systick = 1;

    // Disable the SCTimer interrupt. Use memory barriers to ensure that no
    // interrupt is pending in the pipeline.
    // More info:
    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHHFHJD.html
    NVIC_DisableIRQ(SCT_IRQn);
    __DSB();
    __ISB();
    --systick_count;
    NVIC_EnableIRQ(SCT_IRQn);
}


// ****************************************************************************
int main(void)
{
    init_hardware();
    init_uart0();
    // init_persistent_data_storage();  // FIXME
    init_servo_reader();
    //init_lights();

    while (1) {
        service_systick();

        // FIXME: uart_servo and servo_reader are auto-detect!
        // Use rc-sound-module as reference
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
    }
}
