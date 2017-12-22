#include <stdint.h>
#include <stdbool.h>

#include <samd21.h>
#include <usb.h>
#include <flash_layout.h>
#include <printf.h>

#define ENABLE_UART_DIAGNOSTICS


typedef struct {
    uint8_t group;
    uint8_t pin;
    uint8_t mux;
} gpio_t;

static const gpio_t GPIO_USB_DM = { .group = 0, .pin = 24, .mux = PORT_PMUX_PMUXE_G_Val };
static const gpio_t GPIO_USB_DP = { .group = 0, .pin = 25, .mux = PORT_PMUX_PMUXE_G_Val };
// static const gpio_t GPIO_LED = { .group = 0, .pin = 19 };


// magic_value is a location in an unused RAM area that allows the application
// to communicate with the bootloader.
// The app writes a special value into this memory location which, when found
// by the bootloader after reboot, causes the bootloader to stay in firmware
// upgrade mode.
extern volatile uint32_t * const magic_value;
#define BOOTLOADER_MAGIC 0x47110815
#define DOUBLE_TAP_MAGIC 0x0d06f00d

// Global flag that is set by the DFU class when a firmware upgrade has finished
// and the MCU should be restarted.
volatile bool bootloader_done = false;

static volatile uint32_t milliseconds;


// ****************************************************************************
inline static void gpio_out(gpio_t gpio)
{
    PORT->Group[gpio.group].DIRSET.reg = 1 << gpio.pin;
}

// ****************************************************************************
inline static void gpio_set(gpio_t gpio)
{
    PORT->Group[gpio.group].OUTSET.reg = 1 << gpio.pin;
}


// ****************************************************************************
inline static void gpio_clear(gpio_t gpio)
{
    PORT->Group[gpio.group].OUTCLR.reg = 1 << gpio.pin;
}

// ****************************************************************************
inline static void gpio_mux(gpio_t gpio)
{
    if (gpio.pin & 1) {
        PORT->Group[gpio.group].PMUX[gpio.pin >> 1].bit.PMUXO = gpio.mux;
    }
    else {
        PORT->Group[gpio.group].PMUX[gpio.pin >> 1].bit.PMUXE = gpio.mux;
    }
    PORT->Group[gpio.group].PINCFG[gpio.pin].bit.PMUXEN = 1;
}


#ifdef ENABLE_UART_DIAGNOSTICS
static const gpio_t GPIO_TXD = { .group = 0, .pin = 4, .mux = PORT_PMUX_PMUXE_D_Val };

// ****************************************************************************
static void init_uart(uint32_t baudrate)
{
    #define UART_CLK 48000000

    uint64_t brr = (uint64_t)65536 * (UART_CLK - 16 * baudrate) / UART_CLK;

    gpio_out(GPIO_TXD);
    gpio_mux(GPIO_TXD);

    // Turn on power to the USART peripheral
    PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

    // Use GLKGEN0 (8 MHz) as clock source for the UART
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
        GCLK_CLKCTRL_GEN(0) |
        GCLK_CLKCTRL_CLKEN;

    // Run UART from GCLK; Setup Tx pad 0
    SERCOM0->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_DORD |
        SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
        SERCOM_USART_CTRLA_TXPO(0);

    // Enable transmit only; 8 bit characters
    SERCOM0->USART.CTRLB.reg =
        SERCOM_USART_CTRLB_TXEN |
        SERCOM_USART_CTRLB_CHSIZE(0);
    while (SERCOM0->USART.SYNCBUSY.reg);

    SERCOM0->USART.BAUD.reg = (uint16_t)brr;
    while (SERCOM0->USART.SYNCBUSY.reg);

    SERCOM0->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    while (SERCOM0->USART.SYNCBUSY.reg);
}


// ****************************************************************************
static void uart_putc(void *p, char c)
{
    (void) p;
    while (!(SERCOM0->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    SERCOM0->USART.DATA.reg = c;
}


// ****************************************************************************
static void print_app_diagnostics(void)
{
    uint32_t sp;
    uint32_t pc;

    sp = ((uint32_t *)FLASH_FIRMWARE_START)[0];
    pc = ((uint32_t *)FLASH_FIRMWARE_START)[1];

    printf("MSP             0x%08x\n", sp);
    printf("PC              0x%08x\n", pc);
}
#endif

// ****************************************************************************
static void init_systick(void)
{
    // Configure the SYSTICK to create an interrupt every 1 millisecond
    SysTick_Config(48000);
    NVIC_SetPriority(SysTick_IRQn, 0x0);

    __enable_irq();
}


// ****************************************************************************
void SysTick_Handler(void)
{
    ++milliseconds;
}


// ****************************************************************************
static void delay_ms(uint32_t value_ms) {
    uint32_t end_ms = milliseconds + value_ms;

    while (milliseconds < end_ms) {
        __WFI();
    }
}


// ****************************************************************************
static void init_clock(void)
{
    uint32_t *coarse_p;
    uint32_t coarse;

    // ------------------------------------------------
    // Since we intend to run at 48 MHz system clock, we neeed to conigure
    // one wait state for the flash according to the datasheet.
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_RWS(1);

    // ------------------------------------------------
    // Set up the PLL to provide a 48 MHz system clock
    SYSCTRL->INTFLAG.reg =
            SYSCTRL_INTFLAG_BOD33RDY |
            SYSCTRL_INTFLAG_BOD33DET |
            SYSCTRL_INTFLAG_DFLLRDY;

    SYSCTRL->DFLLCTRL.reg = 0;
    while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));

    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_MUL(48000);

    // Load PLL calibration values from NVRAM
    coarse_p = (uint32_t *)SYSCTRL_FUSES_DFLL48M_COARSE_CAL_ADDR;
    coarse = (*coarse_p & SYSCTRL_FUSES_DFLL48M_COARSE_CAL_Msk) >> SYSCTRL_FUSES_DFLL48M_COARSE_CAL_Pos;
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(512);

    SYSCTRL->DFLLCTRL.reg =
            SYSCTRL_DFLLCTRL_ENABLE |
            SYSCTRL_DFLLCTRL_USBCRM |
            SYSCTRL_DFLLCTRL_BPLCKC |
            SYSCTRL_DFLLCTRL_STABLE |
            SYSCTRL_DFLLCTRL_CCDIS;

    while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));


    // ------------------------------------------------
    // Reset the generic clock control block
    GCLK->CTRL.reg = GCLK_CTRL_SWRST;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

    // Setup GENCLK0 to run at 48 MHz. This clock is used for high speed
    // peripherals such as USB and UART
    GCLK->GENDIV.reg =
        GCLK_GENDIV_ID(0) |
        GCLK_GENDIV_DIV(1);

    GCLK->GENCTRL.reg =
            GCLK_GENCTRL_ID(0) |
            GCLK_GENCTRL_SRC_DFLL48M |
            GCLK_GENCTRL_RUNSTDBY |
            GCLK_GENCTRL_GENEN;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
}


// ****************************************************************************
static void init_usb(void)
{
    gpio_mux(GPIO_USB_DM);
    gpio_mux(GPIO_USB_DP);

    usb_init();
    usb_attach();
}


// ****************************************************************************
static inline void run_application(void)
{
    uint32_t msp;
    uint32_t reset_vector;

    // Disable all interrupts and the systick timer
    __disable_irq();
    SysTick->CTRL = 0;

    // Switch to the the interrupt vector table of the application
    SCB->VTOR = FLASH_FIRMWARE_START & SCB_VTOR_TBLOFF_Msk;

    // Set up the stack pointer
    msp = ((uint32_t *)FLASH_FIRMWARE_START)[0];
    __set_MSP(msp);

    // Jump to the application
    reset_vector = ((uint32_t *)FLASH_FIRMWARE_START)[1];
    __asm__ volatile("bx %0" :: "r" (reset_vector));
}


// ****************************************************************************
static bool application_is_present(void)
{
    uint32_t sp;
    uint32_t pc;
    uint32_t ram_start;
    uint32_t ram_end;
    uint32_t flash_start;
    uint32_t flash_end;

    sp = ((uint32_t *)FLASH_FIRMWARE_START)[0];
    pc = ((uint32_t *)FLASH_FIRMWARE_START)[1];
    ram_start = HMCRAMC0_ADDR;
    ram_end = HMCRAMC0_ADDR + HMCRAMC0_SIZE;
    flash_start = FLASH_ADDR;
    flash_end = FLASH_ADDR + FLASH_SIZE;

    // We examine the interrupt vector table.
    // The application is assumed to be present when the initial stack pointer
    // value points to a valid RAM location, and the reset vector points
    // to a flash memory location.

    return ((sp >= ram_start)  &&  (sp <= ram_end)  &&  (pc >= flash_start)  &&  (pc < flash_end));
}


// ****************************************************************************
static void bootloader(void)
{
    // Clear the value so that sub-sequent resets go back to load the
    // application.
    *magic_value = 0xffffffff;

    init_clock();
    init_systick();

#ifdef ENABLE_UART_DIAGNOSTICS
    init_uart(115200);
    init_printf(0, uart_putc);
    printf("\n\n**********\nRC Light Controller Bootloader\n");
    print_app_diagnostics();
#endif

    init_usb();

    while (!bootloader_done) {
        __WFI();
    }

#ifdef ENABLE_UART_DIAGNOSTICS
    printf("DFU completed, rebooting!\n");
#endif

    delay_ms(25);
    usb_detach();
    delay_ms(100);
    NVIC_SystemReset();
}


// ****************************************************************************
static bool bootloader_requested(void)
{
    // Check if the special RAM location contains a magic value that both
    // the application and the bootloader know. If it does, the bootloader
    // knows that the app has requested for a firmware upgrade.
    if (*magic_value == BOOTLOADER_MAGIC) {
        return true;
    }

    // Algorithm to allow the user to force the bootloader through
    // double-tapping of the reset pin.

    // If we are dealing with any reset cause other than through the external
    // reset pin we clear the magic value and launch the application.
    if (!PM->RCAUSE.bit.EXT) {
        *magic_value = 0xffffffff;
        return false;
    }

    // If the magic RAM location does not contain the special double-tap value,
    // it must be the first tap. We load our special value and then wait about
    // 100 ms.
    // If the user doesn't tap a second time during this waiting period then
    // we clear the magic RAM location and proceed with a normal application
    // boot.
    // If the user taps on the reset pin again while within the waiting period,
    // then the magic RAM location contains our special value when the MCU
    // reaches this code again, and triggers the bootloader.
    if (*magic_value != DOUBLE_TAP_MAGIC) {
        *magic_value = DOUBLE_TAP_MAGIC;

        // Timing loop of about 100 ms at the default system clock of 1 MHz
        for (uint32_t volatile i = 0; i < 25000; i++);

        *magic_value = 0xffffffff;
        return false;
    }

    // Second tap: activate the bootloader.
    return true;
}


// ****************************************************************************
int main(void)
{
    if (bootloader_requested()  ||  !application_is_present()) {
        bootloader();
    }

    run_application();
}