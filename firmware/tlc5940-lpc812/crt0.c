#include <LPC8xx.h>

extern int main(void);

// ****************************************************************************
// Simple gcc-compatible C runtime initialization
//
// This works in conjunction with lpc81x.ld
// ****************************************************************************

// These are all defined by the linker via the lpc81x.ld linker script.
extern unsigned int _text;
extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;

static inline void crt0(void)
{
    unsigned int *source;
    unsigned int *destination;
    unsigned int *end;

    // Place stack canaries into RAM
    destination = (unsigned int *)(0x10000000);
    end = (unsigned int *)(0x10001000);
    while (destination < end) {
        *(destination++) = 0xcafebabe;
    }

    // Copy initialization values from Flash to RAM
    source = (unsigned int *)(&_etext);
    destination = (unsigned int *)(&_data);
    end = (unsigned int *)(&_edata);
    while (destination < end) {
        *(destination++) = *(source++);
    }

    // Zero out uninitialized RAM
    destination = (unsigned int *)(&_bss);
    end = (unsigned int *)(&_ebss);
    while (destination < end) {
        *(destination++) = 0;
    }

    main();
    while (1);  // Hang if main() returns
}


// ****************************************************************************
// Interrupt vectors
typedef void (*irq_handler)(void);

// This symbol is defined by the linker in the lpc81x.ld script
extern void _stacktop(void);

// Forward declaration of the exception handlers.
// When the application defines a handler with the same name it will
// take precedence over these "weak" definitions
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
void NMI_handler(void) ALIAS(default_irq_handler);
void HardFault_handler(void) ALIAS(default_irq_handler);
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

__attribute__ ((section(".isr_vector")))
irq_handler vectors[48] = {
    &_stacktop,             // The initial stack pointer
    crt0,                   // Reset handler
    NMI_handler,            // NMI handler
    HardFault_handler,	    // Hard fault handler
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

__attribute__ ((section(".after_vectors")))
static void default_irq_handler(void)
{
    while(1);
}
