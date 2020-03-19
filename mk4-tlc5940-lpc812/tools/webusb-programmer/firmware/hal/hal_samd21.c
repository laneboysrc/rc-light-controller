#include <stdint.h>
#include <stdio.h>

#include <globals.h>

#include <hal.h>
#include <usb.h>
#include <printf.h>

void add_uint8_to_uart_receive_buffer(uint8_t byte);
void add_uint8_to_usb_receive_buffer(uint8_t byte);

extern bool test_interface_is_write_busy(void);
extern void test_interface_write(uint8_t *data, uint8_t length);

volatile uint32_t milliseconds;

// These are defined by the linker via the samd21e15.ld linker script.
extern uint32_t _ram;
extern uint32_t _stacktop;

// magic_value is a location in an unused RAM area that allows the application
// to communicate with the bootloader.
// The app writes a special value into this memory location which, when found
// by the bootloader after reboot, causes the bootloader to stay in firmware
// upgrade mode.
extern uint32_t * const magic_value;
#define BOOTLOADER_MAGIC 0x47110815
bool start_bootloader = false;


#define RECEIVE_BUFFER_SIZE (128)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)
static uint8_t uart_receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t uart_read_index = 0;
static volatile uint16_t uart_write_index = 0;
static uint8_t usb_receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t usb_read_index = 0;
static volatile uint16_t usb_write_index = 0;

#define SEND_BUFFER_SIZE (64)
// static uint8_t uart_send_buffer[SEND_BUFFER_SIZE];
// static volatile uint16_t uart_send_buffer_index = 0;
static uint8_t usb_send_buffer[SEND_BUFFER_SIZE];
static volatile uint16_t usb_send_buffer_index = 0;

// We use all 24 bits of TCC0, so the maximum period value is 0xffffff
#define TCC0_PERIOD (0xfffffful)



// ****************************************************************************
void HAL_hardware_init(void)
{
    uint32_t *coarse_p;
    uint32_t coarse;


    // ------------------------------------------------
    // Perform GPIO initialization as early as possible
    HAL_gpio_set(HAL_GPIO_POWER_ENABLE);
    HAL_gpio_out(HAL_GPIO_POWER_ENABLE);

    HAL_gpio_in(HAL_GPIO_OUT_ISP);

    HAL_gpio_pmuxen(HAL_GPIO_USB_DM);
    HAL_gpio_pmuxen(HAL_GPIO_USB_DP);

    // Turn the status LED on to signify we are initializing
    HAL_gpio_out(HAL_GPIO_LED_OK);
    HAL_gpio_out(HAL_GPIO_LED_BUSY);
    HAL_gpio_out(HAL_GPIO_LED_ERROR);
    HAL_gpio_set(HAL_GPIO_LED_OK);
    HAL_gpio_set(HAL_GPIO_LED_BUSY);
    HAL_gpio_set(HAL_GPIO_LED_ERROR);

    // Configure GPIOs for the UART
    HAL_gpio_out(HAL_GPIO_TX);
    HAL_gpio_pmuxen(HAL_GPIO_TX);

    HAL_gpio_in(HAL_GPIO_RX);
    HAL_gpio_pmuxen(HAL_GPIO_RX);


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
    coarse_p = (uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR;
    coarse = (*coarse_p & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
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
    // peripherals such as SPI, USB and UART
    GCLK->GENDIV.reg =
        GCLK_GENDIV_ID(0) |
        GCLK_GENDIV_DIV(1);

    GCLK->GENCTRL.reg =
            GCLK_GENCTRL_ID(0) |
            GCLK_GENCTRL_SRC_DFLL48M |
            GCLK_GENCTRL_RUNSTDBY |
            GCLK_GENCTRL_GENEN;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

    // Setup GENCLK1 to run at 2 MHz. We use GENCLK1 for timers that need
    // microsecond resolution (e.g. servo output and servo reader).
    // We derive the clock from the 48 MHz PLL, so we use a clock divider of
    // 24
    GCLK->GENDIV.reg =
        GCLK_GENDIV_ID(1) |
        GCLK_GENDIV_DIV(24);

    GCLK->GENCTRL.reg =
            GCLK_GENCTRL_ID(1) |
            GCLK_GENCTRL_SRC_DFLL48M |
            GCLK_GENCTRL_RUNSTDBY |
            GCLK_GENCTRL_GENEN;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);


    // ------------------------------------------------
    // Turn on power to the peripherals we use
    PM->APBCMASK.reg =
        UART_SERCOM_APBCMASK |
        PM_APBAMASK_EIC |
        PM_APBCMASK_EVSYS |
        PM_APBCMASK_TCC0 |
        PM_APBCMASK_TC4 |
        PM_APBBMASK_USB;


    // ------------------------------------------------
    // Initialize the Event System

    // Use GLKGEN0 (48 MHz) as clock source for the EVSYS0..2
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID_EVSYS_0 |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID_EVSYS_1 |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID_EVSYS_2 |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    // Always turn on the Generic Clock within EVSYS
    EVSYS->CTRL.reg = EVSYS_CTRL_GCLKREQ;


    // ------------------------------------------------
    // Configure the SYSTICK to create an interrupt every 1 millisecond
    SysTick_Config(48000);
    NVIC_SetPriority(SysTick_IRQn, 0);

    __enable_irq();
}


// ****************************************************************************
void SysTick_Handler(void)
{
    ++milliseconds;
}


// ****************************************************************************
void HAL_hardware_init_final(void)
{
    usb_init();
    usb_attach();
}


// ****************************************************************************
void HAL_service(void)
{
    if (start_bootloader) {
        uint32_t end = milliseconds + 50;

        // fprintf(STDOUT_DEBUG, "REBOOTING INTO BOOTLOADER\n");

        // Wait a short while ...
        while (milliseconds < end);

        usb_detach();

        // Place a special value into a certain RAM location that is detected
        // by the bootloader, so that it enters firmware upgrade mode
        *magic_value = BOOTLOADER_MAGIC;

        // Disable the interrupts as they are apparently not cleared by a
        // system reset.
        NVIC_DisableIRQ(UART_SERCOM_IRQN);
        NVIC_DisableIRQ(USB_IRQn);
        NVIC_DisableIRQ(TCC0_IRQn);

        NVIC_SystemReset();
    }

    // HAL_gpio_clear(HAL_GPIO_LED);
    // HAL_gpio_clear(HAL_GPIO_LED2);


    // If we have data to send via USB serial, and USB serial is available to
    // send, then do it
    if (usb_send_buffer_index) {
        if (!test_interface_is_write_busy()) {
            test_interface_write(usb_send_buffer, usb_send_buffer_index);
            usb_send_buffer_index = 0;
        }
    }
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



// ****************************************************************************
void HAL_uart_init(uint32_t baudrate)
{
    #define UART_CLK 48000000

    uint64_t brr = (uint64_t)65536 * (UART_CLK - 16 * baudrate) / UART_CLK;

    // Use GLKGEN0 (48 MHz) as clock source for the UART
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(UART_SERCOM_GCLK_ID) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    // Run UART from GCLK; Setup Rx and Tx pads
    UART_SERCOM->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_DORD |
        SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
        SERCOM_USART_CTRLA_RXPO(HAL_GPIO_RX.rxpo) |
        SERCOM_USART_CTRLA_TXPO(HAL_GPIO_TX.txpo);

    // Enable transmit and receive; 8 bit characters
    UART_SERCOM->USART.CTRLB.reg =
        SERCOM_USART_CTRLB_RXEN |
        SERCOM_USART_CTRLB_TXEN |
        SERCOM_USART_CTRLB_CHSIZE(0);           // 8 bits
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    UART_SERCOM->USART.BAUD.reg = (uint16_t)brr;
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    UART_SERCOM->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    // Enable the receive interrupt
    UART_SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    NVIC_EnableIRQ(UART_SERCOM_IRQN);
}


// ****************************************************************************
void UART_SERCOM_HANDLER(void)
{
    if (UART_SERCOM->USART.INTFLAG.bit.RXC) {
        add_uint8_to_uart_receive_buffer((uint8_t)UART_SERCOM->USART.DATA.reg);
    }
}


// ****************************************************************************
static void usbserial_putc(char c) {
    if (usb_send_buffer_index < SEND_BUFFER_SIZE) {
        usb_send_buffer[usb_send_buffer_index++] = c;
    }
}


// ****************************************************************************
static bool usbserial_getchar_pending(void)
{
    return (usb_read_index != usb_write_index);
}


// ****************************************************************************
static uint8_t usbserial_getchar(void) {
    uint8_t data;

    while (!usbserial_getchar_pending());

    data = usb_receive_buffer[usb_read_index++];

    // Wrap around the read pointer.
    usb_read_index &= RECEIVE_BUFFER_INDEX_MASK;

    return data;
}


// ****************************************************************************
void add_uint8_to_usb_receive_buffer(uint8_t byte)
{
    usb_receive_buffer[usb_write_index++] = byte;

    // Wrap around the write pointer. This works because the buffer size is
    // a modulo of 2.
    usb_write_index &= RECEIVE_BUFFER_INDEX_MASK;

    // If we are bumping into the read pointer we are dealing with a buffer
    // overflow. Back off and rather destroy the last value.
    if (usb_write_index == usb_read_index) {
        usb_write_index = (usb_write_index - 1) & RECEIVE_BUFFER_INDEX_MASK;
    }
}


// ****************************************************************************
void add_uint8_to_uart_receive_buffer(uint8_t byte)
{
    uart_receive_buffer[uart_write_index++] = byte;

    // Wrap around the write pointer. This works because the buffer size is
    // a modulo of 2.
    uart_write_index &= RECEIVE_BUFFER_INDEX_MASK;

    // If we are bumping into the read pointer we are dealing with a buffer
    // overflow. Back off and rather destroy the last value.
    if (uart_write_index == uart_read_index) {
        uart_write_index = (uart_write_index - 1) & RECEIVE_BUFFER_INDEX_MASK;
    }
}


// ****************************************************************************
bool HAL_getchar_pending(void *p)
{
    if (p == STDOUT_USB) {
        return usbserial_getchar_pending();
    }

    return (uart_read_index != uart_write_index);
}


// ****************************************************************************
uint8_t HAL_getchar(void *p)
{
    uint8_t data;

    if (p == STDOUT_USB) {
        return usbserial_getchar();
    }

    while (!HAL_getchar_pending(p));

    data = uart_receive_buffer[uart_read_index++];

    // Wrap around the read pointer.
    uart_read_index &= RECEIVE_BUFFER_INDEX_MASK;

    return data;
}


// ****************************************************************************
void HAL_putc(void *p, char c)
{
    if (p == STDOUT_USB) {
        usbserial_putc(c);
        return;
    }

    // FIXME: use a ring buffer to avoid blocking
    while (!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    UART_SERCOM->USART.DATA.reg = c;
}


