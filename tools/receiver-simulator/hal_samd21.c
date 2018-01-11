#include <stdint.h>
#include <stdio.h>

#include <hal.h>
#include <printf.h>


volatile uint32_t milliseconds;

// These are defined by the linker via the samd21e15.ld linker script.
extern uint32_t _ram;
extern uint32_t _stacktop;

uint32_t saved_TCC0_cc_value = 1500;


#define RECEIVE_BUFFER_SIZE (16)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)
static uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t read_index = 0;
static volatile uint16_t write_index = 0;

static void HAL_adc_init(void);
uint16_t HAL_adc_read(uint32_t mux);


// ****************************************************************************
void HAL_hardware_init(bool is_servo_reader, bool servo_output_enabled, bool uart_output_enabled)
{
    uint32_t *coarse_p;
    uint32_t coarse;

    (void) is_servo_reader;
    (void) servo_output_enabled;
    (void) uart_output_enabled;


    // ------------------------------------------------
    // Perform GPIO initialization as early as possible
    HAL_gpio_out(HAL_GPIO_TX);
    HAL_gpio_pmuxen(HAL_GPIO_TX);

    HAL_gpio_in(HAL_GPIO_RX);
    HAL_gpio_pmuxen(HAL_GPIO_RX);

    HAL_gpio_out(HAL_GPIO_ST);
    HAL_gpio_pmuxen(HAL_GPIO_ST);

    HAL_gpio_out(HAL_GPIO_TH);
    HAL_gpio_pmuxen(HAL_GPIO_TH);

    HAL_gpio_in(HAL_GPIO_ST_IN);
    HAL_gpio_pmuxen(HAL_GPIO_ST_IN);

    HAL_gpio_in(HAL_GPIO_TH_IN);
    HAL_gpio_pmuxen(HAL_GPIO_TH_IN);


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
        PM_APBCMASK_EVSYS |
        PM_APBCMASK_TC3 |
        PM_APBCMASK_TC4 |
        PM_APBCMASK_ADC;


    // Always turn on the Generic Clock within EVSYS
    EVSYS->CTRL.reg = EVSYS_CTRL_GCLKREQ;


    // ------------------------------------------------
    // Configure the SYSTICK to create an interrupt every 1 millisecond
    SysTick_Config(48000);
    NVIC_SetPriority(SysTick_IRQn, 0);

    __enable_irq();


    HAL_adc_init();
    HAL_servo_output_init();
}


// ****************************************************************************
void SysTick_Handler(void)
{
    ++milliseconds;
}


// ****************************************************************************
void HAL_hardware_init_final(void)
{
    HAL_servo_output_set_pulse(1400);
}


// ****************************************************************************
void HAL_service(void)
{
    // Nothing to do
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
        SERCOM_USART_CTRLA_RXPO(HAL_GPIO_RX.pad) |
        SERCOM_USART_CTRLA_TXPO(HAL_GPIO_TX.pad);

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

    while (!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    UART_SERCOM->USART.DATA.reg = c;
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



// ****************************************************************************
void HAL_spi_init(void)
{
    // Not implemented
}


// ****************************************************************************
void HAL_spi_transaction(uint8_t *data, uint8_t count)
{
    // Not implemented
    (void) data;
    (void) count;
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



// ****************************************************************************
volatile const uint32_t *HAL_persistent_storage_read(void)
{
    // Not implemented
    return NULL;
}


// ****************************************************************************
const char *HAL_persistent_storage_write(const uint32_t *new_data)
{
    // Not implemented
    (void) new_data;
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

    // Use GLKGEN1 (2 MHz) as clock source for TC3
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_TCC2_TC3) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(1);


    // --------------------------------------
    // Setup TC3 in PWM mode. The pulse width is via Compare Channel 0.
    //
    // Unfortunately in 16bit mode TC3 has no period register, so we have to
    // use Compare Channel 1 to generate an event, which we then pass back as
    // input event to TC3 via the Event System, re-triggering the timer.

    // Reset TC3
    TC3->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (TC3->COUNT16.CTRLA.reg & TC_CTRLA_SWRST);

    // Set-up Channel 3 to output to TC3
    EVSYS->USER.reg =
        EVSYS_USER_USER(0x12) |             // TC3
        EVSYS_USER_CHANNEL(3+1);

    // Set-up Channel 3 to trigger synchronously on TC3 MC1
    EVSYS->CHANNEL.reg =
        EVSYS_CHANNEL_CHANNEL(3) |
        EVSYS_CHANNEL_PATH_ASYNCHRONOUS |
        EVSYS_CHANNEL_EVGEN(0x35);          // TC3 MC1

    TC3->COUNT16.CC[0].reg = 0;             // Don't output a pulse initially
    TC3->COUNT16.CC[1].reg = (16000 * 2)-1;   // 16 ms period

    TC3->COUNT16.EVCTRL.reg =
        TC_EVCTRL_MCEO1 |                   // Enable Capture channel 1 event output
        TC_EVCTRL_TCEI |                    // Enable event input
        TC_EVCTRL_EVACT_RETRIGGER;          // Event action is "re-trigger"

    TC3->COUNT16.CTRLA.reg |=
        TC_CTRLA_MODE_COUNT16 |             // We use the timer in 16 bit mode
        TC_CTRLA_WAVEGEN_NPWM |             // Normal PWM operation
        TC_CTRLA_ENABLE;

    // Wait for all synchronizations finished to prevent HARD FAULT
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY);


    // --------------------------------------
    // Setup TC4 in PWM mode. The pulse width is via Compare Channel 0.
    //
    // Unfortunately in 16bit mode TC4 has no period register, so we have to
    // use Compare Channel 1 to generate an event, which we then pass back as
    // input event to TC4 via the Event System, re-triggering the timer.

    // Reset TC4
    TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (TC4->COUNT16.CTRLA.reg & TC_CTRLA_SWRST);

    // Set-up Channel 3 to output to TC4
    EVSYS->USER.reg =
        EVSYS_USER_USER(0x13) |             // TC4
        EVSYS_USER_CHANNEL(4+1);

    // Set-up Channel 3 to trigger synchronously on TC4 MC1
    EVSYS->CHANNEL.reg =
        EVSYS_CHANNEL_CHANNEL(4) |
        EVSYS_CHANNEL_PATH_ASYNCHRONOUS |
        EVSYS_CHANNEL_EVGEN(0x38);          // TC4 MC1

    TC4->COUNT16.CC[0].reg = 0;             // Don't output a pulse initially
    TC4->COUNT16.CC[1].reg = (32000 * 2)-1;   // 32 ms period

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
    TC3->COUNT16.CTRLBSET.reg = TC_CTRLBCLR_CMD_RETRIGGER;
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
void HAL_servo_reader_init(bool CPPM, uint32_t max_pulse)
{
    // Not implemented
    (void) CPPM;
    (void) max_pulse;
}


// ****************************************************************************
bool HAL_servo_reader_get_new_channels(uint32_t *out_us)
{
    // Not implemented
    (void) out_us;
    return false;
}



// ****************************************************************************
// ****************************************************************************
// ****************************************************************************



void HAL_adc_init(void)
{
    uint32_t *nvram_pointer;
    uint32_t bias;
    uint32_t linearity0;
    uint32_t linearity1;
    uint32_t linearity;

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(ADC_GCLK_ID) |
        GCLK_CLKCTRL_GEN(0) |
        GCLK_CLKCTRL_CLKEN;

    // Reset the ADC
    ADC->CTRLA.reg = ADC_CTRLA_SWRST;
    while (ADC->CTRLA.reg & ADC_CTRLA_SWRST);

    ADC->REFCTRL.reg =
        ADC_REFCTRL_REFSEL_INTVCC1 |
        ADC_REFCTRL_REFCOMP;

    ADC->CTRLB.reg =
        ADC_CTRLB_RESSEL_16BIT |
        ADC_CTRLB_PRESCALER_DIV32;

    ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_128;

    nvram_pointer = (uint32_t *)ADC_FUSES_BIASCAL_ADDR;
    bias = (*nvram_pointer & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;

    nvram_pointer = (uint32_t *)ADC_FUSES_LINEARITY_0_ADDR;
    linearity0 = (*nvram_pointer & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
    nvram_pointer = (uint32_t *)ADC_FUSES_LINEARITY_1_ADDR;
    linearity1 = (*nvram_pointer & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos;
    linearity = (linearity1 << 4) | linearity0;

    ADC->CALIB.reg =
        ADC_CALIB_BIAS_CAL(bias) |
        ADC_CALIB_LINEARITY_CAL(linearity);

    ADC->CTRLA.reg = ADC_CTRLA_ENABLE;
}

//-----------------------------------------------------------------------------
uint16_t HAL_adc_read(uint32_t mux)
{
    ADC->INPUTCTRL.reg =
        mux |
        ADC_INPUTCTRL_MUXNEG_GND |
        ADC_INPUTCTRL_GAIN_DIV2;
    ADC->SWTRIG.reg = ADC_SWTRIG_START;
    while (0 == (ADC->INTFLAG.reg & ADC_INTFLAG_RESRDY));

    return ADC->RESULT.reg;
}