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


static uint8_t light_switch_position;
static uint8_t tlc5940_light_data[16];

/*
SPI configuration:
    Configuration: CPOL = 0, CPHA = 0,
    We can send 6 bit frame lengths, so no need to pack light data!
    TXRDY indicates when we can put the next data into txbuf
    Use SSEL function to de-assert XLAT while sending new data
*/

#define GPIO_GSCLK LPC_GPIO_PORT->W0[1]
#define GPIO_BLANK LPC_GPIO_PORT->W0[6]
#define GPIO_XLAT LPC_GPIO_PORT->W0[2]
#define GPIO_SCK LPC_GPIO_PORT->W0[3]
#define GPIO_SIN LPC_GPIO_PORT->W0[7]

#define LED_BRIGHTNESS_CONST_A        (0.08f)                       /* Set Point LED brightness equation: a * b ^ (brightness_level + c) ...                      */
#define LED_BRIGHTNESS_CONST_B        (1.75f)                       /* Constants have been set for the equations to produce distinctive brightness levels         */
#define LED_BRIGHTNESS_CONST_C        (2.00f)
#define LED_BRIGHTNESS_EQUATION(level) (LED_BRIGHTNESS_CONST_A * pow(LED_BRIGHTNESS_CONST_B, level + LED_BRIGHTNESS_CONST_C))



typedef enum {
    ALWAYS_ON,
    LIGHT_SWITCH_POSITION,
    LIGHT_SWITCH_POSITION_0 = LIGHT_SWITCH_POSITION + 0,
    LIGHT_SWITCH_POSITION_1 = LIGHT_SWITCH_POSITION + 1,
    LIGHT_SWITCH_POSITION_2 = LIGHT_SWITCH_POSITION + 2,
    LIGHT_SWITCH_POSITION_3 = LIGHT_SWITCH_POSITION + 3,
    LIGHT_SWITCH_POSITION_4 = LIGHT_SWITCH_POSITION + 4,
    LIGHT_SWITCH_POSITION_5 = LIGHT_SWITCH_POSITION + 5,
    LIGHT_SWITCH_POSITION_6 = LIGHT_SWITCH_POSITION + 6,
    LIGHT_SWITCH_POSITION_7 = LIGHT_SWITCH_POSITION + 7,
    LIGHT_SWITCH_POSITION_8 = LIGHT_SWITCH_POSITION + 8,
    TAIL_LIGHT,
    BRAKE_LIGHT,
    REVERSING_LIGHT,
    INDICATOR_LEFT,
    INDICATOR_RIGHT
} CAR_LIGHT_FUNCTION_T;









// ****************************************************************************
static void send_light_data_to_tlc5940(void)
{
    volatile int i;

    // Wait for MSTIDLE
    while (!(LPC_SPI0->STAT & (1 << 8)));

    for (i = 15; i >= 0; i--) {
        // Wait for TXRDY
        while (!(LPC_SPI0->STAT & (1 << 1)));

        LPC_SPI0->TXDAT = tlc5940_light_data[i];
    }

    // Force END OF TRANSFER
    LPC_SPI0->STAT = (1 << 7);
}


// ****************************************************************************
void init_lights(void)
{
    GPIO_BLANK = 1;
    GPIO_GSCLK = 0;
    GPIO_XLAT = 0;

    // FIXME: can we make that more configurable?
    LPC_GPIO_PORT->DIR0 |=
        (1 << 1) | (1 << 2) | (1 << 3) | (1 << 6) | (1 << 7);

    // Use 2 MHz SPI clock. 16 bytes take about 50 us to transmit.
    LPC_SPI0->DIV = (__SYSTEM_CLOCK / 2000000) - 1;

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
    LPC_SWM->PINASSIGN3 = 0x03ffffff;   // PIO0_3 is SCK
    LPC_SWM->PINASSIGN4 = 0xff02ff07;   // PIO0_2 is XLAT (SSEL) PIO0_3 is SIN (MOSI)

    send_light_data_to_tlc5940();

    GPIO_BLANK = 0;
    GPIO_GSCLK = 1;
}


// ****************************************************************************
void next_light_sequence(void)
{
	;
}


// ****************************************************************************
void light_switch_up(void)
{
    if (light_switch_position < (config.light_switch_positions - 1)) {
        ++light_switch_position;
    }
}


// ****************************************************************************
void light_switch_down(void)
{
    if (light_switch_position > 0) {
        --light_switch_position;
    }
}


// ****************************************************************************
void toggle_light_switch(void)
{
    if (light_switch_position < (config.light_switch_positions - 1)) {
        light_switch_position = config.light_switch_positions - 1;
    }
    else {
        light_switch_position = 0;
    }
}


// ****************************************************************************
static const void * get_light_value(
    const CAR_LIGHT_T *lights, int index, CAR_LIGHT_FUNCTION_T function)
{
    const MONOCHROME_CAR_LIGHT_T *mono;

    switch(lights->led_type) {
        case MONOCHROME:
            mono = &((MONOCHROME_CAR_LIGHT_T *)lights->car_lights)[index];

            switch (function) {
                case ALWAYS_ON:
                    return &mono->always_on;

                case LIGHT_SWITCH_POSITION_0:
                case LIGHT_SWITCH_POSITION_1:
                case LIGHT_SWITCH_POSITION_2:
                case LIGHT_SWITCH_POSITION_3:
                case LIGHT_SWITCH_POSITION_4:
                case LIGHT_SWITCH_POSITION_5:
                case LIGHT_SWITCH_POSITION_6:
                case LIGHT_SWITCH_POSITION_7:
                case LIGHT_SWITCH_POSITION_8:
                    return &mono->light_switch_position[
                        function - LIGHT_SWITCH_POSITION];

                case TAIL_LIGHT:
                    return &mono->tail_light;

                case BRAKE_LIGHT:
                    return &mono->brake_light;

                case REVERSING_LIGHT:
                    return &mono->reversing_light;

                case INDICATOR_LEFT:
                    return &mono->indicator_left;

                case INDICATOR_RIGHT:
                    return &mono->indicator_right;

                default:
                    return (uint8_t []){0};
            }

        case RGB:
            // FIXME: implement RGB light
        default:
            return (uint8_t []){0, 0, 0};
    }
}


// ****************************************************************************
static bool is_value_zero(
    const CAR_LIGHT_T *lights, int index, CAR_LIGHT_FUNCTION_T function)
{
    const void * value = get_light_value(lights, index, function);

    switch(lights->led_type) {
        case MONOCHROME:
            return *(MONOCHROME_LED_T *)value == 0 ? true : false;

        case RGB:
            // All r/g/b values must be zero for RGB LEDs to qualify as zero
            if (((RGB_LED_T *)value)->r != 0) {
                return false;
            }
            if (((RGB_LED_T *)value)->g != 0) {
                return false;
            }
            if (((RGB_LED_T *)value)->b != 0) {
                return false;
            }
            return true;

        default:
            return true;
    }
}


// ****************************************************************************
static void set_light(LED_TYPE_T led_type, void *led, const void *value)
{
    switch(led_type) {
        case MONOCHROME:
            *(MONOCHROME_LED_T *)led = *(MONOCHROME_LED_T *)value;

        case RGB:
        default:
            break;

    }
}


// ****************************************************************************
static void mix_light(LED_TYPE_T led_type, void *led, const void *value)
{
    MONOCHROME_LED_T *mono_led;
    MONOCHROME_LED_T *mono_value;

    switch(led_type) {
        case MONOCHROME:
            mono_led = (MONOCHROME_LED_T *)led;
            mono_value = (MONOCHROME_LED_T *)value;

            if (*mono_value > *mono_led) {
                *mono_led = *mono_value;
            }
            break;

        case RGB:
        default:
            break;

    }
}


// ****************************************************************************
static void set_car_light(void *led, const CAR_LIGHT_T *lights,
    int index, CAR_LIGHT_FUNCTION_T function)
{
    set_light(lights->led_type, led, get_light_value(lights, index, function));
}


// ****************************************************************************
static void mix_car_light(void *led, const CAR_LIGHT_T *lights,
    int index, CAR_LIGHT_FUNCTION_T function)
{
    mix_light(lights->led_type, led, get_light_value(lights, index, function));
}


// ****************************************************************************
static void combined_tail_brake(
    const CAR_LIGHT_T *lights, int index, MONOCHROME_LED_T *led)
{
    if (light_switch_position > 0) {
        mix_car_light(led, lights, index, TAIL_LIGHT);
    }

    if (global_flags.braking) {
        mix_car_light(led, lights, index, BRAKE_LIGHT);
    }
}


// ****************************************************************************
static void combined_tail_brake_indicators(
    const CAR_LIGHT_T *lights, int index, MONOCHROME_LED_T *led)
{
    if (global_flags.blink_hazard || global_flags.blink_indicator_left ||
        global_flags.blink_indicator_right) {
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
                mix_car_light(led, lights, index, BRAKE_LIGHT);
            }
            else if (light_switch_position > 0) {
                mix_car_light(led, lights, index, TAIL_LIGHT);
            }
            else {
                if (global_flags.blink_indicator_left
                    || global_flags.blink_hazard) {
                    mix_car_light(led, lights, index, INDICATOR_LEFT);
                }
                if (global_flags.blink_indicator_right
                    || global_flags.blink_hazard) {
                    mix_car_light(led, lights, index, INDICATOR_RIGHT);
                }
            }
        }
        else {
            // Dark blink period: light is off unless both tail and brake are
            // active

            if (light_switch_position > 0 && global_flags.braking) {
                mix_car_light(led, lights, index, TAIL_LIGHT);
            }
        }
    }
    else {
        // No indicator active: process like normal tail/brake lights
        combined_tail_brake(lights, index, led);
    }
}


// ****************************************************************************
static void process_light(
    const CAR_LIGHT_T *lights, int index, MONOCHROME_LED_T *current_led)
{
    set_car_light(current_led, lights, index, ALWAYS_ON);

    mix_car_light(current_led, lights, index,
        LIGHT_SWITCH_POSITION + light_switch_position);

    if (global_flags.reversing) {
        mix_car_light(current_led, lights, index, REVERSING_LIGHT);
    }

    if (!is_value_zero(lights, index, TAIL_LIGHT) &&
        !is_value_zero(lights, index, BRAKE_LIGHT) &&
        (   !is_value_zero(lights, index, INDICATOR_LEFT) ||
            !is_value_zero(lights, index, INDICATOR_RIGHT))) {
        // Special case for combined tail / brake / indicators
        combined_tail_brake_indicators(lights, index, current_led);
    }
    else {
        combined_tail_brake(lights, index, current_led);

        if (global_flags.blink_flag) {

            if (global_flags.blink_hazard ||
                global_flags.blink_indicator_left) {
                mix_car_light(current_led, lights, index, INDICATOR_LEFT);
            }
            if (global_flags.blink_hazard ||
                global_flags.blink_indicator_right) {
                mix_car_light(current_led, lights, index, INDICATOR_RIGHT);
            }
        }
    }
}


// ****************************************************************************
static void process_car_lights(void)
{
    int i;

    for (i = 0; i < 16 ; i++) {
        process_light(&local_monochrome_leds, i, &tlc5940_light_data[i]);
    }

    if (config.flags.slave_output) {
        for (i = 0; i < 16 ; i++) {
            process_light(&slave_monochrome_leds, i, &tlc5940_light_data[i]);
        }
    }
}


// ****************************************************************************
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
