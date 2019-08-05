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


#define RECEIVE_BUFFER_SIZE (16)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)


__attribute__ ((section(".persistent_data")))
static volatile const uint32_t persistent_data[HAL_NUMBER_OF_PERSISTENT_ELEMENTS];

volatile uint32_t milliseconds;

static bool diagnostics_on_uart;

static uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t read_index = 0;
static volatile uint16_t write_index = 0;

static volatile bool new_raw_channel_data = false;

static uint32_t raw_data[3];


// ****************************************************************************
void SysTick_handler(void)
{
    ++milliseconds;
}


// ****************************************************************************
void HardFault_handler(void)
{
    HAL_putc(NULL, 'H');
    HAL_putc(NULL, '\n');
    while (1);
}


// ****************************************************************************
void HAL_hardware_init(bool is_servo_reader, bool servo_output_enabled, bool uart_output_enabled)
{
#if HAL_SYSTEM_CLOCK != 12000000
#error Clock initialization code expexts __SYSTEM_CLOCK to be set to 1200000
#endif

    // Turn on brown-out detection and reset
    LPC_SYSCON->BODCTRL = (1 << 4) | (1 << 2) | (3 << 0);


    // Set flash wait-states to 1 system clock
    LPC_FLASHCTRL->FLASHCFG = 0;


    // Turn on peripheral clocks for SCTimer, IOCON, SPI0
    // (GPIO, SWM alrady enabled after reset)
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 8) | (1 << 18) | (1 << 11);


    // ------------------------
    // IO configuration

    // Enable reset, all other special functions disabled
    LPC_SWM->PINENABLE0 = 0xffffffbf;

    // Configure the UART input and output
    if (is_servo_reader) {
        // Turn the UART output on unless a servo output is requested
        if (!servo_output_enabled) {
            // U0_TXT_O=PIO0_12 (OUT/ISP)
            LPC_SWM->PINASSIGN0 = (0xff << 24) |
                                  (0xff << 16) |
                                  (0xff << 8) |
                                  (HAL_GPIO_OUT.pin << 0);
        }
    }
    else {
        // U0_TXT_O=PIO0_4 (TH), U0_RXD_I=PIO0_0 (ST)
        LPC_SWM->PINASSIGN0 = (0xff << 24) |
                              (0xff << 16) |
                              (HAL_GPIO_ST.pin << 8) |
                              (HAL_GPIO_TH.pin << 0);
    }

    // Make the open drain ports PIO0_10, PIO0_11 outputs and pull to ground
    // to prevent them from floating.
    HAL_gpio_clear(HAL_GPIO_PIN10);
    HAL_gpio_out(HAL_GPIO_PIN10);

    HAL_gpio_clear(HAL_GPIO_PIN11);
    HAL_gpio_out(HAL_GPIO_PIN11);

    // Make the switched light output PIO0_9 an output and shut it off.
    HAL_gpio_clear(HAL_GPIO_SWITCHED_LIGHT_OUTPUT);
    HAL_gpio_out(HAL_GPIO_SWITCHED_LIGHT_OUTPUT);


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


    // ------------------------
    // Configure SCTimer globally for two 16-bit counters
    LPC_SCT->CONFIG = 0;


    // ------------------------
    // SysTick configuration 1000 Hz / 1 ms
    SysTick_Config((HAL_SYSTEM_CLOCK / 1000)- 1);


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
}


// ****************************************************************************
void HAL_hardware_init_final(void)
{
    // Turn off peripheral clock for IOCON and SWM to preserve power
    LPC_SYSCON->SYSAHBCLKCTRL &= ~((1 << 18) | (1 << 7));
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
#define UART_STAT_RXRDY (1 << 0)
#define UART_STAT_TXRDY (1 << 2)
#define UART_STAT_TXIDLE (1 << 3)


// ****************************************************************************
void HAL_uart_init(uint32_t baudrate)
{
    // Turn on peripheral clocks for UART0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);

    // Toggle peripheral reset for USART0
    LPC_SYSCON->PRESETCTRL &= ~(1 << 3);
    LPC_SYSCON->PRESETCTRL |=  (1 << 3);

    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;
    LPC_SYSCON->UARTFRGMULT = U_MULT;

    if (baudrate == 115200) {
        LPC_USART0->BRG = BRGVAL(115200);
    }
    else {
        LPC_USART0->BRG = BRGVAL(38400);
    }

    LPC_USART0->CFG = UART_CFG_DATALEN(8) | UART_CFG_ENABLE;     // 8n1

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
        write_index = (write_index - 1) & RECEIVE_BUFFER_INDEX_MASK;
    }
}


// ****************************************************************************
void HAL_putc(void *p, char c)
{
    // Ignore diagnostics requests if disabled
    if (!diagnostics_on_uart  &&  p == STDOUT_DEBUG) {
        return;
    }

    while (!(LPC_USART0->STAT & UART_STAT_TXRDY));
    LPC_USART0->TXDATA = c;
}


// ****************************************************************************
bool HAL_getchar_pending(void)
{
    if (LPC_USART0->STAT & (1 << 8)) {
        // uart0_send_cstring("overrun\n");
        LPC_USART0->STAT |= (1 << 8);
    }
    if (LPC_USART0->STAT & (1 << 13)) {
        // uart0_send_cstring("frameerr\n");
        LPC_USART0->STAT |= (1 << 13);
    }
    if (LPC_USART0->STAT & (1 << 15)) {
        // uart0_send_cstring("noise\n");
        LPC_USART0->STAT |= (1 << 15);
    }

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
volatile const uint32_t *HAL_persistent_storage_read(void)
{
    return persistent_data;
}


// ****************************************************************************
const char *HAL_persistent_storage_write(const uint32_t *new_data)
{
    unsigned int param[5];

    param[0] = 50;
    param[1] = ((unsigned int)persistent_data) >> 10;
    param[2] = ((unsigned int)persistent_data) >> 10;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: prepare sector failed";
    }

    param[0] = 59;  // Erase page command
    param[1] = ((unsigned int)persistent_data) >> 6;
    param[2] = ((unsigned int)persistent_data) >> 6;
    param[3] = HAL_SYSTEM_CLOCK / 1000;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: erase page failed";
    }

    param[0] = 50;
    param[1] = ((unsigned int)persistent_data) >> 10;
    param[2] = ((unsigned int)persistent_data) >> 10;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: prepare sector failed";
    }

    param[0] = 51;  // Copy RAM to Flash command
    param[1] = (unsigned int)persistent_data;
    param[2] = (unsigned int)new_data;
    param[3] = 64;
    param[4] = HAL_SYSTEM_CLOCK / 1000;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: copy RAM to flash failed";
    }

    return NULL;
}


// ****************************************************************************
void HAL_servo_output_init(void)
{
    LPC_SCT->CONFIG |= (1 << 18);           // Auto-limit on counter H
    LPC_SCT->CTRL_H |= (1 << 3) |           // Clear the counter H
                       (11 << 5);           // PRE_H[12:5] = 12-1 (SCTimer H clock 1 MHz)
    LPC_SCT->MATCHREL[0].H = 20000 - 1;     // 20 ms per overflow (50 Hz)
    LPC_SCT->MATCHREL[4].H = 1500;          // Servo pulse 1.5 ms intially

    LPC_SCT->EVENT[0].STATE = 0xFFFF;       // Event 0 happens in all states
    LPC_SCT->EVENT[0].CTRL = (0 << 0) |     // Match register 0
                             (1 << 4) |     // Select H counter
                             (0x1 << 12);   // Match condition only

    LPC_SCT->EVENT[4].STATE = 0xFFFF;       // Event 4 happens in all states
    LPC_SCT->EVENT[4].CTRL = (4 << 0) |     // Match register 4
                             (1 << 4) |     // Select H counter
                             (0x1 << 12);   // Match condition only

    // We've chosen CTOUT_1 because CTOUT_0 resides in PINASSIGN6, which
    // changing may affect CTIN_1..3 that we need.
    // CTOUT_1 is in PINASSIGN7, where no other function is needed for our
    // application.
    LPC_SCT->OUT[1].SET = (1 << 0);        // Event 0 will set CTOUT_1
    LPC_SCT->OUT[1].CLR = (1 << 4);        // Event 4 will clear CTOUT_1

    // CTOUT_1 = PIO0_12
    LPC_SWM->PINASSIGN7 = 0xffffff0c;

    LPC_SCT->CTRL_H &= ~(1 << 2);          // Start the SCTimer H
}


// ****************************************************************************
// Put the servo pulse duration in milliseconds into the match register
// to output the pulse of the given duration.
void HAL_servo_output_set_pulse(uint16_t servo_pulse)
{
    LPC_SCT->MATCHREL[4].H = servo_pulse;
}


// ****************************************************************************
void HAL_servo_output_enable(void)
{
    // Re-enable event 0 to set CTOUT_1
    LPC_SCT->OUT[1].SET = (1 << 0);
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

    This module reads the servo pulses for steering, throttle and CH3/AUX
    from a receiver.

    It can also read the pulses from a CPPM output, provided your receiver
    has such an output.


    Internal operation for reading servo pulses:
    --------------------------------------------
    The SCTimer in 16-bit mode is utilized.
    We use 3 events, 3 capture registers, and 3 CTIN signals connected to the
    servo input pins. The 16 bit timer L is running at 2 MHz, giving us a
    resolution of 0.5 us.

    At rest, the 3 capture registers wait for a rising edge.
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
    of the output, but it is very robust for use in the pre-processor.


    Internal operation for reading CPPM:
    ------------------------------------
    The SCTimer in 16-bit mode is utilized.
    We use 1 event, 1 capture register, and the CTIN_1 signals connected to the
    ST servo input pin. The 16 bit timer L is running at 2 MHz, giving us a
    resolution of 0.5 us.

    The capture register is setup to trigger of falling edges of the CPPM
    signal. Every time an interrupt occurs the time duration from the
    previous pulse is calculated.

    If that time difference is larger than the largest servo pulse we expect
    (which is 2.5 ms) then we know that a new CPPM "frame" has started and
    we set our state-machine so that the next edge is stored as CH1, then
    the next as CH2, and one more edge as CH3. After we received all
    3 channels we update the rest of the light controller with the new
    data and setup the CPPM reader to wait for a frame sync signal (= >2/5ms
    between interrupts). Smaller pulses (i.e. because the receiver outputs
    more than 3 channles) are ignored.

    In case the receiver outputs less than 3 channels, the frame detection
    function outputs the channels that have been received so far.

******************************************************************************/
static void output_raw_channels(uint16_t result[3])
{
    raw_data[0] = result[0] ;
    raw_data[1] = result[1] ;
    raw_data[2] = result[2] ;


    // Do not clear the results, rather keep them at their current value. This
    // is important for receivers that output the 3 channels asynchronously or
    // at different rates, like the Spektrum 4210 and 4220.

    // result[0] = result[1] = result[2] = 0;

    new_raw_channel_data = true;
}


// ****************************************************************************
bool HAL_servo_reader_get_new_channels(uint32_t *out)
{
    if (!new_raw_channel_data) {
        return false;
    }

    new_raw_channel_data = false;
    out[0] = raw_data[0] >> 1;
    out[1] = raw_data[1] >> 1;
    out[2] = raw_data[2] >> 1;
    return true;
}


// ****************************************************************************
void HAL_servo_reader_init(void)
{
    int i;

    // SCTimer setup
    // At this point we assume that SCTimer has been setup in the following way:
    //
    //  * Split 16-bit timers
    //  * Events 1, 2 and 3 available for our use
    //  * Registers 1, 2 and 3 available for our use
    //  * CTIN_1, CTIN_2 and CTIN3 available for our use

    LPC_SCT->CTRL_L |= (1 << 3) |   // Clear the counter L
                       (5 << 5);    // PRE_L[12:5] = 6-1 (SCTimer L clock 2 MHz)


    // Configure registers 1..3 to capture servo pulses on SCTimer L
    for (i = 1; i <= 3; i++) {
        LPC_SCT->REGMODE_L |= (1 << i);         // Register i is capture register

        LPC_SCT->EVENT[i].STATE = 0xFFFF;       // Event i happens in all states
        LPC_SCT->EVENT[i].CTRL = (0 << 5) |     // OUTSEL: select input elected by IOSEL
                                 (i << 6) |     // IOSEL: CTIN_i
                                 (0x1 << 10) |  // IOCOND: rising edge
                                 (0x2 << 12);   // COMBMODE: Uses the specified I/O condition only
        LPC_SCT->CAPCTRL[i].L = (1 << i);       // Event i loads capture register i
        LPC_SCT->EVEN |= (1 << i);              // Event i generates an interrupt
    }

    LPC_SWM->PINASSIGN6 = (0xff << 24) |
                          (HAL_GPIO_AUX.pin << 16) |    // CTIN_3
                          (HAL_GPIO_TH.pin << 8) |      // CTIN_2
                          (HAL_GPIO_ST.pin << 0);       // CTIN_1

    LPC_SCT->CTRL_L &= ~(1 << 2);               // Start the SCTimer L
    NVIC_EnableIRQ(SCT_IRQn);
}


// ****************************************************************************
void SCT_irq_handler(void)
{
    static uint16_t start[3] = {0, 0, 0};
    static uint16_t result[3] = {0, 0, 0};
    static uint8_t channel_flags = 0;
    uint16_t capture_value;

    int i;

    for (i = 1; i <= 3; i++) {
        // Event i: Capture CTIN_i
        if (LPC_SCT->EVFLAG & (1 << i)) {
            capture_value = LPC_SCT->CAP[i].L;

            if (LPC_SCT->EVENT[i].CTRL & (0x1 << 10)) {
                // Rising edge triggered
                start[i - 1] = capture_value;

                if (channel_flags & (1 << i)) {
                    output_raw_channels(result);
                    channel_flags = (1 << i);
                }
                channel_flags |= (1 << i);
            }
            else {
                // Falling edge triggered
                if (start[i - 1] > capture_value) {
                    // Compensate for wrap-around
                    capture_value += LPC_SCT->MATCHREL[0].L + 1;
                }
                result[i - 1] = capture_value - start[i - 1];
            }

            LPC_SCT->EVENT[i].CTRL ^= (0x3 << 10);   // IOCOND: toggle edge
            LPC_SCT->EVFLAG = (1 << i);
        }
    }
}


// ****************************************************************************
void HAL_spi_init(void)
{
    HAL_gpio_set(HAL_GPIO_XLAT);

    HAL_gpio_out(HAL_GPIO_SCK);
    HAL_gpio_out(HAL_GPIO_SIN);
    HAL_gpio_out(HAL_GPIO_XLAT);

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

    LPC_SWM->PINASSIGN4 = (0xff << 24) |
                          (HAL_GPIO_XLAT.pin << 16) |       // XLAT (SSEL)
                          (0xff << 8) |
                          (HAL_GPIO_SIN.pin << 0);          // SIN (MOSI)
}


// ****************************************************************************
void HAL_spi_transaction(uint8_t *data, uint8_t count)
{
    volatile uint8_t i;

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
bool HAL_switch_triggered(void)
{
    return false;
}