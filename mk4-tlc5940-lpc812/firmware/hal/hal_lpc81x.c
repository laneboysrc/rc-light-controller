#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <LPC8xx.h>
#include <LPC8xx_ROM_API.h>
#include <globals.h>
#include <hal.h>
#include <printf.h>


void SysTick_handler(void);
void HardFault_handler(void);
void SCT_irq_handler(void);
void UART0_irq_handler(void);

// These are all defined by the linker via the lpc81x.ld linker script.
extern unsigned int _ram;
extern unsigned int _stacktop;


#define RECEIVE_BUFFER_SIZE (64)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)


__attribute__ ((section(".persistent_data")))
static volatile const uint32_t persistent_data[HAL_NUMBER_OF_PERSISTENT_ELEMENTS];

volatile uint32_t milliseconds;

static volatile uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t read_index = 0;
static volatile uint16_t write_index = 0;
static volatile bool receive_buffer_overflow;

static volatile bool new_raw_channel_data = false;

static uint8_t aux2_pin = 0xff;
static uint8_t spi1_mosi_pin = 0xff;
static volatile bool aux3_active = false;
static volatile uint32_t aux2_aux3_timeout;
static uint32_t raw_data[5];



// ****************************************************************************
// LPC832 specific functionality
//
// The LPC832 has DMA and an additional MUX register for the SCT inputs,
// which is not in the LPC8xx.h file we are using.
// The output functions of the SCTimer also only work when both L/H timers are
// enabled on LPC832, while on LPC812 just enableing the H timer was
// sufficient.
//
// Also that the PINASSIGN registers are different on LPC832.
//
// UART and SPI are basically the same.
//
// We use the flag is_lpc832_or_lpc824 to handle these differences at run-time.
// ****************************************************************************
uint16_t mcu_type;

typedef struct {                            /*!< (@ 0x4002c020) INMUX Structure          */
  __IO uint32_t INP[4];                     /*!< (@ 0x40024000) Input mux registers 0..3 */
} LPC_INMUX_TypeDef;
#define LPC_INMUX_BASE      (LPC_APB0_BASE + 0x2c020)
#define LPC_INMUX           ((LPC_INMUX_TypeDef *) LPC_INMUX_BASE)



/* ****************************************************************************
SCT Timer usage

The SCT is configured into two independent 16 bit timer/counters.

Counter H is used for the servo output.
It makes use of EVENT4 (20 ms pulse repetition) and EVENT5 (servo pulse width).
CTOUT_1 is used to drive the output pin.

Counter L is used for reading up to 4 servo inputs.
EVENT0 .. EVENT3 are used
CTIN_0 .. CTIN_3 are used and connected to the servo input pins.

**************************************************************************** */


// ****************************************************************************
void SysTick_handler(void)
{
    ++milliseconds;
}


// ****************************************************************************
void HardFault_handler(void)
{
    HAL_putc(((void *) 1), 'H');
    HAL_putc(((void *) 1), '\n');
    while (1);
}


// ****************************************************************************
void HAL_hardware_init(void)
{
#if HAL_SYSTEM_CLOCK != 12000000
#error Clock initialization code expexts __SYSTEM_CLOCK to be set to 1200000
#endif

    // Variable that allows us to deal with configuration that is slightly
    // different between the various support LPC MCUs
    //
    // The device ID is something like 0x00008122 (LPC812M101JDH20), where the
    // last number indicates the package variant. So we shift 4 bits to the
    // right to obtain the MCU type number.
    // So mcu_type will contain 0x0812 for LPC812, 0x0832 for LPC832, etc.
    mcu_type = LPC_SYSCON->DEVICE_ID >> 4;

    // Turn on brown-out detection and reset
    LPC_SYSCON->BODCTRL = (1 << 4) | (1 << 2) | (3 << 0);


    // Set flash wait-states to 1 system clock
    LPC_FLASHCTRL->FLASHCFG = 0;


    // Turn on peripheral clocks for SCTimer, IOCON, SPI1, SPI0
    // (GPIO, SWM alrady enabled after reset)
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 8) | (1 << 18) | (1 << 12) | (1 << 11);


    // ------------------------
    // IO configuration

    // Disable all special functions
    // NOTE: this is required on both LPC812 and LPC832, because by default
    // SWD pins are enabled on PIO2 and PIO3, so we can't communicate with the
    // TLC5940
    LPC_SWM->PINENABLE0 = 0xffffffff;

    // Make the open drain ports PIO0_10, PIO0_11 outputs and pull to ground
    // to prevent them from floating.
    //
    // Exceptions:
    // - If multi-aux-channel is enabled PIO0_11 is used as input with
    //   an external pull-down, so we keep it as the default (input)
    HAL_gpio_clear(HAL_GPIO_PIN10);
    HAL_gpio_out(HAL_GPIO_PIN10);

    if (!config.flags2.multi_aux) {

        HAL_gpio_clear(HAL_GPIO_PIN11);
        HAL_gpio_out(HAL_GPIO_PIN11);
    }


    // Enable glitch filtering on the IOs
    // GOTCHA: ICONCLKDIV0 is actually the last register in the array!
    LPC_SYSCON->IOCONCLKDIV[6] = 255;       // Glitch filter 0: Main clock divided by 255
    LPC_SYSCON->IOCONCLKDIV[5] = 1;         // Glitch filter 1: Main clock divided by 1

    // NOTE: for some reason it is absolutely necessary to enable glitch
    // filtering on the IOs used for the capture timer. One clock cytle of the
    // main clock is enough, but with none weird things happen.

    HAL_gpio_glitch_filter(HAL_GPIO_ST);
    HAL_gpio_glitch_filter(HAL_GPIO_TH);
    HAL_gpio_glitch_filter(HAL_GPIO_AUX);
    HAL_gpio_glitch_filter(HAL_GPIO_AUX2);
    HAL_gpio_glitch_filter(HAL_GPIO_AUX2_S);
    HAL_gpio_glitch_filter(HAL_GPIO_AUX3);
    HAL_gpio_glitch_filter(HAL_GPIO_PIN10);


    // ------------------------
    // Configure SCTimer globally for two 16-bit counters
    LPC_SCT->CONFIG = 0;


    // ------------------------
    // SysTick configuration 1000 Hz / 1 ms
    SysTick_Config((HAL_SYSTEM_CLOCK / 1000)- 1);
}


// ****************************************************************************
void HAL_hardware_init_final(void)
{
    // Turn off peripheral clock for IOCON to preserve power
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 18);

    // IMPORTANT: SWM needs to stay enabled so that we can dynamically change
    // CTIN_0 to capture multiple AUX channels!

    // LPC832: OUT.SET and OUT.CLR only work when both L and H counters
    // are running!
    LPC_SCT->CTRL_H &= ~(1 << 2);          // Start the SCTimer H
    LPC_SCT->CTRL_L &= ~(1 << 2);          // Start the SCTimer L
}


// ****************************************************************************
void HAL_service(void)
{
#ifndef NODEBUG
    #define CANARY 0xcafebabe

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


/*
UART register value calculation

Problem description:
    - System clock varies, but is fixed at compile time
    - Assumption is that UARTCLKDIV is 1
    - We have a fixed list of baudrates to support
        - Therefore we can use the preprocessor for calculation
    - We want to find settings for MULT and BRG for each baudrate

First we need to calculate the BRG value for the highest possible baudrate

    BAUDRATE = U_PCLK/(16 * (BRGVAL + 1))
    BAUDRATE * 16 * (BRGVAL + 1) = U_PCLK
    U_PCLK = __SYSTEM_CLOCK
    BAUDRATE = 115200
    BAUDRATE * 16 * (BRGVAL_MAXBAUD + 1) = __SYSTEM_CLOCK
    BRGVAL_MAXBAUD = (__SYSTEM_CLOCK / (BAUDRATE * 16)) - 1
    BRGVAL_MAXBAUD = int(__SYSTEM_CLOCK / (115200 * 16)) - 1

    For 12 MHz BRGVAL_MAXBAUD is 5
    For 30 MHz BRGVAL_MAXBAUD is 15

Then we can calculate the exact U_PCLK we need:

    U_PCLK = BAUDRATE * 16 * (BRGVAL_MAXBAUD + 1)

    For 12 MHZ U_PCLK is 11059200
    For 30 MHZ U_PCLK is 29491200

Now we can calculate the MULT needed:

    U_PCLK = (__SYSTEM_CLOCK / UARTCLKDIV) / (1 + (MULT / DIV))
    UARTCLKDIV = 1
    DIV = 256
    U_PCLK = __SYSTEM_CLOCK / (1 + (MULT / DIV))
    1 + (MULT / DIV) = __SYSTEM_CLOCK / U_PCLK
    MULT = round((__SYSTEM_CLOCK / U_PCLK - 1) * DIV)

Since we only can do integer math in #define we multiply by DIV first.
The rounding we implement by adding the divisor / 2 to the nominator.

    MULT = ((DIV *__SYSTEM_CLOCK) + (U_PCLK / 2)) / U_PCLK - DIV

Note that we need 64 bit math for that!

    For 12 MHZ MULT is 22
    For 30 MHZ MULT is 4

With the given MULT we can calculate the BRG values for other baudrates:

    U_PCLK = (__SYSTEM_CLOCK / UARTCLKDIV) / (1 + (MULT / DIV))
    BRGVAL = U_PCLK / (BAUDRATE * 16) - 1

Again we have to round by adding BAUDRATE * 16 / 2 to the nominator:

    BRGVAL = (U_PCLK + (BAUDRATE * 16 / 2)) / (BAUDRATE * 16) - 1

*/
#define MAX_BAUDRATE ((uint64_t)115200)
#define U_DIV ((uint64_t)256)
#define BRGVAL_MAXBAUD ((HAL_SYSTEM_CLOCK / (MAX_BAUDRATE * 16)) - 1)
#define U_PCLK (MAX_BAUDRATE * 16 * (BRGVAL_MAXBAUD + 1))
#define U_MULT ((((HAL_SYSTEM_CLOCK * U_DIV) + (U_PCLK / 2)) / U_PCLK) - U_DIV)

#define U_PCLK_ACTUAL ((HAL_SYSTEM_CLOCK * U_DIV) / (U_DIV + U_MULT))

#define BRGVAL(x) ((U_PCLK_ACTUAL + (x * 8))/ (x * 16) - 1)

#define UART_CFG_ENABLE (1 << 0)
#define UART_CFG_DATALEN(d) ((unsigned)((d) - 7) << 2)
#define UART_CFG_8N1 (UART_CFG_DATALEN(8) | (0 << 4) | (0 << 6))
#define UART_CFG_8E2 (UART_CFG_DATALEN(8) | (2 << 4) | (1 << 6))
#define UART_CFG_RXPOL (1 << 22)

#define UART_STAT_RXRDY (1 << 0)
#define UART_STAT_TXRDY (1 << 2)
#define UART_STAT_TXIDLE (1 << 3)


// ****************************************************************************
void HAL_uart_init(uint32_t baudrate, uint8_t rx_pin, uint8_t tx_pin, bool invert_100000)
{
    // Defaults: 115200 8N1
    uint32_t uart_brg = BRGVAL(115200);
    uint32_t uart_cfg = UART_CFG_8N1 | UART_CFG_ENABLE;

    // Configure RX and TX pins
    LPC_SWM->PINASSIGN0 = (0xff << 24) |
                          (0xff << 16) |
                          (rx_pin << 8) |
                          (tx_pin << 0);

    // Turn on peripheral clocks for UART0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);

    // Toggle peripheral reset for USART0
    LPC_SYSCON->PRESETCTRL &= ~(1 << 3);
    LPC_SYSCON->PRESETCTRL |=  (1 << 3);

    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;
    LPC_SYSCON->UARTFRGMULT = U_MULT;

    if (baudrate == 100000) {
        uart_brg = BRGVAL(100000);

        uart_cfg = UART_CFG_8E2 | UART_CFG_ENABLE;

        // Set RXPOL to invert UART receiver polarity for S.Bus. Note that
        // RXPOL only works on LPC832
        if (invert_100000) {
            uart_cfg |= UART_CFG_RXPOL;
        }
    }
    else if (baudrate == 38400) {
       uart_brg = BRGVAL(38400);
    }

    LPC_USART0->BRG = uart_brg;
    LPC_USART0->CFG = uart_cfg;

    LPC_USART0->INTENSET = (1 << 0);    // Enable RXRDY interrupt
    NVIC_EnableIRQ(UART0_IRQn);
}


// ****************************************************************************
void UART0_irq_handler(void)
{
    receive_buffer[write_index++] = (uint8_t)LPC_USART0->RXDATA;

    // Wrap around the write pointer. This works because the buffer size is
    // a modulo of 2.
    write_index &= RECEIVE_BUFFER_INDEX_MASK;

    // If we are bumping into the read pointer we are dealing with a buffer
    // overflow. Back off and rather destroy the last value.
    if (write_index == read_index) {
        receive_buffer_overflow = true;
        write_index = (write_index - 1) & RECEIVE_BUFFER_INDEX_MASK;
    }
}


// ****************************************************************************
void HAL_putc(void *p, char c)
{
    // Ignore diagnostics requests if disabled
    if (p == NULL) {
        return;
    }

    while (!(LPC_USART0->STAT & UART_STAT_TXRDY));
    LPC_USART0->TXDATA = c;
}


// ****************************************************************************
bool HAL_getchar_pending(void)
{
    if (receive_buffer_overflow) {
        receive_buffer_overflow = false;
        printf("bufovf\n");
    }

    if (LPC_USART0->STAT & (1 << 8)) {
        // printf("overrun\n");
        LPC_USART0->STAT = (1 << 8);
    }
    if (LPC_USART0->STAT & (1 << 13)) {
        // printf("frameerr\n");
        LPC_USART0->STAT = (1 << 13);
    }
    if (LPC_USART0->STAT & (1 << 14)) {
        // printf("parityerr\n");
        LPC_USART0->STAT = (1 << 14);
    }
    if (LPC_USART0->STAT & (1 << 15)) {
        // printf("noise\n");
        LPC_USART0->STAT = (1 << 15);
    }

    return (read_index != write_index);
}


// ****************************************************************************
uint8_t HAL_getchar(void)
{
    uint8_t data;

    while (!HAL_getchar_pending());

    data = receive_buffer[read_index];

    __disable_irq();
    ++read_index;
    // Wrap around the read pointer of necessary. Only works because the buffer
    // size is a modulus of 2
    read_index &= RECEIVE_BUFFER_INDEX_MASK;
    __enable_irq();

    return data;
}


// ****************************************************************************
volatile const uint32_t *HAL_persistent_storage_read(void)
{
    return persistent_data;
}


// ****************************************************************************
const char *HAL_persistent_storage_write(const uint32_t *new_data)
{
    // NOTE: on LPC832 cmd and result can not be the same memory, despite
    // the user manual Rev 1.1. stating
    //
    //      The user can reuse the command table for results by passing
    //      the same pointer in both registers, r0 and r1.
    //
    // When we do this, then regardless of the command the return status
    // is always INVALID_COMMAND (1).
    //
    // Sharing cmd and result was fine on LPC812, but apparently not on LPC832.

    unsigned int cmd[5];
    unsigned int result[5];

    cmd[0] = 50;
    cmd[1] = ((unsigned int)persistent_data) >> 10;
    cmd[2] = ((unsigned int)persistent_data) >> 10;
    __disable_irq();
    iap_entry(cmd, result);
    __enable_irq();
    if (result[0] != 0) {
        return "prep sec";
    }

    cmd[0] = 59;  // Erase page command
    cmd[1] = ((unsigned int)persistent_data) >> 6;
    cmd[2] = ((unsigned int)persistent_data) >> 6;
    cmd[3] = HAL_SYSTEM_CLOCK / 1000;
    __disable_irq();
    iap_entry(cmd, result);
    __enable_irq();
    if (result[0] != 0) {
        return "clr pg";
    }

    cmd[0] = 50;
    cmd[1] = ((unsigned int)persistent_data) >> 10;
    cmd[2] = ((unsigned int)persistent_data) >> 10;
    __disable_irq();
    iap_entry(cmd, result);
    __enable_irq();
    if (result[0] != 0) {
        return "prep sec";
    }

    cmd[0] = 51;  // Copy RAM to Flash command
    cmd[1] = (unsigned int)persistent_data;
    cmd[2] = (unsigned int)new_data;
    cmd[3] = 64;
    cmd[4] = HAL_SYSTEM_CLOCK / 1000;
    __disable_irq();
    iap_entry(cmd, result);
    __enable_irq();
    if (result[0] != 0) {
        return "write";
    }

    return NULL;
}


// ****************************************************************************
void HAL_servo_output_init(uint8_t pin)
{
    LPC_SCT->CONFIG |= (1 << 18);           // Auto-limit on counter H
    LPC_SCT->CTRL_H |= (1 << 3) |           // Clear the counter H
                       (11 << 5);           // PRE_H[12:5] = 12-1 (SCTimer H clock 1 MHz)
    LPC_SCT->MATCHREL[0].H = 20000 - 1;     // 20 ms per overflow (50 Hz)
    LPC_SCT->MATCHREL[1].H = 1500;          // Servo pulse 1.5 ms intially

    LPC_SCT->EVENT[4].STATE = 0xFFFF;       // Event 4 happens in all states
    LPC_SCT->EVENT[4].CTRL = (0 << 0) |     // Match register 0
                             (1 << 4) |     // Select H counter
                             (0x1 << 12);   // Match condition only

    LPC_SCT->EVENT[5].STATE = 0xFFFF;       // Event 5 happens in all states
    LPC_SCT->EVENT[5].CTRL = (1 << 0) |     // Match register 1
                             (1 << 4) |     // Select H counter
                             (0x1 << 12);   // Match condition only

    // We've chosen CTOUT_1 because CTOUT_0 resides in PINASSIGN6, which
    // changing may affect CTIN_1..3 that we need.
    // CTOUT_1 is in PINASSIGN7, where no other function is needed for our
    // application.
    LPC_SCT->OUT[1].SET = (1 << 4);        // Event 4 will set CTOUT_1
    LPC_SCT->OUT[1].CLR = (1 << 5);        // Event 5 will clear CTOUT_1

    if (mcu_type == 0x812) {
        LPC_SWM->PINASSIGN7 = (0xff << 24) |    // I2C_SDA
                              (0xff << 16) |    // CTOUT_3
                              (0xff << 8) |     // CTOUT_2
                              (pin << 0);       // CTOUT_1
    }
    else {
        LPC_SWM->PINASSIGN8 = (0xff << 24) |    // CTOUT_4
                              (0xff << 16) |    // CTOUT_3
                              (0xff << 8) |     // CTOUT_2
                              (pin << 0);       // CTOUT_1
    }
}


// ****************************************************************************
// Put the servo pulse duration in microseconds into the match register
// to output the pulse of the given duration.
void HAL_servo_output_set_pulse(uint16_t servo_pulse)
{
    LPC_SCT->MATCHREL[1].H = servo_pulse;
}


// ****************************************************************************
void HAL_servo_output_enable(void)
{
    // Re-enable event 4 to set CTOUT_1
    LPC_SCT->OUT[1].SET = (1 << 4);
}


// ****************************************************************************
void HAL_servo_output_disable(void)
{
    // Turn off the setting of CTOUT_1, so no pulse will be generated. However,
    // clearing of CTOUT_1 is still active through event 0, so if a pulse is
    // currently active it will be nicely terminated, and from the next period
    // onwards the pulses will cease.
    LPC_SCT->OUT[1].SET = 0;
}


/******************************************************************************

    This module reads the servo pulses for steering, throttle and up to 3 AUX
    channels from a receiver.


    Internal operation for reading servo pulses:
    --------------------------------------------
    The SCTimer in 16-bit mode is utilized.
    We use 4 events, 4 capture registers, and 4 CTIN signals connected to the
    servo input pins. The 16 bit timer L is running at 2 MHz, giving us a
    resolution of 0.5 us.

    At rest, the capture registers wait for a rising edge.
    When an edge is detected the value is retrieved from the capture register
    and stored in a holding place. The edge of the capture block is toggled.

    When a falling edge is detected we calculate the difference (taking
    overflow into account) and store it in a result registers (raw_data, one
    per channel).

    In order to be able to handle missing channels we do the following:

    Each channel has a flag that gets set on the rising edge.
    When a channel sees its flag set at a rising edge it clears the
    flags of the *other* channels, but leaves its own flat set. It then copies
    all result registers into transfer registers (one per channel) and sets a
    flag to let the mainloop know that a set of data is available.
    The result registers are cleared.

    This way the first channel that outputs data will dictate the repeat
    frequency of the combined set of channels. If this "dominant channel"
    goes missing, another channel will take over after two pulses.

    Missing channels will have the value 0 in raw_data, active channels the
    measured pulse duration in microseconds.

    The downside of the algorithm is that there is a one frame delay
    of the output, but it is very robust for all kind of receivers.

******************************************************************************/
void HAL_servo_reader_init(void)
{
    int i;

    LPC_SCT->CTRL_L |= (1 << 3) |   // Clear the counter L
                       (5 << 5);    // PRE_L[12:5] = 6-1 (SCTimer L clock 2 MHz)


    // Configure registers 1..3 to capture servo pulses on SCTimer L
    for (i = 0; i <= 3; i++) {
        LPC_SCT->REGMODE_L |= (1 << i);         // Register i is capture register

        LPC_SCT->EVENT[i].STATE = 0xFFFF;       // Event i happens in all states
        LPC_SCT->EVENT[i].CTRL = (0 << 5) |     // OUTSEL: select input selected by IOSEL
                                 (i << 6) |     // IOSEL: CTIN_i
                                 (0x1 << 10) |  // IOCOND: rising edge
                                 (0x2 << 12);   // COMBMODE: Uses the specified I/O condition only
        LPC_SCT->CAPCTRL[i].L = (1 << i);       // Event i loads capture register i

        // Only enable EVENT0, which we use for AUX2 and AUX3, if the multi-aux
        // configuration is set.
        if (i != 0  ||  config.flags2.multi_aux) {
            LPC_SCT->EVEN |= (1 << i);              // Event i generates an interrupt
        }
    }

    // If we are using the 5-channel Pre-Processor with switching outputs, we
    // have to use a different pin for AUX2
    aux2_pin = HAL_gpio_read(HAL_GPIO_HARDWARE_CONFIG) ? HAL_GPIO_AUX2.pin : HAL_GPIO_AUX2_S.pin;

    // We keep AUX2 in CTIN_0 as it is a lone value in PINASSIGN5. We can
    // therefore easily swap AUX2 and AUX3 without having to worry of other
    // fields in the register.
    if (mcu_type != 0x812) {
        LPC_SWM->PINASSIGN6 = (aux2_pin << 24) |            // CTIN_0
                              (0xff << 16) |                // SPI1_SSEL1
                              (0xff << 8) |                 // SPI1_SSEL0
                              (0xff << 0);                  // SPI1_MISO

        LPC_SWM->PINASSIGN7 = (0xff << 24) |                // CTOUT_0
                              (HAL_GPIO_AUX.pin << 16) |    // CTIN_3
                              (HAL_GPIO_TH.pin << 8) |      // CTIN_2
                              (HAL_GPIO_ST.pin << 0);       // CTIN_1

        // The LPC832 has an additional SCT input multiplexer that we need
        // to initialize to the CTIN_0 .. CTIN_3
        LPC_INMUX->INP[0] = 0x0;
        LPC_INMUX->INP[1] = 0x1;
        LPC_INMUX->INP[2] = 0x2;
        LPC_INMUX->INP[3] = 0x3;
    }
    else {
        LPC_SWM->PINASSIGN5 = (aux2_pin << 24) |            // CTIN_0
                              (0xff << 16) |                // SPI1_SSEL
                              (0xff << 8) |                 // SPI1_MISO
                              (spi1_mosi_pin << 0);         // SPI1_MOSI

        LPC_SWM->PINASSIGN6 = (0xff << 24) |                // CTOUT_0
                              (HAL_GPIO_AUX.pin << 16) |    // CTIN_3
                              (HAL_GPIO_TH.pin << 8) |      // CTIN_2
                              (HAL_GPIO_ST.pin << 0);       // CTIN_1
    }

    NVIC_EnableIRQ(SCT_IRQn);
}


// ****************************************************************************
static void swap_aux2_aux3(void)
{
    if (milliseconds < aux2_aux3_timeout) {
        return;
    }
    aux2_aux3_timeout = milliseconds + 30;


    // Disable Event 0 (AUX2/AUX3) generating an interrupt
    LPC_SCT->EVEN &= ~(1 << 0);

    if (!aux3_active) {
        aux3_active = true;
        if (mcu_type != 0x812) {
            LPC_SWM->PINASSIGN6 = (HAL_GPIO_AUX3.pin << 24) |   // CTIN_0
                                  (0xff << 16) |                // SPI1_SSEL0
                                  (0xff << 8) |                 // SPI1_SSEL1
                                  (0xff << 0);                  // SPI1_MISO
        }
        else {
            LPC_SWM->PINASSIGN5 = (HAL_GPIO_AUX3.pin << 24) |   // CTIN_0
                                  (0xff << 16) |                // SPI1_SSEL
                                  (0xff << 8) |                 // SPI1_MISO
                                  (spi1_mosi_pin << 0);         // SPI1_MOSI
        }

    }
    else {
        aux3_active  = false;
        if (mcu_type != 0x812) {
            LPC_SWM->PINASSIGN6 = (aux2_pin << 24) |            // CTIN_0
                                  (0xff << 16) |                // SPI1_SSEL0
                                  (0xff << 8) |                 // SPI1_SSEL1
                                  (0xff << 0);                  // SPI1_MISO
        }
        else {
            LPC_SWM->PINASSIGN5 = (aux2_pin << 24) |            // CTIN_0
                                  (0xff << 16) |                // SPI1_SSEL
                                  (0xff << 8) |                 // SPI1_MISO
                                  (spi1_mosi_pin << 0);         // SPI1_MOSI
        }
    }

    // Generate intererupt on rising edge
    LPC_SCT->EVENT[0].CTRL = (0 << 5) |     // OUTSEL: select input selected by IOSEL
                             (0 << 6) |     // IOSEL: CTIN_0
                             (0x1 << 10) |  // IOCOND: rising edge
                             (0x2 << 12);   // COMBMODE: Uses the specified I/O condition only

    // Clear any potentially pending interrupts
    LPC_SCT->EVFLAG = (1 << 0);

    // Re-enable Event 0 generating an interrupt
    LPC_SCT->EVEN |= (1 << 0);
}


// ****************************************************************************
bool HAL_servo_reader_get_new_channels(uint32_t *out)
{
    if (config.flags2.multi_aux) {
        swap_aux2_aux3();
    }

    if (!new_raw_channel_data) {
        return false;
    }

    new_raw_channel_data = false;
    out[0] = raw_data[0] >> 1;
    out[1] = raw_data[1] >> 1;
    // Only output AUX if it is not configured as local switch
    out[2] = (config.flags.ch3_is_local_switch) ? 1500 : (raw_data[2] >> 1);
    out[3] = raw_data[3] >> 1;
    out[4] = raw_data[4] >> 1;
    return true;
}


// ****************************************************************************
static void output_raw_channels(uint16_t result[5])
{
    raw_data[AUX2] = result[0];
    raw_data[ST] = result[1];
    raw_data[TH] = result[2];
    raw_data[AUX] = result[3];
    raw_data[AUX3] = result[4];


    // Do not clear the results, rather keep them at their current value. This
    // is important for receivers that output the 3 channels asynchronously or
    // at different rates, like the Spektrum 4210 and 4220.

    // result[0] = result[1] = result[2] = 0;

    new_raw_channel_data = true;
}


// ****************************************************************************
void SCT_irq_handler(void)
{
    static uint16_t start[5] = {0, 0, 0, 0, 0};
    static uint16_t result[5] = {0, 0, 0, 0, 0};
    static uint8_t channel_flags = 0;
    uint16_t capture_value;

    int event_index;
    int storage_index;

    for (event_index = 0; event_index <= 3; event_index++) {

        // EVENT0 / CTIN_0 is multiplexed between AUX2 and AUX3. So depending
        // on which one we are currently processing we set the corresponding
        // storage index into start[]/result[]/channel_flags[] to 0 (AUX2) or
        // 4 (AUX3).
        storage_index = event_index;
        if (event_index == 0 && aux3_active) {
            storage_index = 4;
        }

        // Event event_index: Capture CTIN_event_index
        if (LPC_SCT->EVFLAG & (1 << event_index)) {
            capture_value = LPC_SCT->CAP[event_index].L;

            if (LPC_SCT->EVENT[event_index].CTRL & (0x1 << 10)) {
                // Rising edge triggered

                start[storage_index] = capture_value;

                if (channel_flags & (1 << storage_index)) {
                    output_raw_channels(result);
                    channel_flags = (1 << storage_index);
                }
                channel_flags |= (1 << storage_index);
            }
            else {
                // Falling edge triggered
                if (start[storage_index] > capture_value) {
                    // Compensate for wrap-around
                    capture_value += LPC_SCT->MATCHREL[0].L + 1;
                }
                result[storage_index] = capture_value - start[storage_index];

                // AUX2/3 captured? Trigger a swap between AUX2 and AUX3 immediately
                if (event_index == 0) {
                    aux2_aux3_timeout = 0;
                }
            }

            // IOCOND: toggle edge
            LPC_SCT->EVENT[event_index].CTRL ^= (0x3 << 10);

            // Clear the event flag
            LPC_SCT->EVFLAG = (1 << event_index);
        }
    }
}


// ****************************************************************************
void HAL_spi_init(void)
{
    HAL_gpio_set(HAL_GPIO_XLAT);

    HAL_gpio_out_mask(
        (1 << HAL_GPIO_SCK.pin) |
        (1 << HAL_GPIO_SIN.pin) |
        (1 << HAL_GPIO_SIN_LPC832.pin) |
        (1 << HAL_GPIO_XLAT.pin));

    // Use 2 MHz SPI clock. 16 bytes take about 50 us to transmit.
    LPC_SPI0->DIV = (HAL_SYSTEM_CLOCK / 2000000) - 1;

    LPC_SPI0->CFG = (1 << 0) |          // Enable SPI0
                    (1 << 2) |          // Master mode
                    (0 << 3) |          // LSB First mode disabled
                    (0 << 4) |          // CPHA = 0
                    (0 << 5) |          // CPOL = 0
                    (0 << 8);           // SPOL = 0

    LPC_SPI0->TXCTRL = (1 << 21) |      // set EOF
                       (1 << 22) |      // RXIGNORE, otherwise SPI hangs until
                                        //   we read the data register
                       ((6 - 1) << 24); // 6 bit frames

    // We use the SSEL function for XLAT: low during the transmission, high
    // during the idle periood.
    LPC_SWM->PINASSIGN3 = (HAL_GPIO_SCK.pin << 24) |        // SCK
                          (0xff << 16) |
                          (0xff << 8) |
                          (0xff << 0);

    // SIN is on a different pin on the LPC832 based hardware!
    LPC_SWM->PINASSIGN4 = (0xff << 24) |
                          (HAL_GPIO_XLAT.pin << 16) |               // XLAT (SSEL)
                          (0xff << 8) |
                          (mcu_type == 0x812 ?                      // SIN (MOSI)
                            (HAL_GPIO_SIN.pin << 0) :
                            (HAL_GPIO_SIN_LPC832.pin << 0));
}


// ****************************************************************************
void HAL_spi_transaction(uint8_t *data, uint8_t count)
{
    int i;

    // Wait for MSTIDLE, should be a no-op since we are waiting after
    // the transfer.
    while (!(LPC_SPI0->STAT & (1 << 8)));

    for (i = count; i >= 1; i--) {
        // Wait for TXRDY
        while (!(LPC_SPI0->STAT & (1 << 1)));

        LPC_SPI0->TXDAT = data[i - 1];
    }

    // Force END OF TRANSFER
    LPC_SPI0->STAT = (1 << 7);

    // Wait for the transfer to finish
    while (!(LPC_SPI0->STAT & (1 << 8)));
}


// ****************************************************************************
void HAL_ws2811_init(uint8_t tx_pin)
{
    spi1_mosi_pin = tx_pin;
    HAL_gpio_out_mask((1 << spi1_mosi_pin));

    // // Use 3 MHz SPI clock. This means a single 'ws2811-bit' takes 3 bits
    // // over SPI. We send two ws2811-bits in a single SPI frame (6 bit frame).
    LPC_SPI1->DIV = (HAL_SYSTEM_CLOCK / 3000000) - 1;

    LPC_SPI1->CFG = (1 << 0) |          // Enable SPI0
                    (1 << 2) |          // Master mode
                    (0 << 3) |          // LSB First mode disabled
                    (0 << 4) |          // CPHA = 0
                    (0 << 5) |          // CPOL = 0
                    (0 << 8);           // SPOL = 0

    LPC_SPI1->TXCTRL = (1 << 21) |      // set EOF
                       (1 << 22) |      // RXIGNORE, otherwise SPI hangs until
                                        //   we read the data register
                       ((6 - 1) << 24); // 6 bit frames

    if (mcu_type != 0x812) {
        LPC_SWM->PINASSIGN5 = (spi1_mosi_pin << 24) |  // SCK
                              (0xff << 16) |    // SPI1_SCK
                              (0xff << 8) |     // SPI0_SSEL3
                              (0xff << 0);      // SPI0_SSEL2
    }
    else {
        LPC_SWM->PINASSIGN5 = (aux2_pin << 24) |            // CTIN_0
                              (0xff << 16) |                // SPI1_SSEL
                              (0xff << 8) |                 // SPI1_MISO
                              (spi1_mosi_pin << 0);                // SPI1_MOSI
    }
}


// ****************************************************************************
void HAL_ws2811_transaction(uint8_t *data, uint8_t count)
{
    int i;
    int j;

    const uint8_t LOW = 0x5;    // 0b100;
    const uint8_t HIGH = 0x6;   // 0b110;

    // Wait for MSTIDLE, should be a no-op since we are waiting after
    // the transfer.
    while (!(LPC_SPI1->STAT & (1 << 8)));

    for (i = 0; i < count; i++) {

        // Transmit two ws2811-bits in a single SPI frame (6 bit frame)
        for (j = 0; j < 4 ; j++) {
            uint8_t value;

            value = (data[i] & 0x80 ? HIGH : LOW) << 3;
            value |= data[i] & 0x40 ? HIGH : LOW;
            data[i] = data[i] << 2;

            // Wait for TXRDY
            while (!(LPC_SPI1->STAT & (1 << 1)));

            LPC_SPI1->TXDAT = value;
        }
    }

    // Force END OF TRANSFER
    LPC_SPI1->STAT = (1 << 7);

    // Wait for the transfer to finish
    while (!(LPC_SPI1->STAT & (1 << 8)));
}


// ****************************************************************************
bool HAL_switch_triggered(void)
{
    static bool transitioned = false;

    if (!config.flags.ch3_is_local_switch) {
        return false;
    }

    if (transitioned) {
        if (HAL_gpio_read(HAL_GPIO_AUX)) {
            transitioned = false;
        }
    }
    else {
        if (!HAL_gpio_read(HAL_GPIO_AUX)) {
            transitioned = true;
            return true;
        }
    }
    return false;
}