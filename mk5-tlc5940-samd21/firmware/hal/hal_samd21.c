#include <stdint.h>
#include <stdio.h>

#include <hal.h>
#include <printf.h>


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
    GCLK->GENCTRL.reg =
            GCLK_GENCTRL_ID(0) |
            GCLK_GENCTRL_SRC_DFLL48M |
            GCLK_GENCTRL_RUNSTDBY |
            GCLK_GENCTRL_GENEN;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

    // Setup GENCLK1 to run at 1 MHz. We use GENCLK1 for timers that need
    // microsecond resolution (e.g. servo output and servo reader).
    // We derive the clock from the 48 MHz PLL, so we use a clock divider of
    // 48
    GCLK->GENDIV.reg =
        GCLK_GENDIV_DIV(48) |
        GCLK_GENDIV_ID(1) ;

    GCLK->GENCTRL.reg =
            GCLK_GENCTRL_ID(1) |
            GCLK_GENCTRL_SRC_DFLL48M |
            GCLK_GENCTRL_RUNSTDBY |
            GCLK_GENCTRL_GENEN;

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);


    // ------------------------------------------------
    HAL_gpio_switched_light_output_out();


    // ------------------------------------------------
    // Configure the SYSTICK to create an interrupt every 1 millisecond
    SysTick_Config(48000);

    __enable_irq();
}


// ****************************************************************************
void HAL_hardware_init_final(void)
{

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
}


// ****************************************************************************
void HAL_uart_init(uint32_t baudrate)
{
    #define UART_CLK 48000000

    uint64_t brr = (uint64_t)65536 * (UART_CLK - 16 * baudrate) / UART_CLK;

    HAL_gpio_UART_TXD_out();
    HAL_gpio_UART_TXD_pmuxen(UART_TXD_PMUX);

    HAL_gpio_UART_RXD_in();
    HAL_gpio_UART_RXD_pmuxen(UART_RXD_PMUX);

    HAL_gpio_XLAT_out();
    HAL_gpio_XLAT_clear();


    PM->APBCMASK.reg |= UART_SERCOM_APBCMASK;

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(UART_SERCOM_GCLK_ID) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    UART_SERCOM->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_DORD |
        SERCOM_USART_CTRLA_MODE(SERCOM_USART_CTRLA_MODE_USART_INT_CLK_Val) |
        SERCOM_USART_CTRLA_RXPO(1/*PAD1*/) |
        SERCOM_USART_CTRLA_TXPO(0/*PAD0*/);

    UART_SERCOM->USART.CTRLB.reg =
        SERCOM_USART_CTRLB_RXEN |
        SERCOM_USART_CTRLB_TXEN |
        SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    UART_SERCOM->USART.BAUD.reg = (uint16_t)brr;
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    UART_SERCOM->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    while (UART_SERCOM->USART.SYNCBUSY.reg);

    UART_SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    NVIC_EnableIRQ(SERCOM0_IRQn);
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
void HAL_spi_init(void)
{
    int baud = 1;   // 12 MHz SPI clock @ 48 MHz

    HAL_gpio_SIN_out();
    HAL_gpio_SIN_pmuxen(SPI_SIN_PMUX);

    HAL_gpio_SCLK_out();
    HAL_gpio_SCLK_pmuxen(SPI_SCLK_PMUX);

    HAL_gpio_XLAT_out();
    HAL_gpio_XLAT_set();

    PM->APBCMASK.reg |= SPI_SERCOM_APBCMASK;

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(SPI_SERCOM_GCLK_ID) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    SPI_SERCOM->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_SWRST;

    while (SPI_SERCOM->SPI.CTRLA.reg & SERCOM_SPI_CTRLA_SWRST);

    SPI_SERCOM->SPI.BAUD.reg = baud;

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
        while (!SPI_SERCOM->SPI.INTFLAG.bit.DRE);
    }

    while (!SPI_SERCOM->SPI.INTFLAG.bit.TXC);
    HAL_gpio_XLAT_set();
}


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
void HAL_servo_output_init(void)
{
    HAL_gpio_OUT_out();
    HAL_gpio_OUT_pmuxen(PORT_PMUX_PMUXE_E_Val);

    PM->APBCMASK.reg |= PM_APBCMASK_TCC0;

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(TCC0_GCLK_ID) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(1);

    TCC0->CTRLA.reg = TCC_CTRLA_SWRST;

    while (TCC0->CTRLA.reg & TCC_CTRLA_SWRST);

    // Setup TCC0 to run at 1 MHz by using GENCLK1 (which we initialized to
    // 1 MHz). This way we can directly put microsecond values in the registers
    TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
    TCC0->PER.reg = 20000;          // 20 ms period = 50 Hz servo pulse
    TCC0->COUNT.reg = 0;
    TCC0->CC[0].reg = 0;            // Don't output a pulse for now
    TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;

    // Wait for all synchronizations finished to prevent HARD FAULT
    while (TCC0->SYNCBUSY.reg);
}


// ****************************************************************************
void HAL_servo_output_set_pulse(uint16_t servo_pulse_us)
{
    saved_TCC0_cc_value = servo_pulse_us;
    TCC0->CC[0].reg = saved_TCC0_cc_value;

    // Wait for synchronization finished to prevent HARD FAULT
    while (TCC0->SYNCBUSY.reg & TCC_SYNCBUSY_CC2);
}


// ****************************************************************************
void HAL_servo_output_enable(void)
{
    TCC0->CC[0].reg = saved_TCC0_cc_value;

    // Wait for synchronization finished to prevent HARD FAULT
    while (TCC0->SYNCBUSY.reg & TCC_SYNCBUSY_CC2);
}


// ****************************************************************************
void HAL_servo_output_disable(void)
{
    TCC0->CC[0].reg = 0;

    // Wait for synchronization finished to prevent HARD FAULT
    while (TCC0->SYNCBUSY.reg & TCC_SYNCBUSY_CC2);
}


// ****************************************************************************
void HAL_servo_reader_init(bool CPPM, uint32_t max_pulse)
{
    (void) CPPM;
    (void) max_pulse;

    HAL_gpio_ST_in();
    HAL_gpio_TH_in();
    HAL_gpio_CH3_in();


    // We need to set up the following:
    //
    // - EXTINT for the GPIO port, generating an event
    //    We can generate an event on both edges
    // - EVSYS to trigger TCC0 capture event when EXTINT fires
    // - TC? for capturing the event

    // PM->APBCMASK.reg |= PM_APBCMASK_TCC0;

    // GCLK->CLKCTRL.reg =
    //     GCLK_CLKCTRL_ID(TCC0_GCLK_ID) |
    //     GCLK_CLKCTRL_CLKEN |
    //     GCLK_CLKCTRL_GEN(1);

    // TCC0->CTRLA.reg = TCC_CTRLA_SWRST;

    // while (TCC0->CTRLA.reg & TCC_CTRLA_SWRST);

    // TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NFRQ;

    // TCC0->EVCTRL.reg = TCC_EVCTRL_MCEI0;

    // TCC0->CTRLA.reg |=
    //     TCC_CTRLA_CPTEN0 |
    //     TCC_CTRLA_ENABLE;


}


// ****************************************************************************
bool HAL_servo_reader_get_new_channels(uint32_t *raw_data)
{
    (void) raw_data;
    return false;
}

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// Interrupt handlers
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************


// ****************************************************************************
void SysTick_Handler(void)
{
    ++milliseconds;
}

// ****************************************************************************
void SERCOM0_Handler(void)
{
    if (UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) {
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