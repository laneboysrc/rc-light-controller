#include <stdint.h>


// The entropy variable is a true random number generated from the random RAM
// contents after power-up.
uint32_t entropy;


// These are all defined by the linker via the samd11d14.ld linker script.
extern unsigned int _text;
extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;
extern unsigned int _ram;
extern unsigned int _eram;
extern void _stacktop(void);


// Forward declaration of the exception handlers.
// When the application defines a handler with the same name it will
// take precedence over these "weak" definitions
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
void Reset_handler(void);
void NMI_handler(void) ALIAS(default_irq_handler);
void HardFault_handler(void) ALIAS(default_irq_handler);
void SVCall_handler(void) ALIAS(default_irq_handler);
void PendSV_handler(void) ALIAS(default_irq_handler);
void SysTick_handler(void) ALIAS(default_irq_handler);

void PM_irq_handler(void) ALIAS(default_irq_handler);
void SYSCTRL_irq_handler(void) ALIAS(default_irq_handler);
void WTD_irq_handler(void) ALIAS(default_irq_handler);
void RTC_irq_handler(void) ALIAS(default_irq_handler);
void EIC_irq_handler(void) ALIAS(default_irq_handler);
void NVMCTRL_irq_handler(void) ALIAS(default_irq_handler);
void DMAC_irq_handler(void) ALIAS(default_irq_handler);
void USB_irq_handler(void) ALIAS(default_irq_handler);
void EVSYS_irq_handler(void) ALIAS(default_irq_handler);
void SERCOM0_irq_handler(void) ALIAS(default_irq_handler);
void SERCOM1_irq_handler(void) ALIAS(default_irq_handler);
void SERCOM2_irq_handler(void) ALIAS(default_irq_handler);
void SERCOM3_irq_handler(void) ALIAS(default_irq_handler);
void SERCOM4_irq_handler(void) ALIAS(default_irq_handler);
void SERCOM5_irq_handler(void) ALIAS(default_irq_handler);
void TCC0_irq_handler(void) ALIAS(default_irq_handler);
void TCC1_irq_handler(void) ALIAS(default_irq_handler);
void TCC2_irq_handler(void) ALIAS(default_irq_handler);
void TC3_irq_handler(void) ALIAS(default_irq_handler);
void TC4_irq_handler(void) ALIAS(default_irq_handler);
void TC5_irq_handler(void) ALIAS(default_irq_handler);
void TC6_irq_handler(void) ALIAS(default_irq_handler);
void TC7_irq_handler(void) ALIAS(default_irq_handler);
void ADC_irq_handler(void) ALIAS(default_irq_handler);
void AC_irq_handler(void) ALIAS(default_irq_handler);
void DAC_irq_handler(void) ALIAS(default_irq_handler);
void PTC_irq_handler(void) ALIAS(default_irq_handler);
void I2S_irq_handler(void) ALIAS(default_irq_handler);

extern int main(void);


//-----------------------------------------------------------------------------
__attribute__ ((used, section(".isr_vectors")))
void (* const vectors[])(void) =
{
    &_stacktop,             // The initial stack pointer

    // Cortex-M0+ handlers
    Reset_handler,          // Reset handler
    NMI_handler,            // NMI handler
    HardFault_handler,      // Hard fault handler
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    SVCall_handler,         // SVCall handler
    0,                      // Reserved
    0,                      // Reserved
    PendSV_handler,         // PendSV handler
    SysTick_handler,        // SysTick handler

    // SAMD21 peripheral handlers
    PM_irq_handler,         // Power Manager
    SYSCTRL_irq_handler,    // System Controller
    WTD_irq_handler,        // Watchdog Timer
    RTC_irq_handler,        // Real Time Counter
    EIC_irq_handler,        // External Interrupt Controller
    NVMCTRL_irq_handler,    // Non-Volatile Memory Controller
    DMAC_irq_handler,       // Direct Memory Access Controller
    USB_irq_handler,        // USB Controller
    EVSYS_irq_handler,      // Event System
    SERCOM0_irq_handler,    // Serial Communication Interface 0
    SERCOM1_irq_handler,    // Serial Communication Interface 1
    SERCOM2_irq_handler,    // Serial Communication Interface 2
    SERCOM3_irq_handler,    // Serial Communication Interface 3
    SERCOM4_irq_handler,    // Serial Communication Interface 4
    SERCOM5_irq_handler,    // Serial Communication Interface 5
    TCC0_irq_handler,       // Timer/Counter for Control 0
    TCC1_irq_handler,       // Timer/Counter for Control 1
    TCC2_irq_handler,       // Timer/Counter for Control 2
    TC3_irq_handler,        // Timer/Counter 3
    TC4_irq_handler,        // Timer/Counter 4
    TC5_irq_handler,        // Timer/Counter 5
    TC6_irq_handler,        // Timer/Counter 6
    TC7_irq_handler,        // Timer/Counter 7
    ADC_irq_handler,        // Analog-to-Digital Converter
    AC_irq_handler,         // Analog Comparator
    DAC_irq_handler,        // Digital-to-Analog Converter
    PTC_irq_handler,        // Peripheral Touch Controller
    I2S_irq_handler,        // Inter-IC Sound Interface
};

static void default_irq_handler(void)
{
    while(1);
}

//-----------------------------------------------------------------------------
void Reset_handler(void)
{
    unsigned int *source;
    unsigned int *destination;
    unsigned int *end;
    uint32_t temp_entropy;

    // Calculate the entropy random value from the RAM contents after reset
    temp_entropy = 0x0817;  // Just an arbitrary initialization value
    source = &_ram;
    end = &_eram;
    while (source < end) {
        temp_entropy ^= *(source++);
    }

#ifndef NODEBUG
    // Place stack canaries into RAM
    destination = (unsigned int *)(0x10000000);
    end = (unsigned int *)(0x10001000);
    while (destination < end) {
        *(destination++) = 0xcafebabe;
    }
#endif

    // Copy initialization values from Flash to RAM
    source = &_etext;
    destination = &_data;
    end = &_edata;
    while (destination < end) {
        *(destination++) = *(source++);
    }

    // Zero out uninitialized RAM
    destination = &_bss;
    end = &_ebss;
    while (destination < end) {
        *(destination++) = 0;
    }

    // Copy the calculated random value into the final destination RAM
    // (which we just erased above ...)
    entropy = temp_entropy;

    main();
    while (1);
}


