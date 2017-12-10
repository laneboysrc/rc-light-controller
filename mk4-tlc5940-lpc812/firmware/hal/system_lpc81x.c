#include <stdint.h>
#include <stdio.h>

#include <LPC8xx.h>
#include <hal.h>



#define __SYSTICK_IN_MS 20

void SysTick_handler(void);
void HardFault_handler(void);


volatile uint32_t milliseconds;

// These are all defined by the linker via the lpc81x.ld linker script.
extern unsigned int _ram;
extern unsigned int _stacktop;



// ****************************************************************************
void SysTick_handler(void)
{
    ++milliseconds;
}


// ****************************************************************************
void HardFault_handler(void)
{
    HAL_putc(NULL, 'H');
    HAL_putc(NULL, '\n');
    while (1);
}


// ****************************************************************************
void HAL_hardware_init(bool is_servo_reader, bool has_servo_output)
{
#if HAL_SYSTEM_CLOCK != 12000000
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
    if (is_servo_reader) {
        // Turn the UART output on unless a servo output is requested
        if (!has_servo_output) {
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
    HAL_gpio_switched_light_output_clear();
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
    // SysTick configuration 1000 Hz / 1 ms
    SysTick_Config((HAL_SYSTEM_CLOCK / 1000)- 1);
}


// ****************************************************************************
void HAL_hardware_init_final(void)
{
    // Turn off peripheral clock for IOCON and SWM to preserve power
    LPC_SYSCON->SYSAHBCLKCTRL &= ~((1 << 18) | (1 << 7));
}

// ****************************************************************************
uint32_t *HAL_stack_check(void)
{
    #define CANARY 0xcafebabe


    static uint32_t *last_found;
    uint32_t *now;

    if (last_found == NULL) {
        last_found = (uint32_t *)&_stacktop;
    }

    now = last_found;

    // for (int i = 0; i < 60; i++) {
    //     uart0_send_uint32_hex(*now--);
    //     uart0_send_linefeed();
    // }
    // uart0_send_linefeed();

    while (*now != CANARY  &&  now > (uint32_t *)&_ram) {
        --now;
    }

    if (now < last_found) {
        last_found = now;
        return now;
    }
    return NULL;
}

// ****************************************************************************
void HAL_service(void)
{
    ;
}