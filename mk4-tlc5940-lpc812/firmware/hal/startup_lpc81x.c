#include <LPC8xx.h>


// The entropy variable is a true random number generated from the random RAM
// contents after power-up.
uint32_t entropy;


// These are all defined by the linker via the lpc81x.ld linker script.
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
// void HardFault_handler(void) ALIAS(default_irq_handler);
void SVCall_handler(void) ALIAS(default_irq_handler);
void PendSV_handler(void) ALIAS(default_irq_handler);
void SysTick_handler(void) ALIAS(default_irq_handler);
void SPI0_irq_handler(void) ALIAS(default_irq_handler);
void SPI1_irq_handler(void) ALIAS(default_irq_handler);
void UART0_irq_handler(void) ALIAS(default_irq_handler);
void UART1_irq_handler(void) ALIAS(default_irq_handler);
void UART2_irq_handler(void) ALIAS(default_irq_handler);
void I2C_irq_handler(void) ALIAS(default_irq_handler);
void SCT_irq_handler(void) ALIAS(default_irq_handler);
void MRT_irq_handler(void) ALIAS(default_irq_handler);
void CMP_irq_handler(void) ALIAS(default_irq_handler);
void WDT_irq_handler(void) ALIAS(default_irq_handler);
void BOD_irq_handler(void) ALIAS(default_irq_handler);
void WKT_irq_handler(void) ALIAS(default_irq_handler);
void PININT0_irq_handler(void) ALIAS(default_irq_handler);
void PININT1_irq_handler(void) ALIAS(default_irq_handler);
void PININT2_irq_handler(void) ALIAS(default_irq_handler);
void PININT3_irq_handler(void) ALIAS(default_irq_handler);
void PININT4_irq_handler(void) ALIAS(default_irq_handler);
void PININT5_irq_handler(void) ALIAS(default_irq_handler);
void PININT6_irq_handler(void) ALIAS(default_irq_handler);
void PININT7_irq_handler(void) ALIAS(default_irq_handler);

extern int main(void);

extern void uart0_send_cstring(const char *);
static void HardFault_handler(void)
{
    uart0_send_cstring("HARD\n");
    while (1);
}

// ****************************************************************************
// Interrupt vectors
__attribute__ ((section(".isr_vectors")))
void (* const vectors[])(void) = {
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

    // LPC81x specific vectors
    SPI0_irq_handler,       // SPI0
    SPI1_irq_handler,       // SPI1
    0,                      // Reserved
    UART0_irq_handler,      // UART0
    UART1_irq_handler,      // UART1
    UART2_irq_handler,      // UART2
    0,                      // Reserved
    0,                      // Reserved
    I2C_irq_handler,        // I2C
    SCT_irq_handler,        // SCTimer
    MRT_irq_handler,        // Multi-rate timer
    CMP_irq_handler,        // Comparator
    WDT_irq_handler,        // Watchdog
    BOD_irq_handler,        // Brown Out Detect
    0,                      // Reserved
    WKT_irq_handler,        // Wakeup timer
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    PININT0_irq_handler,    // PIO INT0
    PININT1_irq_handler,    // PIO INT1
    PININT2_irq_handler,    // PIO INT2
    PININT3_irq_handler,    // PIO INT3
    PININT4_irq_handler,    // PIO INT4
    PININT5_irq_handler,    // PIO INT5
    PININT6_irq_handler,    // PIO INT6
    PININT7_irq_handler,    // PIO INT7
};

static void default_irq_handler(void)
{
    while (1);
}


// ****************************************************************************
// Simple gcc-compatible C runtime initialization
//
// This works in conjunction with lpc81x.ld
// ****************************************************************************
void Reset_handler(void)
{
    unsigned int *source;
    unsigned int *destination;
    unsigned int *end;
    uint32_t temp_entropy;

    // Calculate the entropy random value from the RAM contents after reset
    // Only process RAM locations unitl we reach the stack.
    temp_entropy = 0x0817;  // Just an arbitrary initialization value
    source = &_ram;
    end = (unsigned int *)__get_MSP();
    while (source < end) {
        temp_entropy ^= *(source++);
    }

#ifndef NODEBUG
    // Place stack canaries into RAM, but only up to the current stack pointer
    destination = &_ram;
    end = (unsigned int *)__get_MSP();
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
    while (1);  // Hang if main() returns
}


