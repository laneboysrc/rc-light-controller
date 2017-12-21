#include <stdint.h>
#include <stdbool.h>

#include <samd21.h>
#include <usb.h>
#include <flash_layout.h>
#include <printf.h>


#define UART_SERCOM SERCOM0
#define UART_SERCOM_GCLK_ID SERCOM0_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM0
#define UART_TXD_PMUX PORT_PMUX_PMUXE_D_Val
#define UART_RXD_PMUX PORT_PMUX_PMUXE_D_Val
#define UART_TXD_PAD 0      // PAD0


typedef struct {
    uint8_t bit;
    uint8_t port;
    uint8_t mux;
} gpio_t;


// These are all defined by the linker via the linker script.
extern uint32_t _ram;
extern uint32_t _eram;

extern uint32_t * const magic_value;

static const gpio_t GPIO_TXD = { .port = 0, .bit = 4, .mux = UART_TXD_PMUX };
static const gpio_t GPIO_USB_DM = { .port = 0, .bit = 24, .mux = PORT_PMUX_PMUXE_G_Val };
static const gpio_t GPIO_USB_DP = { .port = 0, .bit = 25, .mux = PORT_PMUX_PMUXE_G_Val };

bool bootloader_done = false;

static volatile uint32_t milliseconds;

// ****************************************************************************
inline static void gpio_out(gpio_t gpio)
{
    PORT->Group[gpio.port].DIRSET.reg = (1<<gpio.bit);
}


// ****************************************************************************
inline static void gpio_mux(gpio_t gpio)
{
    PORT->Group[gpio.port].PINCFG[gpio.bit].bit.PMUXEN = 1;
    if (gpio.bit & 1) {
        PORT->Group[gpio.port].PMUX[gpio.bit >> 1].bit.PMUXO = gpio.mux;
    }
    else {
        PORT->Group[gpio.port].PMUX[gpio.bit >> 1].bit.PMUXE = gpio.mux;
    }
}


// ****************************************************************************
static void init_uart(uint32_t baudrate)
{
    #define UART_CLK 48000000

    uint64_t brr = (uint64_t)65536 * (UART_CLK - 16 * baudrate) / UART_CLK;

    gpio_out(GPIO_TXD);
    gpio_mux(GPIO_TXD);

    // Turn on power to the USART peripheral
    PM->APBCMASK.reg |= UART_SERCOM_APBCMASK;

    // Use GLKGEN0 (8 MHz) as clock source for the UART
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(UART_SERCOM_GCLK_ID) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    // Run UART from GCLK; Setup Tx pad
    UART_SERCOM->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_DORD |
        SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
        SERCOM_USART_CTRLA_TXPO(UART_TXD_PAD);

    // Enable transmit only; 8 bit characters
    UART_SERCOM->USART.CTRLB.reg =
        SERCOM_USART_CTRLB_TXEN |
        SERCOM_USART_CTRLB_CHSIZE(0);           // 8 bits
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    UART_SERCOM->USART.BAUD.reg = (uint16_t)brr;
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    UART_SERCOM->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    while (UART_SERCOM->USART.SYNCBUSY.reg);
}


// ****************************************************************************
static void uart_putc(void *p, char c)
{
    (void) p;
    while (!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    UART_SERCOM->USART.DATA.reg = c;
}


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
static void run_application(void)
{
    uint32_t msp;
    uint32_t reset_vector;

    msp = ((uint32_t *)FLASH_FIRMWARE_START)[0];
    reset_vector = ((uint32_t *)FLASH_FIRMWARE_START)[1];

    // Disable all interrupts and the systick timer
    __disable_irq();
    SysTick->CTRL = 0;

    // Switch to the the interrupt vector table of the application
    SCB->VTOR = FLASH_FIRMWARE_START & SCB_VTOR_TBLOFF_Msk;

    // Set up the stack pointer
    __set_MSP(msp);

    // Jump to the application
    __asm__ volatile("bx %0" :: "r" (reset_vector));
}


// ****************************************************************************
static bool application_is_present(void)
{
    uint32_t sp;
    uint32_t pc;
    uint32_t ram;
    uint32_t ram_end;

    sp = ((uint32_t *)FLASH_FIRMWARE_START)[0];
    pc = ((uint32_t *)FLASH_FIRMWARE_START)[1];
    ram = HMCRAMC0_ADDR;
    ram_end = HMCRAMC0_ADDR + HMCRAMC0_SIZE;

    return ((sp >= ram)  &&  (sp <= ram_end)  &&  (pc < FLASH_SIZE));
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


// ****************************************************************************
static void bootloader(void)
{
    init_clock();
    init_systick();
    init_uart(115200);
    init_printf(0, uart_putc);
    init_usb();

    printf("\n\n**********\nRC Light Controller Bootloader\n");

    print_app_diagnostics();

    while (!bootloader_done) {
        if ((milliseconds % 1000) == 0) {
            printf("%u\n", milliseconds / 1000);
        }
        __WFI();
    }

    printf("DFU completed, rebooting!\n");
    delay_ms(25);
    usb_detach();
    delay_ms(100);
    NVIC_SystemReset();
}


// ****************************************************************************
static bool bootloader_requested(void)
{
    if (*magic_value == 0x47110815) {
        *magic_value = 0xffffffff;
        return true;
    }

    return false;
}


// ****************************************************************************
int main(void)
{
    if (bootloader_requested()  ||  !application_is_present()) {
        bootloader();
    }

    run_application();
}