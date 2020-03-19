#include <stdint.h>
#include <stdio.h>

#include <globals.h>

#include <hal.h>
#include <usb.h>
#include <printf.h>

void add_uint8_to_receive_buffer(uint8_t byte);
extern bool test_interface_is_write_busy(void);
extern void test_interface_write(uint8_t *data, uint8_t length);

volatile uint32_t milliseconds;

static bool diagnostics_on_uart;

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

static uint8_t tx_pad;

__attribute__ ((section(".persistent_data")))
static volatile const uint32_t persistent_data[HAL_NUMBER_OF_PERSISTENT_ELEMENTS];


#define RECEIVE_BUFFER_SIZE (16)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)
static uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t read_index = 0;
static volatile uint16_t write_index = 0;

#define SEND_BUFFER_SIZE (64)
static uint8_t send_buffer[SEND_BUFFER_SIZE];
static volatile uint16_t send_buffer_index = 0;

// We use all 24 bits of TCC0, so the maximum period value is 0xffffff
#define TCC0_PERIOD (0xfffffful)



// ****************************************************************************
void HAL_hardware_init(bool is_servo_reader, bool servo_output_enabled, bool uart_output_enabled)
{
    uint32_t *coarse_p;
    uint32_t coarse;

    // FIXME: output diagnostics on UART only if uart_output_enabled is false
    (void) uart_output_enabled;


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
    tx_pad = HAL_GPIO_TX.txpo;

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


    // ------------------------
    // The UART Tx can be used for diagnostics output if it is not in use.
    // The pin is in use when HAL gets passed "uart_output_enabled==true"
    // (UART used for pre-processor, winch or slave output), or when we
    // have a servo reader and the servo output is enabled.
    //
    // Or in other words: the UART can output diagnostics on the OUT pin
    // when it is not in use, or the UART can output diagnostics on the TH/Tx
    // pin when we the UART reader is in use and no UART output function is
    // configured.
    diagnostics_on_uart = !uart_output_enabled;
    if (is_servo_reader) {
        if (servo_output_enabled) {
            diagnostics_on_uart = false;
        }
    }


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
#ifndef NODEBUG
    #define CANARY 0xcafebabe

    /*
    There is an issue if we initialize the last_found static variable with
    _stacktop at compile time. For some reason it does not contain the proper
    value.
    We therefore initialize it with 0 and check for that. If we enounter 0 we
    load the _stacktop address and everything works well.

    Worst case the program hangs when last_found is not aligned to 4 bytes, as
    a hard fault is raised.
    */

    static uint32_t *last_found;
    uint32_t *now;

    if (last_found == NULL) {
        last_found = (uint32_t *)&_stacktop;
    }

    now = last_found;

    while (*now != CANARY  &&  now > (uint32_t *)&_ram) {
        --now;
    }

    if (now < last_found) {
        last_found = now;
        fprintf(STDOUT_DEBUG, "Stack down to 0x%08x\n", (uint32_t)now);
    }
#endif

    if (start_bootloader) {
        uint32_t end = milliseconds + 50;

        fprintf(STDOUT_DEBUG, "REBOOTING INTO BOOTLOADER\n");

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
    if (send_buffer_index) {
        if (!test_interface_is_write_busy()) {
            test_interface_write(send_buffer, send_buffer_index);
            send_buffer_index = 0;
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
        SERCOM_USART_CTRLA_TXPO(tx_pad);

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
        add_uint8_to_receive_buffer((uint8_t)UART_SERCOM->USART.DATA.reg);
    }
}


// ****************************************************************************
void add_uint8_to_receive_buffer(uint8_t byte)
{
    receive_buffer[write_index++] = byte;

    // Wrap around the write pointer. This works because the buffer size is
    // a modulo of 2.
    write_index &= RECEIVE_BUFFER_INDEX_MASK;

    // If we are bumping into the read pointer we are dealing with a buffer
    // overflow. Back off and rather destroy the last value.
    if (write_index == read_index) {
        write_index = (write_index - 1) & RECEIVE_BUFFER_INDEX_MASK;
    }
}


// ****************************************************************************
bool HAL_getchar_pending(void)
{
    return (read_index != write_index);
}


// ****************************************************************************
uint8_t HAL_getchar(void)
{
    uint8_t data;

    while (!HAL_getchar_pending());

    data = receive_buffer[read_index++];

    // Wrap around the read pointer.
    read_index &= RECEIVE_BUFFER_INDEX_MASK;

    return data;
}


// ****************************************************************************
static void usbserial_putc(char c) {
    if (send_buffer_index < SEND_BUFFER_SIZE) {
        send_buffer[send_buffer_index++] = c;
    }
}


// ****************************************************************************
void HAL_putc(void *p, char c)
{
    if (p == STDOUT_DEBUG) {
        usbserial_putc(c);
    }

    // Ignore diagnostics requests if disabled
    if (!diagnostics_on_uart  &&  p == STDOUT_DEBUG) {
        return;
    }

    while (!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    UART_SERCOM->USART.DATA.reg = c;
}


