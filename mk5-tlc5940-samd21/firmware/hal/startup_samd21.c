#include <stdint.h>

#include <samd21.h>

// The entropy variable is a true random number generated from the random RAM
// contents after power-up.
uint32_t entropy;


// magic_value is a location in an unused RAM area that allows the application
// to communicate with the bootloader.
// The app writes a special value into this memory location which, when found
// by the bootloader after reboot, causes the bootloader to stay in firmware
// upgrade mode.
//
// Note: this address must be aligned to 4 bytes
//
// Since this file is utilized by both the application and the bootloader,
// changing the address here updates both once recompiled.
uint32_t * const magic_value = (uint32_t *)0x20000cac;


// These are all defined by the linker via the linker script.
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
void Reset_Handler(void);
void SysTick_Handler(void) ALIAS(default_handler);

void PM_Handler(void) ALIAS(default_handler);
void SYSCTRL_Handler(void) ALIAS(default_handler);
void WTD_Handler(void) ALIAS(default_handler);
void RTC_Handler(void) ALIAS(default_handler);
void EIC_Handler(void) ALIAS(default_handler);
void NVMCTRL_Handler(void) ALIAS(default_handler);
void DMAC_Handler(void) ALIAS(default_handler);
void USB_Handler(void) ALIAS(default_handler);
void EVSYS_Handler(void) ALIAS(default_handler);
void SERCOM0_Handler(void) ALIAS(default_handler);
void SERCOM1_Handler(void) ALIAS(default_handler);
void SERCOM2_Handler(void) ALIAS(default_handler);
void SERCOM3_Handler(void) ALIAS(default_handler);
void SERCOM4_Handler(void) ALIAS(default_handler);
void SERCOM5_Handler(void) ALIAS(default_handler);
void TCC0_Handler(void) ALIAS(default_handler);
void TCC1_Handler(void) ALIAS(default_handler);
void TCC2_Handler(void) ALIAS(default_handler);
void TC3_Handler(void) ALIAS(default_handler);
void TC4_Handler(void) ALIAS(default_handler);
void TC5_Handler(void) ALIAS(default_handler);
void TC6_Handler(void) ALIAS(default_handler);
void TC7_Handler(void) ALIAS(default_handler);
void ADC_Handler(void) ALIAS(default_handler);
void AC_Handler(void) ALIAS(default_handler);
void DAC_Handler(void) ALIAS(default_handler);
void PTC_Handler(void) ALIAS(default_handler);
void I2S_Handler(void) ALIAS(default_handler);

extern int main(void);


//-----------------------------------------------------------------------------
__attribute__ ((used, section(".isr_vectors")))
void (* const vectors[])(void) =
{
    &_stacktop,             // The initial stack pointer

    // Cortex-M0+ handlers
    Reset_Handler,          // Reset handler
    NMI_Handler,            // NMI handler
    HardFault_Handler,      // Hard fault handler
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    0,                      // Reserved
    SVC_Handler,            // SVCall handler
    0,                      // Reserved
    0,                      // Reserved
    PendSV_Handler,         // PendSV handler
    SysTick_Handler,        // SysTick handler

    // SAMD21 peripheral handlers
    PM_Handler,         // Power Manager
    SYSCTRL_Handler,    // System Controller
    WTD_Handler,        // Watchdog Timer
    RTC_Handler,        // Real Time Counter
    EIC_Handler,        // External Interrupt Controller
    NVMCTRL_Handler,    // Non-Volatile Memory Controller
    DMAC_Handler,       // Direct Memory Access Controller
    USB_Handler,        // USB Controller
    EVSYS_Handler,      // Event System
    SERCOM0_Handler,    // Serial Communication Interface 0
    SERCOM1_Handler,    // Serial Communication Interface 1
    SERCOM2_Handler,    // Serial Communication Interface 2
    SERCOM3_Handler,    // Serial Communication Interface 3
    SERCOM4_Handler,    // Serial Communication Interface 4
    SERCOM5_Handler,    // Serial Communication Interface 5
    TCC0_Handler,       // Timer/Counter for Control 0
    TCC1_Handler,       // Timer/Counter for Control 1
    TCC2_Handler,       // Timer/Counter for Control 2
    TC3_Handler,        // Timer/Counter 3
    TC4_Handler,        // Timer/Counter 4
    TC5_Handler,        // Timer/Counter 5
    TC6_Handler,        // Timer/Counter 6
    TC7_Handler,        // Timer/Counter 7
    ADC_Handler,        // Analog-to-Digital Converter
    AC_Handler,         // Analog Comparator
    DAC_Handler,        // Digital-to-Analog Converter
    PTC_Handler,        // Peripheral Touch Controller
    I2S_Handler,        // Inter-IC Sound Interface
};


// ****************************************************************************
static void default_handler(void)
{
    // The default handler issues a breakpoint so we can easily spot it during
    // a debug session (i.e. we enabled an interrupt that we don't have
    // a handler for)
    __BKPT(0);
    while (1);
}


// ****************************************************************************
void NMI_Handler(void)
{
    __BKPT(14);
    while (1);
}

// ****************************************************************************
void HardFault_Handler(void)
{
    __BKPT(13);
    while (1);
}

// ****************************************************************************
void SVC_Handler(void)
{
    __BKPT(5);
    while (1);
}

// ****************************************************************************
void PendSV_Handler(void)
{
    __BKPT(2);
    while (1);
}


//-----------------------------------------------------------------------------
void Reset_Handler(void)
{
    unsigned int *source;
    unsigned int *destination;
    unsigned int *end;
    uint32_t temp_entropy;

    // Switch to the the interrupt vector table of the application
    //
    // We put this here to solve an issue we faced when debugging using GDB:
    // Originally the bootloader set VTOR before jumping to the application.
    // However, when GDB was used this code seem to have been bypassed and
    // the VTOR was still pointing to the one from the bootloader.
    //
    // The solution is to set VTOR here, and then everything works fine.
    //
    // Note that this file is shared between the application and the bootloader.
    // All it means is that we waste a bit of ROM space in the bootloader as
    // we set VTOR to the value it already is. Note that `vectors` points to
    // different flash locations for the bootloader and the application!
    SCB->VTOR = ((uint32_t) &vectors) & SCB_VTOR_TBLOFF_Msk;

    // Calculate the entropy random value from the RAM contents after reset
    temp_entropy = 0x0817;  // Just an arbitrary initialization value
    source = &_ram;
    end = (unsigned int *)__get_MSP();
    while (source < end) {
        temp_entropy ^= *(source++);
    }

#ifndef NODEBUG
#ifndef NO_STACK_CANARIES
    // Place stack canaries into RAM
    destination = &_ram;
    end = (unsigned int *)__get_MSP();
    while (destination < end) {
        *(destination++) = 0xcafebabe;
    }
#endif
#endif

    // FIXME: there seems to be a problem with initialization values?!?
    //        e.g. line coding after reset is wrong
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


