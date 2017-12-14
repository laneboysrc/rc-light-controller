#include <stdint.h>
#include <stdio.h>

#include <hal.h>
#include <printf.h>

volatile bool tcc1_int_fired = false;

volatile uint32_t st_value;
volatile uint32_t th_value;



volatile uint32_t milliseconds;

// These are defined by the linker via the samd21e15.ld linker script.
extern unsigned int _ram;
extern unsigned int _stacktop;

uint32_t saved_TCC0_cc_value = 1500;

__attribute__ ((section(".persistent_data")))
static volatile const uint32_t persistent_data[HAL_NUMBER_OF_PERSISTENT_ELEMENTS];


DECLARE_GPIO(UART_TXD, GPIO_PORTA, GPIO_BIT_TX)
DECLARE_GPIO(UART_RXD, GPIO_PORTA, GPIO_BIT_RX)

DECLARE_GPIO(SIN, GPIO_PORTA, GPIO_BIT_SIN)
DECLARE_GPIO(SCLK, GPIO_PORTA, GPIO_BIT_SCK)
DECLARE_GPIO(XLAT, GPIO_PORTA, GPIO_BIT_XLAT)

DECLARE_GPIO(ST, GPIO_PORTA, GPIO_BIT_ST)
DECLARE_GPIO(TH, GPIO_PORTA, GPIO_BIT_TH)
DECLARE_GPIO(CH3, GPIO_PORTA, GPIO_BIT_CH3)

DECLARE_GPIO(OUT, GPIO_PORTA, GPIO_BIT_OUT)

#ifdef SAMR21_XPLAINED_PRO

#define UART_SERCOM SERCOM0
#define UART_SERCOM_GCLK_ID SERCOM0_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM0
#define UART_TXD_PMUX PORT_PMUX_PMUXE_D_Val
#define UART_RXD_PMUX PORT_PMUX_PMUXE_D_Val
#define UART_TXD_PAD 0      // PAD0
#define UART_RXD_PAD 1      // PAD1

#define SPI_SERCOM  SERCOM3
#define SPI_SERCOM_GCLK_ID SERCOM3_GCLK_ID_CORE
#define SPI_SERCOM_APBCMASK PM_APBCMASK_SERCOM3
#define SPI_SCLK_PMUX PORT_PMUX_PMUXE_C_Val
#define SPI_SIN_PMUX PORT_PMUX_PMUXE_C_Val

#else

// FIXME not tested, PMUX may be wrong
#define UART_SERCOM SERCOM3
#define UART_SERCOM_GCLK_ID SERCOM0_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM0
#define UART_TXD_PMUX PORT_PMUX_PMUXE_D_Val
#define UART_RXD_PMUX PORT_PMUX_PMUXE_D_Val
#define UART_TXD_PAD 0      // PAD0
#define UART_RXD_PAD 1      // PAD1

#define SPI_SERCOM  SERCOM0
#define SPI_SERCOM_GCLK_ID SERCOM3_GCLK_ID_CORE
#define SPI_SERCOM_APBCMASK PM_APBCMASK_SERCOM3
#define SPI_SCLK_PMUX PORT_PMUX_PMUXE_C_Val
#define SPI_SIN_PMUX PORT_PMUX_PMUXE_C_Val

#endif

// FIXME
DECLARE_GPIO(OUT_TX, GPIO_PORTA, 18)     //  TCC0/WO[2]  Dummy just for now


/*
    Macro-magic to extract the calibration values from the bit-fields stored
    in the NVM of the SAMD21.

    We first retrieve a uint32_t pointer to the word where the calibration
    value resides, based on the lowest bit number of the bit field (i.e.
    NVM_USB_TRANSN_START).

    Then we shift the bits down so that the lowest bit ends up at bit 0 of
    the value. We have to do this modulo 32 bits. Then we create a mask
    based on the number of bits (END-START+1) and AND it to the value. Now
    the resulting number is the desired calibration value.

    Reference: Datasheet chapter "NVM Software Calibration Area Mapping"
*/
#define NVM_USB_TRANSN_START 45
#define NVM_USB_TRANSN_END 49

#define NVM_USB_TRANSP_START 50
#define NVM_USB_TRANSP_END 54

#define NVM_USB_TRIM_START 55
#define NVM_USB_TRIM_END 57

#define NVM_DFLL48M_COARSE_CAL_START 58
#define NVM_DFLL48M_COARSE_CAL_END 63

#define NVM_GET_CALIBRATION_VALUE(name) \
    ((*((uint32_t *)NVMCTRL_OTP4 + NVM_##name##_START / 32)) >> (NVM_##name##_START % 32)) & ((1 << (NVM_##name##_END - NVM_##name##_START + 1)) - 1)



#define RECEIVE_BUFFER_SIZE (16)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)
static uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t read_index = 0;
static volatile uint16_t write_index = 0;


// ****************************************************************************
void HAL_hardware_init(bool is_servo_reader, bool servo_output_enabled, bool uart_output_enabled)
{
    uint32_t coarse;

    (void) is_servo_reader;
    (void) servo_output_enabled;
    (void) uart_output_enabled;


    // ------------------------------------------------
    // Perform GPIO initialization as early as possible
    HAL_gpio_UART_TXD_out();
    HAL_gpio_UART_TXD_pmuxen(UART_TXD_PMUX);

    HAL_gpio_UART_RXD_in();
    HAL_gpio_UART_RXD_pmuxen(UART_RXD_PMUX);

    HAL_gpio_XLAT_out();
    HAL_gpio_switched_light_output_out();

    HAL_gpio_SIN_out();
    HAL_gpio_SIN_pmuxen(SPI_SIN_PMUX);

    HAL_gpio_SCLK_out();
    HAL_gpio_SCLK_pmuxen(SPI_SCLK_PMUX);

    HAL_gpio_OUT_out();
    HAL_gpio_OUT_pmuxen(PORT_PMUX_PMUXE_E_Val);     // Set GPIO to output TC4/W[0]

    HAL_gpio_ST_in();
    HAL_gpio_ST_pmuxen(PORT_PMUX_PMUXE_A_Val);      // Enable the EIC function on the ST pin (EXTINT0)
    HAL_gpio_TH_in();
    HAL_gpio_TH_pmuxen(PORT_PMUX_PMUXE_A_Val);      // Enable the EIC function on the TH pin (EXTINT1)
    HAL_gpio_CH3_in();


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

    coarse = NVM_GET_CALIBRATION_VALUE(DFLL48M_COARSE_CAL);
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
        GCLK_GENDIV_DIV(1) |
        GCLK_GENDIV_ID(0) ;

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
        GCLK_GENDIV_DIV(24) |
        GCLK_GENDIV_ID(1) ;

    GCLK->GENCTRL.reg =
            GCLK_GENCTRL_ID(1) |
            GCLK_GENCTRL_SRC_DFLL48M |
            GCLK_GENCTRL_RUNSTDBY |
            GCLK_GENCTRL_GENEN;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);


    // ------------------------------------------------
    // Turn on power to the peripherals we use
    PM->APBCMASK.reg |=
        UART_SERCOM_APBCMASK |
        SPI_SERCOM_APBCMASK |
        PM_APBAMASK_EIC |
        PM_APBCMASK_EVSYS |
        PM_APBCMASK_TCC0 |
        PM_APBCMASK_TC4;


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
    // FIXME: this is for testing only
    HAL_servo_reader_init(false, 2500);
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

    if (tcc1_int_fired) {
        static uint32_t last_st;
        static uint32_t last_th;

        tcc1_int_fired = 0;

        if (last_st != st_value) {
            last_st = st_value;
            printf("ST measured: %d\n", last_st);
        }
        if (last_th != th_value) {
            last_th = th_value;
            printf("TH measured: %d\n", last_th);
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
        SERCOM_USART_CTRLA_RXPO(UART_RXD_PAD) |
        SERCOM_USART_CTRLA_TXPO(UART_TXD_PAD);

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
    NVIC_EnableIRQ(SERCOM0_IRQn);
}


// ****************************************************************************
void SERCOM0_Handler(void)
{
    if (UART_SERCOM->USART.INTFLAG.bit.RXC) {
        receive_buffer[write_index++] = (uint8_t)UART_SERCOM->USART.DATA.reg;

        // Wrap around the write pointer. This works because the buffer size is
        // a modulo of 2.
        write_index &= RECEIVE_BUFFER_INDEX_MASK;

        // If we are bumping into the read pointer we are dealing with a buffer
        // overflow. Back off and rather destroy the last value.
        if (write_index == read_index) {
            write_index = (write_index - 1) & RECEIVE_BUFFER_INDEX_MASK;
        }
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
void HAL_putc(void *p, char c)
{
    (void) p;
    // FIXME: check for STDOUT and STDOUT_DEBUG
    // if (p == STDOUT_DEBUG) {
    // }

    while (!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    UART_SERCOM->USART.DATA.reg = c;
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



// ****************************************************************************
void HAL_spi_init(void)
{
    // Use GLKGEN0 (48 MHz) as clock source for SPI
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(SPI_SERCOM_GCLK_ID) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    // Reset the peripheral
    SPI_SERCOM->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_SWRST;
    while (SPI_SERCOM->SPI.CTRLA.reg & SERCOM_SPI_CTRLA_SWRST);

    // 12 MHz SPI clock @ 48 MHz
    SPI_SERCOM->SPI.BAUD.reg = 1;

    // Enable the SPI master, mode 0
    SPI_SERCOM->SPI.CTRLA.reg =
        SERCOM_SPI_CTRLA_MODE_SPI_MASTER |
        SERCOM_SPI_CTRLA_DOPO(0) |
        SERCOM_SPI_CTRLA_ENABLE ;
}


// ****************************************************************************
void HAL_spi_transaction(uint8_t *data, uint8_t count)
{
    HAL_gpio_XLAT_clear();

    for (uint8_t i = 0; i < count; i++) {
        SPI_SERCOM->SPI.DATA.reg = data[i];

        // Wait until the next byte can be written to the data register
        while (!SPI_SERCOM->SPI.INTFLAG.bit.DRE);
    }

    // Wait until the last byte has been sent completely
    while (!SPI_SERCOM->SPI.INTFLAG.bit.TXC);

    HAL_gpio_XLAT_set();
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



// ****************************************************************************
volatile const uint32_t *HAL_persistent_storage_read(void)
{
    return persistent_data;
}


// ****************************************************************************
const char *HAL_persistent_storage_write(const uint32_t *new_data)
{
    uint32_t i;

    // Set manual page write
    NVMCTRL->CTRLB.bit.MANW = 1;

    // Execute the Erase Row command
    NVMCTRL->ADDR.reg = NVMCTRL_ADDR_ADDR((uint32_t)persistent_data >> 1);
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    while (!NVMCTRL->INTFLAG.bit.READY);

    // Execute Page Buffer Clear
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
    while (!NVMCTRL->INTFLAG.bit.READY);

    // Fill the page buffer
    for (i = 0; i < HAL_NUMBER_OF_PERSISTENT_ELEMENTS; i++) {
        *(uint32_t *)(&persistent_data[i]) = new_data[i];
    }

    // Execute the Write Page command
    NVMCTRL->ADDR.reg = NVMCTRL_ADDR_ADDR((uint32_t)persistent_data >> 1);
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    while (!NVMCTRL->INTFLAG.bit.READY);

    return 0;
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



// ****************************************************************************
void HAL_servo_output_init(void)
{
    // Use GLKGEN1 (2 MHz) as clock source for TC4
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_TC4_TC5) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(1);

    // --------------------------------------
    // Setup TC4 in PWM mode with a period of 20 ms. Unfortunately in 16bit
    // mode there is no period register, so we have to use Compare Channel 1
    // to generate an event, which we then pass back as input event to TC4 via
    // the Event System, re-triggering the timer.

    // Reset TC4
    TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (TC4->COUNT16.CTRLA.reg & TC_CTRLA_SWRST);

    // Set-up Channel 3 to output to TC4
    EVSYS->USER.reg =
        EVSYS_USER_USER(0x13/*TC4*/) |
        EVSYS_USER_CHANNEL(3+1);

    // Set-up Channel 3 to trigger synchronously on TC4 MC1
    EVSYS->CHANNEL.reg =
        EVSYS_CHANNEL_CHANNEL(3) |
        EVSYS_CHANNEL_PATH_ASYNCHRONOUS |
        EVSYS_CHANNEL_EVGEN(0x38/*TC4 MC1*/);

    TC4->COUNT16.CC[0].reg = 0;             // Don't output a pulse initially
    TC4->COUNT16.CC[1].reg = (20000 * 2)-1; // 20 ms period = 50 Hz servo pulse

    TC4->COUNT16.EVCTRL.reg =
        TC_EVCTRL_MCEO1 |                   // Enable Capture channel 1 event output
        TC_EVCTRL_TCEI |                    // Enable event input
        TC_EVCTRL_EVACT_RETRIGGER;          // Event action is "re-trigger"

    TC4->COUNT16.CTRLA.reg |=
        TC_CTRLA_MODE_COUNT16 |             // We use the timer in 16 bit mode
        TC_CTRLA_WAVEGEN_NPWM |             // Normal PWM operation
        TC_CTRLA_ENABLE;

    // Wait for all synchronizations finished to prevent HARD FAULT
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);

    // With the re-trigger action enabled, the timer does not start
    // automatically. We have to kick it off manually by issuing a retrigger
    // command.
    TC4->COUNT16.CTRLBSET.reg = TC_CTRLBCLR_CMD_RETRIGGER;
}


// ****************************************************************************
void HAL_servo_output_set_pulse(uint16_t servo_pulse_us)
{
    // Since the timer runs at 2 MHz clock, we have to multiply the micro-second
    // value by 2.
    saved_TCC0_cc_value = servo_pulse_us * 2;

    TC4->COUNT16.CC[0].reg = saved_TCC0_cc_value;
}


// ****************************************************************************
void HAL_servo_output_enable(void)
{
    TC4->COUNT16.CC[0].reg = saved_TCC0_cc_value;
}


// ****************************************************************************
void HAL_servo_output_disable(void)
{
    TC4->COUNT16.CC[0].reg = 0;
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



// ****************************************************************************
// We use the external interrupt controller, event system and TCC0 to read
// the servo pulses.
//
// The servo GPIOs EXTINT is setup to generated an event on both edges.
// The Event System triggers one of three Capture Channels on TCC0 on each
// event.
//
// ST  uses EXTINT0, EVSYS channel 0, TCC CC0
// TH  uses EXTINT1, EVSYS channel 1, TCC CC1
// CH3 uses EXTINT3, EVSYS channel 2, TCC CC2
void HAL_servo_reader_init(bool CPPM, uint32_t max_pulse)
{
    (void) CPPM;
    (void) max_pulse;

    // --------------------------------------
    // Use GLKGEN0 (48 MHz) as clock source for the EIC
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID_EIC |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    // --------------------------------------
    // FIXME: should we do a SWRST here for the EIC?
    // Detect both edges, enable the glitch filter
    EIC->CONFIG[0].reg =
        EIC_CONFIG_SENSE0_BOTH | EIC_CONFIG_FILTEN0 |
        EIC_CONFIG_SENSE1_BOTH | EIC_CONFIG_FILTEN1 |
        EIC_CONFIG_SENSE3_BOTH | EIC_CONFIG_FILTEN3;

    // Enable event generation when an edge is detected
    EIC->EVCTRL.reg =
        EIC_EVCTRL_EXTINTEO0 |
        EIC_EVCTRL_EXTINTEO1 |
        EIC_EVCTRL_EXTINTEO3;

    // Enable the external interrupt controller
    EIC->CTRL.bit.ENABLE = 1;


    // --------------------------------------
    // Set-up Event System channel 0 to output to Match/Capture 0 of TCC0
    EVSYS->USER.reg =
        EVSYS_USER_USER(0x06) |             // TCC0 MC0
        EVSYS_USER_CHANNEL(0+1);

    // Set-up Channel 0 to trigger synchronously on EXTINT0 (ST)
    EVSYS->CHANNEL.reg =
        EVSYS_CHANNEL_CHANNEL(0) |
        EVSYS_CHANNEL_PATH_SYNCHRONOUS |
        EVSYS_CHANNEL_EDGSEL_RISING_EDGE |
        EVSYS_CHANNEL_EVGEN(0x0c);          // EIC EXTINT0

    // Set-up Event System channel 1 to output to Match/Capture 1 of TCC0
    EVSYS->USER.reg =
        EVSYS_USER_USER(0x07) |             // TCC0 MC1
        EVSYS_USER_CHANNEL(1+1);

    // Set-up Channel 1 to trigger synchronously on EXTINT1 (TH)
    EVSYS->CHANNEL.reg =
        EVSYS_CHANNEL_CHANNEL(1) |
        EVSYS_CHANNEL_PATH_SYNCHRONOUS |
        EVSYS_CHANNEL_EDGSEL_RISING_EDGE |
        EVSYS_CHANNEL_EVGEN(0x0d);          // EIC EXTINT1

    // // Set-up Event System channel 2 to output to Match/Capture 2 of TCC0
    EVSYS->USER.reg =
        EVSYS_USER_USER(0x08) |             // TCC0 MC2
        EVSYS_USER_CHANNEL(2+1);

    // Set-up Channel 2 to trigger synchronously on EXTINT3 (CH3)
    EVSYS->CHANNEL.reg =
        EVSYS_CHANNEL_CHANNEL(2) |
        EVSYS_CHANNEL_PATH_SYNCHRONOUS |
        EVSYS_CHANNEL_EDGSEL_RISING_EDGE |
        EVSYS_CHANNEL_EVGEN(0x0f);          // EIC EXTINT3


    // --------------------------------------
    // Use GLKGEN1 (2 MHz) as clock source for the TCC0
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_TCC0_TCC1) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(1);

    // No prescaler; Enable capture channel 0..2
    TCC0->CTRLA.reg =
        TCC_CTRLA_PRESCALER_DIV1 |
        TCC_CTRLA_PRESCSYNC_GCLK |
        TCC_CTRLA_CPTEN0 |
        TCC_CTRLA_CPTEN1 |
        TCC_CTRLA_CPTEN2;

    // Enable capture channel 0..2 event input
    TCC0->EVCTRL.reg =
        TCC_EVCTRL_MCEI0 |
        TCC_EVCTRL_MCEI1 |
        TCC_EVCTRL_MCEI2;

    // Set up the timer to run continously in 24 bit mode
    TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NFRQ;
    TCC0->COUNT.reg = 0;
    TCC0->PER.reg = 0xffffff;

    TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;
    while (TCC0->SYNCBUSY.reg);

    // Enable interrupts on Match/Compare 0..2
    TCC0->INTENSET.reg = TCC_INTFLAG_MC0 | TCC_INTFLAG_MC1 | TCC_INTFLAG_MC2;
    NVIC_EnableIRQ(TCC0_IRQn);
}


// ****************************************************************************
void TCC0_Handler(void)
{
    static uint32_t start;
    uint32_t now;

    NVIC_ClearPendingIRQ(TCC0_IRQn);

    if (TCC0->INTFLAG.bit.MC0) {
        // Wait until the capture has synchronized, then read the captured value
        while (TCC0->SYNCBUSY.bit.CC0);
        now = TCC0->CC[0].reg;

        if (HAL_gpio_ST_read()) {
            start = now;
        }
        else {
            st_value = (now - start) / 2;
            tcc1_int_fired = true;
        }
    }

    if (TCC0->INTFLAG.bit.MC1) {
        // Wait until the capture has synchronized, then read the captured value
        while (TCC0->SYNCBUSY.bit.CC1);
        now = TCC0->CC[1].reg;

        if (HAL_gpio_TH_read()) {
            start = now;
        }
        else {
            th_value = (now - start) / 2;
            tcc1_int_fired = true;
        }
    }

    if (TCC0->INTFLAG.bit.MC2) {
        // Wait until the capture has synchronized, then read the captured value
        while (TCC0->SYNCBUSY.bit.CC2);
        now = TCC0->CC[2].reg;
    }
}


// ****************************************************************************
bool HAL_servo_reader_get_new_channels(uint32_t *raw_data)
{
    (void) raw_data;
    return false;
}
