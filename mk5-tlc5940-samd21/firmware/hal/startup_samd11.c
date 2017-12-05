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
void TCC0_irq_handler(void) ALIAS(default_irq_handler);
void TC1_irq_handler(void) ALIAS(default_irq_handler);
void TC2_irq_handler(void) ALIAS(default_irq_handler);
void ADC_irq_handler(void) ALIAS(default_irq_handler);
void AC_irq_handler(void) ALIAS(default_irq_handler);
void DAC_irq_handler(void) ALIAS(default_irq_handler);
void PTC_irq_handler(void) ALIAS(default_irq_handler);

extern int main(void);


//-----------------------------------------------------------------------------
__attribute__ ((used, section(".isr_vectors")))
void (* const vectors[])(void) =
{
  // Cortex-M0+ handlers
    &_stacktop,             // The initial stack pointer
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

    // SAMD11 peripheral handlers
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
    TCC0_irq_handler,       // Timer/Counter for Control 0
    TC1_irq_handler,        // Timer/Counter 1
    TC2_irq_handler,        // Timer/Counter 2
    ADC_irq_handler,        // Analog-to-Digital Converter
    AC_irq_handler,         // Analog Comparator
    DAC_irq_handler,        // Digital-to-Analog Converter
    PTC_irq_handler,        // Peripheral Touch Controller
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


