/******************************************************************************


    Light scenarios to consider

    - Always-on of certain LEDs
    - Dynamic discover of light_mode maximum value
    - Combined tail / brake lights
    - Combined tail / brake / indicator lights (XR311, Sawback)
    - Hazard and indicators
    - High priority situations (in order or priority)
        - Startup
        - Setup of steering, throttle reverse
        - Setup of the output servo centre, left, right
        - Winch active
    - Events
        - Can trigger a sequence
        - Gear change
        -   Different sequence depending on gear value
        -   Different sequence depending on state of roof lights?
        - Any other?
    - Programmabe sequences
        - Store difference with previous state
        - Steps can have a delay
        - Run once or always
        - Events can trigger a sequence, while another sequence is running
          and needs to continue after the event sequence finished.
        - How to map sequences and non-sequences?
        - Automatic fading between two steps?
        - Resolution 20ms
        - Bogdan's idea regarding flame simulation, depending on throttle

    - Consider single and multi-color LEDs
    - Consider half brightness is not half LED current
    - Simulation of incadescent bulbs
    - Simulation of weak ground connection
    - Support weird, unforeseen combinations, like combined reverse/indicators
      on the Lancia Fulvia



    Combined tail / brake / indicator:

                             BLINKFLAG
                          on          off
     --------------------------------------
     Tail + Brake off     half        off
     Tail                 half        off
     Brake                full        off
     Tail + Brake         full        half

    Best is to pre-calculate this, as well as the indicators.


            Tail | Brake | Ind | Blink      RESULT
            0      0       0     0          0
            0      0       0     1          0
            0      0       1     0          0
            0      0       1     1          half
            0      1       0     0          full
            0      1       0     1          full
            0      1       1     0          0
            0      1       1     1          full
            1      0       0     0          half
            1      0       0     1          half
            1      0       1     0          0
            1      0       1     1          half
            1      1       0     0          full
            1      1       0     1          full
            1      1       1     0          half
            1      1       1     1          full


    Flags for static lights:
    - always on
    - light switch position (max 0..8), 0 is off (default)
    - tail light (= any light switch position other than 0)
    - brakeing
    - reversing
    - indicator left
    - indicator left

    The light controller is intelligent enough to find combined tail/brake
    and tail/brake/indicator lights.
    For unknown combinations the highest light value is used.


******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>
#include <uart0.h>

#define MAX_LIGHT_SWITCH_POSITIONS 9

static uint8_t light_switch_position;
static uint8_t tlc5940_light_data[16];

/*
SPI configuration:
    Configuration: CPOL = 0, CPHA = 0,
    We can send 6 bit frame lengths, so no need to pack light data!
    TXRDY indicates when we can put the next data into txbuf
    Use SSEL function to de-assert XLAT while sending new data
*/

#define GSCLK LPC_GPIO_PORT->W0[1]
#define BLANK LPC_GPIO_PORT->W0[6]
#define XLAT LPC_GPIO_PORT->W0[2]
#define SCK LPC_GPIO_PORT->W0[3]
#define SIN LPC_GPIO_PORT->W0[7]

#define LED_BRIGHTNESS_CONST_A        (0.08f)                       /* Set Point LED brightness equation: a * b ^ (brightness_level + c) ...                      */
#define LED_BRIGHTNESS_CONST_B        (1.75f)                       /* Constants have been set for the equations to produce distinctive brightness levels         */
#define LED_BRIGHTNESS_CONST_C        (2.00f)
#define LED_BRIGHTNESS_EQUATION(level) (LED_BRIGHTNESS_CONST_A * pow(LED_BRIGHTNESS_CONST_B, level + LED_BRIGHTNESS_CONST_C))

typedef uint8_t SINGLE_COLOR_LED_T;

typedef struct {
    SINGLE_COLOR_LED_T always_on;

    SINGLE_COLOR_LED_T light_switch_position[MAX_LIGHT_SWITCH_POSITIONS];

    SINGLE_COLOR_LED_T tail_light;
    SINGLE_COLOR_LED_T brake_light;
    SINGLE_COLOR_LED_T reversing_light;
    SINGLE_COLOR_LED_T indicator_left;
    SINGLE_COLOR_LED_T indicator_right;
} LOCAL_CAR_LIGHT_T;


static LOCAL_CAR_LIGHT_T local_leds[16] = {
    // LED 0
    {.always_on = 63},

    // LED 1
    {.light_switch_position[1] = 63},

    // LED 2
    {.light_switch_position[1] = 63, .light_switch_position[2] = 63},

    // LED 4
    {.tail_light = 63},

    // LED 5
    {.brake_light = 63},

    // LED 6
    {.tail_light = 5, .brake_light = 63},

    // LED 7
    {.reversing_light = 63},

    // LED 8
    {.indicator_left = 63},

    // LED 9
    {.indicator_right = 63},

    // LED 10
    {.indicator_left = 5, .tail_light = 5, .brake_light = 63},

    // LED 11
    {.indicator_right = 5, .tail_light = 5, .brake_light = 63},
};


static void send_light_data_to_tlc5940(void)
{
    volatile int i;

    // Wait for MSTIDLE
    while (!(LPC_SPI0->STAT & (1u << 8)));

    for (i = 15; i >= 0; i--) {
        // Wait for TXRDY
        while (!(LPC_SPI0->STAT & (1u << 1)));

        LPC_SPI0->TXDAT = tlc5940_light_data[i];
    }

    // Force END OF TRANSFER
    LPC_SPI0->STAT = (1u << 7);
}


void init_lights(void)
{
    BLANK = 1;
    GSCLK = 0;
    XLAT = 0;

    LPC_GPIO_PORT->DIR0 |=
        (1u << 1) | (1u << 2) | (1u << 3) | (1u << 6) | (1u << 7);

    // Use 2 MHz SPI clock. 16 bytes take about 50 us to transmit.
    LPC_SPI0->DIV = (__SYSTEM_CLOCK / 2000000) - 1;

    LPC_SPI0->CFG = (1u << 0) |          // Enable SPI0
                    (1u << 2) |          // Master mode
                    (0 << 3) |          // LSB First mode disabled
                    (0 << 4) |          // CPHA = 0
                    (0 << 5) |          // CPOL = 0
                    (0 << 8);           // SPOL = 0

    LPC_SPI0->TXCTRL = (1u << 21) |      // set EOF
                       (1u << 22) |      // RXIGNORE, otherwise SPI hangs until
                                        //   we read the data register
                       ((6 - 1) << 24); // 6 bit frames

    // We use the SSEL function for XLAT: low during the transmission, high
    // during the idle periood.
    LPC_SWM->PINASSIGN3 = 0x03ffffff;   // PIO0_3 is SCK
    LPC_SWM->PINASSIGN4 = 0xff02ff07;   // PIO0_2 is XLAT (SSEL) PIO0_3 is SIN (MOSI)

    send_light_data_to_tlc5940();

    BLANK = 0;
    GSCLK = 1;
}


void next_light_sequence(void)
{
	;
}


void light_switch_up(void)
{
    if (light_switch_position < config.light_switch_positions) {
        ++light_switch_position;
    }
}


void light_switch_down(void)
{
    if (light_switch_position > 0) {
        --light_switch_position;
    }
}


void toggle_light_switch(void)
{
    if (light_switch_position < config.light_switch_positions) {
        light_switch_position = config.light_switch_positions;
    }
    else {
        light_switch_position = 0;
    }
}


static void max_light(SINGLE_COLOR_LED_T *led, uint8_t value)
{
    if (value > *led) {
        *led = value;
    }
}


static void combined_tail_brake_indicators(
    LOCAL_CAR_LIGHT_T *current_light, SINGLE_COLOR_LED_T *current_led)
{

    SINGLE_COLOR_LED_T active_indicator = 0;

    if (global_flags.blink_indicator_left) {
        max_light(&active_indicator, current_light->indicator_left);
    }
    if (global_flags.blink_indicator_right) {
        max_light(&active_indicator, current_light->indicator_right);
    }

    if (active_indicator > 0) {
        //                         BLINKFLAG
        //                      on          off
        // --------------------------------------
        // Tail + Brake off     blink       off
        // Tail                 tail        off
        // Brake                brake       off
        // Tail + Brake         brake       tail

        if (global_flags.blink_flag) {
            // Bright blink period: Brake value has highest priority, followed
            // by tail and finally the indicator value

            if (global_flags.braking) {
                max_light(current_led, current_light->brake_light);
            }
            else if (light_switch_position > 0) {
                max_light(current_led, current_light->tail_light);
            }
            else {
                max_light(current_led, active_indicator);
            }
        }
        else {
            // Dark blink period: light is off unless both tail and brake are
            // active

            if (light_switch_position > 0 && global_flags.braking) {
                max_light(current_led, current_light->tail_light);
            }
        }
    }
    else {
        // No indicator active: process like normal tail/brake lights

        if (current_light->tail_light) {
            if (light_switch_position > 0) {
                max_light(current_led, current_light->tail_light);
            }
        }

        if (current_light->brake_light) {
            if (global_flags.braking) {
                max_light(current_led, current_light->brake_light);
            }
        }
    }
}


static void process_car_lights(void)
{
    int i;

    for (i = 0; i < 16 ; i++) {
        LOCAL_CAR_LIGHT_T *current_light;
        SINGLE_COLOR_LED_T *current_led;

        current_light = &local_leds[i];
        current_led = &tlc5940_light_data[i];

        if (current_light->always_on) {
            *current_led = current_light->always_on;
        }

        if (current_light->light_switch_position[light_switch_position] > 0) {
            *current_led =
                current_light->light_switch_position[light_switch_position];
        }

        if (current_light->reversing_light) {
            if (global_flags.reversing) {
                max_light(current_led, current_light->reversing_light);
            }
        }

        if (current_light->tail_light &&
            current_light->brake_light &&
            (current_light->indicator_left || current_light->indicator_right)) {
            // Special case for combined tail / brake / indicators
            combined_tail_brake_indicators(current_light, current_led);
        }
        else {
            if (current_light->tail_light) {
                if (light_switch_position > 0) {
                    max_light(current_led, current_light->tail_light);
                }
            }

            if (current_light->brake_light) {
                if (global_flags.braking) {
                    max_light(current_led, current_light->brake_light);
                }
            }

            if (current_light->indicator_left) {
                if (global_flags.blink_flag &&
                    (global_flags.blink_hazard ||
                     global_flags.blink_indicator_left)) {
                    max_light(current_led, current_light->indicator_left);
                }
            }

            if (current_light->indicator_right) {
                if (global_flags.blink_flag &&
                    (global_flags.blink_hazard ||
                     global_flags.blink_indicator_right)) {
                    max_light(current_led, current_light->indicator_right);
                }
            }
        }
    }
}


void process_lights(void)
{
    static uint8_t old_light_switch_position = 0xff;

    if (light_switch_position != old_light_switch_position) {
        old_light_switch_position = light_switch_position;
        uart0_send_cstring("light_switch_position ");
        uart0_send_uint32(light_switch_position);
        uart0_send_linefeed();
    }

    if (global_flags.systick) {
        process_car_lights();
        send_light_data_to_tlc5940();
    }
}
