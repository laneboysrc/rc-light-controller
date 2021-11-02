#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "ch552.h"
#include "usb.h"
#include "ring_buffer.h"

#define BAUDRATE 115200


#define P1_MOD_OC_ADDR (0x92)
#define P1_DIR_PU_ADDR (0x93)
#define P3_MOD_OC_ADDR (0x96)
#define P3_DIR_PU_ADDR (0x97)

// LEDs need to be switched to GND to turn on
#define LED_ON (0)
#define LED_OFF (1)

#define GPIO_ADDRESS_LED_OK (P3_ADDR)
#define GPIO_PIN_LED_OK (3)
__sbit __at(GPIO_ADDRESS_LED_OK | GPIO_PIN_LED_OK) LED_OK;

#define GPIO_ADDRESS_LED_BUSY (P1_ADDR)
#define GPIO_PIN_LED_BUSY (1)
__sbit __at(GPIO_ADDRESS_LED_BUSY | GPIO_PIN_LED_BUSY) LED_BUSY;

#define GPIO_ADDRESS_LED_ERROR (P1_ADDR)
#define GPIO_PIN_LED_ERROR (5)
__sbit __at(GPIO_ADDRESS_LED_ERROR | GPIO_PIN_LED_ERROR) LED_ERROR;

#define GPIO_ADDRESS_POWER_INB (P3_ADDR)
#define GPIO_PIN_POWER_INB (2)
__sbit __at(GPIO_ADDRESS_POWER_INB | GPIO_PIN_POWER_INB) PIN_POWER_INB;

#define GPIO_ADDRESS_POWER_INA (P1_ADDR)
#define GPIO_PIN_POWER_INA (4)
__sbit __at(GPIO_ADDRESS_POWER_INA | GPIO_PIN_POWER_INA) PIN_POWER_INA;


#define GPIO_ADDRESS_CH3 (P1_ADDR)
#define GPIO_PIN_CH3 (6)
__sbit __at(GPIO_ADDRESS_CH3 | GPIO_PIN_CH3) PIN_CH3;

#define GPIO_ADDRESS_ISP (P1_ADDR)
#define GPIO_PIN_ISP (7)
__sbit __at(GPIO_ADDRESS_ISP | GPIO_PIN_ISP) PIN_ISP;


// WebUSB programmer commands received via EP0 Vendor specific request
#define CMD_DUT_POWER_OFF (10)
#define CMD_DUT_POWER_ON (11)
#define CMD_OUT_ISP_LOW (20)
#define CMD_OUT_ISP_HIGH (21)
#define CMD_OUT_ISP_TRISTATE (22)
#define CMD_CH3_LOW (23)
#define CMD_CH3_HIGH (24)
#define CMD_CH3_TRISTATE (25)
#define CMD_BAUDRATE_38400 (30)
#define CMD_BAUDRATE_115200 (31)
#define CMD_LED_OK_OFF (40)
#define CMD_LED_OK_ON (41)
#define CMD_LED_BUSY_OFF (42)
#define CMD_LED_BUSY_ON (43)
#define CMD_LED_ERROR_OFF (44)
#define CMD_LED_ERROR_ON (45)
#define CMD_PING (99)

#define WATCHDOG_TIMEOUT_MS (2000)


static __bit systick;

static uint8_t bootloader_state;
static uint8_t bootloader_timeout;

// UART transmit idle
static __bit tx_idle;

static __bit usb_enabled;

// Flag that is set until outgoing data on EP2 has been sent to the host and
// EP2Buffer is free egain
static __bit ep2_busy;

// Helper variables for processing the incoming data on EP1
static uint8_t ep1_byte_count;
static uint8_t ep1_index;

static __bit watchdog_enabled;
static uint16_t watchdog_timeout;



// ****************************************************************************
static void delay_ms(uint8_t ms)
{
    while (ms) {
        if (systick) {
            systick = false;
            --ms;
        }
    }
}


// ****************************************************************************
static void WATCHDOG_reset(void)
{
    watchdog_timeout = WATCHDOG_TIMEOUT_MS;
    watchdog_enabled = true;
}


// ****************************************************************************
static void WATCHDOG_service(void)
{
    if (!watchdog_enabled) {
        return;
    }

    if (watchdog_timeout > 0) {
        return;
    }

    watchdog_enabled = false;

    // Watchdog expired: turn the power to the light controller off
    // Switch to Hi-Z first, then to L to short the light controller power supply
    PIN_POWER_INA = 0;
    PIN_POWER_INB = 0;
    delay_ms(1);
    PIN_POWER_INA = 1;

    // Turn the LEDs off
    LED_OK = LED_OFF;
    LED_BUSY = LED_OFF;
    LED_ERROR = LED_OFF;

    // Switch the UART TX and RX to GPIO output and set it to low, so that
    // we don't power the light controller via the ST/RX pin!
    REN = 0;
    TXD = 0;
    RXD = 0;
    PIN_ISP = 0;
    PIN_CH3 = 0;
}


// ****************************************************************************
bool COMMAND_handler(uint8_t value)
{
    switch (value) {
        case CMD_DUT_POWER_ON:
            PIN_POWER_INA = 0;  // Hi-Z
            PIN_POWER_INB = 0;

            // Switch the TX and RX back to UART
            TXD = 1;
            RXD = 1;
            REN = 1;

            PIN_POWER_INB = 1;  // OUTB = high (a few us later)
            break;

        case CMD_DUT_POWER_OFF:
            PIN_POWER_INA = 0;
            PIN_POWER_INB = 0;

            // Switch the UART TX and RX to GPIO output  and set it to low, so that
            // we don't power the light controller via the ST/RX pin!
            REN = 0;
            TXD = 0;
            RXD = 0;

            PIN_POWER_INA = 1;  // OUTB = low (a few us later)
            break;

        case CMD_LED_OK_ON:
            LED_OK = LED_ON;
            break;

        case CMD_LED_OK_OFF:
            LED_OK = LED_OFF;
            break;

        case CMD_LED_BUSY_ON:
            LED_BUSY = LED_ON;
            break;

        case CMD_LED_BUSY_OFF:
            LED_BUSY = LED_OFF;
            break;

        case CMD_LED_ERROR_ON:
            LED_ERROR = LED_ON;
            break;

        case CMD_LED_ERROR_OFF:
            LED_ERROR = LED_OFF;
            break;

        case CMD_OUT_ISP_LOW:
            PIN_ISP = 0;
            P3_DIR_PU |= (1 << GPIO_PIN_ISP);   // Open drain output with pull-up
            break;

        case CMD_OUT_ISP_HIGH:
            PIN_ISP = 1;
            P3_DIR_PU |= (1 << GPIO_PIN_ISP);   // Open drain output with pull-up
            break;

        case CMD_OUT_ISP_TRISTATE:
            PIN_ISP = 1;
            P3_DIR_PU &= ~(1 << GPIO_PIN_ISP);  // High-impedance input mode
            break;

        case CMD_CH3_LOW:
            PIN_CH3 = 0;
            P3_DIR_PU |= (1 << GPIO_PIN_CH3);   // Open drain output with pull-up
            break;

        case CMD_CH3_HIGH:
            PIN_CH3 = 1;
            P3_DIR_PU |= (1 << GPIO_PIN_CH3);   // Open drain output with pull-up
            break;

        case CMD_CH3_TRISTATE:
            PIN_CH3 = 1;
            P3_DIR_PU &= ~(1 << GPIO_PIN_CH3);  // High-impedance input mode
            break;

        case CMD_BAUDRATE_38400:
            TH1 = (uint8_t)(256 - (24000000 / 16 / 38400));
            break;

        case CMD_BAUDRATE_115200:
            TH1 = (uint8_t)(256 - (24000000 / 16 / 115200));
            break;

        case CMD_PING:
            break;

        default:
            return false;
    }

    WATCHDOG_reset();
    return true;
}


// ****************************************************************************
static void UART0_init(void)
{
    SM0 = 0;        // UART Mode 1
    SM1 = 1;
    SM2 = 0;

    PCON != SMOD;   // Enable "fast mode" (256 - fsys / 16 / baudrate)

    // IMPORTANT: do not enable UART receiver at this point, only when we
    // are ready to talk to the light controller!
    // REN = 1;      // Enable UART receiver

    RCLK = 0;       // Use Timer 1
    TCLK = 0;

    TMOD = T1_MODE_2;   // Timer 1 uses Mode 2 (8-bit auto reload)
    T2MOD = bTMR_CLK | bT1_CLK;
    TH1 = (uint8_t)(256 - (24000000 / 16 / BAUDRATE));
    tx_idle = 1;

    TR1 = 1;        // Start Timer 1
}


// ****************************************************************************
void BOOTLOADER_start(void)
{
    bootloader_state = 1;
}


// ****************************************************************************
static void BOOTLOADER_service(void)
{
    switch (bootloader_state) {
        // Idle
        case 0:
            return;

        // Wait 30 ms before shutting USB down
        case 1:
            bootloader_timeout = 30;
            ++bootloader_state;
            return;

        // Wait 100 ms after shutting USB down to let the host detect that
        // the device is gone
        case 2:
            if (bootloader_timeout == 0) {
                usb_enabled = 0;
                USB_CTRL = 0;

                bootloader_timeout = 100;
                ++bootloader_state;
            }
            return;

        // Disable interrupts and jump to the bootloader
        case 3:
            if (bootloader_timeout == 0) {
                IE_TKEY = 0;
                EA = 0;
                ((void(*)(void)) (uint8_t __code *)0x3800)();
            }
            return;

        default:
            return;
    }
}


// ****************************************************************************
void DATA_sent(void)
{
    ep2_busy = false;
}


// ****************************************************************************
void DATA_received(uint8_t byte_count)
{
    ep1_byte_count = byte_count;
    ep1_index = 0;
}


// ****************************************************************************
void TouchKeyInterrupt(void) __interrupt (INT_NO_TKEY)
{
    systick = true;
    TKEY_CTRL = 0x00;
}


// ****************************************************************************
static void UART_service(void)
{
    if (TI) {
        TI = 0;
        tx_idle = 1;
    }

    if (RI) {
        RING_BUFFER_write_uint8(SBUF);
        RI = 0;
    }
}


// ****************************************************************************
static void USB_service(void)
{
    static uint8_t timeout = 0;

    if (!usb_enabled) {
        return;
    }

    USB_handle_events();

    if (active_usb_configuration == 0) {
        return;
    }

    // USB receiving endpoint has data
    if (tx_idle && ep1_byte_count) {
        tx_idle = false;
        SBUF = ep1_buffer[ep1_index];

        ++ep1_index;
        --ep1_byte_count;

        if (ep1_byte_count == 0) {
            USB_EP1_ack();
        }
    }

    // UART data received and EP2-in is not busy
    if (!ep2_busy) {
        if (RING_BUFFER_is_at_least_half_full() || timeout > 100){
            uint8_t byte_count;

            for (byte_count = 0; byte_count < EP2_SIZE; byte_count++) {
                if (RING_BUFFER_is_empty()) {
                    break;
                }
                ep2_buffer[byte_count] = RING_BUFFER_read_uint8();
            }

            if (byte_count > 0) {
                USB_EP2_send(byte_count);
                ep2_busy = 1;
            }
        }

        if (timeout < 255) {
            ++timeout;
        }
    }
}


// ****************************************************************************
static void SYSTICK_service(void)
{
    if (systick) {
        systick = false;

        if (bootloader_timeout) {
            --bootloader_timeout;
        }

        if (watchdog_timeout) {
            --watchdog_timeout;
        }
    }
}


// ****************************************************************************
static void GPIO_init(void)
{
    /*

        P1.1    Busy LED        Open drain w/ pull-up       active low
        P1.4    Power           Push/pull output            active low
        P1.5    Error LED       Open drain w/ pull-up       active low
        P1.6    CH3             Push/pull output / Hi-Z
        P1.7    OUT/ISP         Push/pull output / Hi-Z

        P3.0    RX              Push/pull output / Open drain w/ pull-up
        P3.1    TX              Push/pull output

        P3.2    SHORT           Push/pull output            active low
        P3.3    OK LED          Open drain w/ pull-up       active low

    */

    LED_OK = LED_OFF;
    LED_BUSY = LED_OFF;
    LED_ERROR = LED_OFF;

    // After reset the IOs are H/H, and OUTB is therefore L.
    // We transition into the second OUTB = L state by switching first
    // INB to L, and then INA to H (which it should be already, but still...)
    PIN_POWER_INB = 0;
    PIN_POWER_INA = 1;

    // Switch all light controller pins including UART TX and RX to low, so
    // that we don't power the light controller via the IO pins!
    PIN_ISP = 0;
    PIN_CH3 = 0;
    TXD = 0;
    RXD = 0;

    // All ports stay configured as default after reset:
    // Bidirectional mode, open-drain output, pull-up resistor on pin

    // Because we are running the MCU from 5V, the IO pins are also using 5V.
    // The light controller runs at 3.3V, so we would see a large current
    // into the light controller IOs. To protect against this we rely on the
    // 10K pull-up in the CH552 and we also have an additional 1K resistor in
    // series.
    //
    // This way each IO only should see about 200 uA current flowing into
    // the ESD protection diodes of the light controller MCU -- which should
    // be fine.
    //
    // On a related note, we use the CH552 GPIO directly to short the light
    // controller power supply when off (to discharge the capacitors in the
    // light controller).
    //
    //
    // We accept these as "calculated risk" to keep the BOM down.
    // A better technical solution would be runnging the CH552 from an external
    // 3.3V LDO, and using a MOSFET to short the light controller power supply.
    // But these add cost and complexity.
}


// ****************************************************************************
void main(void)
{
    // Enable safe mode
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;

    // Set sysclock to internal clock at 24 MHz
    // Note that 24 MHz allows UART1 to run at 115200 BAUD precisely, while
    // 12 MHz would not work (baudrate divider would be 6.5)
    CLOCK_CFG = 0x06;

    // Disable safe mode
    SAFE_MOD = 0x00;

    GPIO_init();

    // Enable Touch_Key interrupt, we use it as our 1ms timer
    IE_TKEY = 1;
    EA = 1;
    // Wait for the clock to stabilize
    delay_ms(50);

    UART0_init();
    USB_init();
    usb_enabled = 1;

    while (1) {
        SYSTICK_service();
        WATCHDOG_service();
        BOOTLOADER_service();
        UART_service();
        USB_service();
    }
}
