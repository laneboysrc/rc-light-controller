/******************************************************************************


    Light scenarios to consider

    - Always-on of certain LEDs
    - Dynamic discover of light_position_switch maximum value
        * This is done by the firmware generator, not at run-time
    - Combined tail / brake lights
    - Combined tail / brake / indicator lights (XR311, Sawback)
    - Hazard and indicators
    - Consider single and multi-color LEDs
    - Consider half brightness is not half LED current
        - Implement gamma correction at the output step
        - Fixed gamma table in FLASH, generated by firmware egenerator
    - Simulation of incadescent bulbs
        From Wikipedia: For a 100-watt, 120-volt general-service lamp, the
        current stabilizes in about 0.10 seconds, and the lamp reaches 90% of
        its full brightness after about 0.13 seconds.
    - Simulation of weak ground connection
    - Support weird, unforeseen combinations, like combined reverse/indicators
      on the Lancia Fulvia
    * We need to handle WS2812 as well as PL9823 (swapped rgb order!)
    * Consider HSL fading?!

    * Setup
        * No signal (1 entry)
            * At run-time it depends on the receiver, but we can still detect
              broken wires (timeout 500ms)
        * Initializing (startup mode neutral; 1 entry)
        * Setup of steering, throttle reverse (2 entries)
        * Setup of the output servo centre, left, right (3 entries)
        * That would be 1568 bytes total, quite a bit!
            * We rather allow up to 10 LEDs being specified, others are off
            * This will take only 280 bytes (everything RGB)

    * Events
        * Can trigger a sequence
        * Gear change
        *   Different sequence depending on gear value
        *   Different sequence depending on state of roof lights?
        * Any other?


    Flags for normal car lights:
    - always on
    - light switch position (max 0..8), 0 is off (default)
        - Can be used to implement parking, main beam, high beam, etc
    - tail light (= any light switch position other than 0)
    - brakeing
    - reversing
    - indicator left
    - indicator left


    Incadescent:
        Configurable time. Default: 140ms. Applies to full scale.
        As assume linear rise/decay (perceived brigthness, gamma corrected).
        Given that we change every systtick, this means that maximum
        value (before gamma correction) we change is 256 / 120ms / 20ms == 37
        Note that if the maximum brightness of a lamp is not 100%, this value
        should be adjusted (e.g. if max is 50%, use double the time constant,
        or half the max step size.) This should be automatically done in the
        firmware generator (max_steps_per_systick).


    Weak ground connection:
        For each led be able to specify a flag indicating which car light
        function is influencing it. For example, a weak ground connecton
        on a combinled trai / brake LED that should be influenced by the
        left indicator, would set the left indicator flag only.
        There would also be a configurable brightness reduction value in %

******************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <hal.h>
#include <globals.h>
#include <printf.h>


#define SLAVE_MAGIC_BYTE ((uint8_t)0x87)

// 16 lights locally, another 16 potentially at a slave
#define MAX_LIGHTS 32


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

uint8_t light_switch_position;
LED_T light_setpoint[MAX_LIGHTS];
LED_T light_actual[MAX_LIGHTS];
uint8_t max_change_per_systick[MAX_LIGHTS];
static uint8_t diagnostics;



// ****************************************************************************
static void output_lights(void)
{
    uint8_t data[16];
    int i;

    if (global_flags.switched_outputs) {
        HAL_gpio_write(HAL_GPIO_OUT0, light_setpoint[0]);
        HAL_gpio_write(HAL_GPIO_OUT1, light_setpoint[1]);
        HAL_gpio_write(HAL_GPIO_OUT2, light_setpoint[2]);
        HAL_gpio_write(HAL_GPIO_OUT3, light_setpoint[3]);
        HAL_gpio_write(HAL_GPIO_OUT3_LPC832, light_setpoint[3]);
        HAL_gpio_write(HAL_GPIO_OUT4, light_setpoint[4]);
        HAL_gpio_write(HAL_GPIO_OUT4_LPC832, light_setpoint[4]);
        HAL_gpio_write(HAL_GPIO_OUT5, light_setpoint[5]);
        HAL_gpio_write(HAL_GPIO_OUT6, light_setpoint[6]);
        HAL_gpio_write(HAL_GPIO_OUT7, light_setpoint[7]);
        HAL_gpio_write(HAL_GPIO_OUT8, light_setpoint[8]);
        return;
    }

    for (i = 0; i < 16; i++) {
       data[i] = gamma_table.gamma_table[light_actual[i]] >> 2;
    }

    HAL_spi_transaction(data, 16);


    // The switched_light_output mirrors the output of LED15 onto the
    // dedicated output pin.
    // Fading is not applied. 0 turns the output off, any other value on.

    if (config.flags2.invert_out15s) {
        HAL_gpio_write(HAL_GPIO_SWITCHED_LIGHT_OUTPUT, !light_setpoint[15]);
    }
    else {
        HAL_gpio_write(HAL_GPIO_SWITCHED_LIGHT_OUTPUT, light_setpoint[15]);
    }

    if (config.flags.ws2811_output) {
        for (i = 0; i < 15; i++) {
           // data[i] = gamma_table.gamma_table[light_actual[16 + i]];
           data[i] = light_actual[16 + i];
        }
        HAL_ws2811_transaction(data, 16);
    }

}


// ****************************************************************************
void init_gpio_lights(void) {
    if (global_flags.switched_outputs) {
        HAL_gpio_clear_mask(
            (1 << HAL_GPIO_OUT0.pin) |
            (1 << HAL_GPIO_OUT1.pin) |
            (1 << HAL_GPIO_OUT2.pin) |
            (1 << HAL_GPIO_OUT3.pin) |
            (1 << HAL_GPIO_OUT3_LPC832.pin) |
            (1 << HAL_GPIO_OUT4.pin) |
            (1 << HAL_GPIO_OUT4_LPC832.pin) |
            (1 << HAL_GPIO_OUT5.pin) |
            (1 << HAL_GPIO_OUT6.pin) |
            (1 << HAL_GPIO_OUT7.pin) |
            (1 << HAL_GPIO_OUT8.pin));

        HAL_gpio_out_mask(
            (1 << HAL_GPIO_OUT0.pin) |
            (1 << HAL_GPIO_OUT1.pin) |
            (1 << HAL_GPIO_OUT2.pin) |
            (1 << HAL_GPIO_OUT3.pin) |
            (1 << HAL_GPIO_OUT3_LPC832.pin) |
            (1 << HAL_GPIO_OUT4.pin) |
            (1 << HAL_GPIO_OUT4_LPC832.pin) |
            (1 << HAL_GPIO_OUT5.pin) |
            (1 << HAL_GPIO_OUT6.pin) |
            (1 << HAL_GPIO_OUT7.pin) |
            (1 << HAL_GPIO_OUT8.pin));
        return;
    }

    // Make the switched light output PIO0_9 an output and shut it off.
    HAL_gpio_clear(HAL_GPIO_SWITCHED_LIGHT_OUTPUT);
    HAL_gpio_out(HAL_GPIO_SWITCHED_LIGHT_OUTPUT);
}


// ****************************************************************************
// SPI configuration:
//     Configuration: CPOL = 0, CPHA = 0,
//     We can send 6 bit frame lengths, so no need to pack light data!
//     TXRDY indicates when we can put the next data into txbuf
//     Use SSEL function to de-assert XLAT while sending new data
// ****************************************************************************
void init_lights(void)
{
    if (!global_flags.switched_outputs) {
        HAL_gpio_set(HAL_GPIO_BLANK);
        HAL_gpio_set(HAL_GPIO_BLANK_LPC832);
        HAL_gpio_clear(HAL_GPIO_GSCLK);

        HAL_gpio_out_mask(
                (1 << HAL_GPIO_BLANK.pin) |
                (1 << HAL_GPIO_BLANK_LPC832.pin) |
                (1 << HAL_GPIO_GSCLK.pin));

        HAL_spi_init();
    }

    output_lights();

    if (!global_flags.switched_outputs) {
        HAL_gpio_clear(HAL_GPIO_BLANK);
        HAL_gpio_clear(HAL_GPIO_BLANK_LPC832);
    }

    // Do this short function in-between clearing BLANK and setting GSCLK to
    // surely meet the setup time requirement of the TLC5940
    init_light_programs();

    if (!global_flags.switched_outputs) {
        HAL_gpio_set(HAL_GPIO_GSCLK);
    }

    light_switch_position = config.initial_light_switch_position;
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
    bool toggle_on = false;

    if (config.flags2.prefer_all_lights_off) {
        // Switch to the highest used light switch position only when we are
        // at light switch position 0, otherwise switch to light switch
        // position 0
        toggle_on = light_switch_position == 0;
    }
    else {
        // Switch to the highest used light switch position unless we are
        // already in the highest used light switch position, otherwise
        // switch to light switch position 0
        toggle_on = light_switch_position < (config.light_switch_positions - 1);
    }

    if (toggle_on) {
        light_switch_position = config.light_switch_positions - 1;
    }
    else {
        light_switch_position = 0;
    }
}


// ****************************************************************************
static const LED_T *get_light_value(const CAR_LIGHT_T *light,
    CAR_LIGHT_FUNCTION_T function)
{
    static const LED_T zero = 0;

    switch (function) {
        case ALWAYS_ON:
            return &light->always_on;

        case LIGHT_SWITCH_POSITION_0:
        case LIGHT_SWITCH_POSITION_1:
        case LIGHT_SWITCH_POSITION_2:
        case LIGHT_SWITCH_POSITION_3:
        case LIGHT_SWITCH_POSITION_4:
        case LIGHT_SWITCH_POSITION_5:
        case LIGHT_SWITCH_POSITION_6:
        case LIGHT_SWITCH_POSITION_7:
        case LIGHT_SWITCH_POSITION_8:
            return &light->light_switch_position[
                function - LIGHT_SWITCH_POSITION];

        case TAIL_LIGHT:
            return &light->tail_light;

        case BRAKE_LIGHT:
            return &light->brake_light;

        case REVERSING_LIGHT:
            return &light->reversing_light;

        case INDICATOR_LEFT:
            return &light->indicator_left;

        case INDICATOR_RIGHT:
            return &light->indicator_right;

        default:
            return &zero;
    }
}


// ****************************************************************************
static bool is_value_zero(const CAR_LIGHT_T *light,
    CAR_LIGHT_FUNCTION_T function)
{
    const LED_T * value = get_light_value(light, function);

    return *value == 0 ? true : false;
}


// ****************************************************************************
static void set_light(LED_T *led, const LED_T *value)
{
    *led = *value;
}


// ****************************************************************************
static void mix_light(LED_T *led, const LED_T *value)
{
    *led = MAX(*led, *value);
}


// ****************************************************************************
static LED_T calculate_step_value(LED_T current, LED_T new, uint8_t max_change)
{
    LED_T adjusted_max_change;

    if (new > current) {
        adjusted_max_change = MIN(max_change, 0xff - current);
        return MIN(new, current + adjusted_max_change);
    }
    else {
        adjusted_max_change = MIN(max_change, current);
        return MAX(new, current - adjusted_max_change);
    }
}


// ****************************************************************************
static void set_car_light(LED_T *led, const CAR_LIGHT_T *light,
    CAR_LIGHT_FUNCTION_T function)
{
    set_light(led, get_light_value(light, function));
}


// ****************************************************************************
static void mix_car_light(LED_T *led, const CAR_LIGHT_T *light,
    CAR_LIGHT_FUNCTION_T function)
{
    mix_light(led, get_light_value(light, function));
}


// ****************************************************************************
static void combined_tail_brake(LED_T *led, const CAR_LIGHT_T *light)
{
    if (light_switch_position > 0) {
        mix_car_light(led, light, TAIL_LIGHT);
    }

    if (global_flags.braking) {
        mix_car_light(led, light, BRAKE_LIGHT);
    }
}


// ****************************************************************************
static void us_style_combined_lights(LED_T *led, const CAR_LIGHT_T *light)
{
    // Actual US style
    //                         BLINKFLAG
    //                      on          off
    // --------------------------------------
    // Tail + Brake off     blink       off
    // Tail                 blink       tail
    // Brake                blink       off
    // Tail + Brake         blink       tail

    // Rationale: cars have a 2-filament bulb: 5W for tail light,
    // and 25W for indicators and brake lights.
    // Indicates take priority over brake lights. Tail light is independent.

    if (global_flags.blink_flag) {
        if (global_flags.blink_indicator_left
            || global_flags.blink_hazard) {
            mix_car_light(led, light, INDICATOR_LEFT);
        }
        if (global_flags.blink_indicator_right
            || global_flags.blink_hazard) {
            mix_car_light(led, light, INDICATOR_RIGHT);
        }
    }
    else {
        // Dark blink period: light is off unless tail is active
        if (light_switch_position > 0) {
            mix_car_light(led, light, TAIL_LIGHT);
        }
    }
}


static void not_quite_us_style_combined_lights(LED_T *led, const CAR_LIGHT_T *light)
{
    // not-really-US style as implemented in our XR311
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
            mix_car_light(led, light, BRAKE_LIGHT);
        }
        else if (light_switch_position > 0) {
            mix_car_light(led, light, TAIL_LIGHT);
        }
        else {
            if (global_flags.blink_indicator_left
                || global_flags.blink_hazard) {
                mix_car_light(led, light, INDICATOR_LEFT);
            }
            if (global_flags.blink_indicator_right
                || global_flags.blink_hazard) {
                mix_car_light(led, light, INDICATOR_RIGHT);
            }
        }
    }
    else {
        // Dark blink period: light is off unless both tail and brake are
        // active
        if (light_switch_position > 0 && global_flags.braking) {
            mix_car_light(led, light, TAIL_LIGHT);
        }
    }
}


// ****************************************************************************
static void combined_tail_brake_indicators(LED_T *led, const CAR_LIGHT_T *light)
{
    if (global_flags.blink_hazard ||
            (global_flags.blink_indicator_left &&
                !is_value_zero(light, INDICATOR_LEFT)) ||
            (global_flags.blink_indicator_right &&
                !is_value_zero(light, INDICATOR_RIGHT))) {

        if (config.flags2.us_style_combined_lights) {
            us_style_combined_lights(led, light);
        }
        else {
            not_quite_us_style_combined_lights(led, light);
        }
    }
    else {
        // No indicator active: process like normal tail/brake lights
        combined_tail_brake(led, light);
    }
}


// ****************************************************************************
static bool is_light_affected(const LIGHT_FEATURE_T *w)
{
    if (w->light_switch_position_0 && light_switch_position == 0) {
        return true;
    }

    if (w->light_switch_position_1 && light_switch_position == 1) {
        return true;
    }

    if (w->light_switch_position_2 && light_switch_position == 2) {
        return true;
    }

    if (w->light_switch_position_3 && light_switch_position == 3) {
        return true;
    }

    if (w->light_switch_position_4 && light_switch_position == 4) {
        return true;
    }

    if (w->light_switch_position_5 && light_switch_position == 5) {
        return true;
    }

    if (w->light_switch_position_6 && light_switch_position == 6) {
        return true;
    }

    if (w->light_switch_position_7 && light_switch_position == 7) {
        return true;
    }

    if (w->light_switch_position_8 && light_switch_position == 8) {
        return true;
    }

    if (w->tail_light && light_switch_position > 0) {
        return true;
    }

    if (w->brake_light && global_flags.braking) {
        return true;
    }

    if (w->reversing_light && global_flags.reversing) {
        return true;
    }

    if (w->indicator_left && global_flags.blink_flag &&
            (global_flags.blink_indicator_left || global_flags.blink_hazard)) {
        return true;
    }

    if (w->indicator_right && global_flags.blink_flag &&
            (global_flags.blink_indicator_right || global_flags.blink_hazard)) {
        return true;
    }

    return false;
}


// ****************************************************************************
static void simulate_weak_ground(LED_T *led, const CAR_LIGHT_T *light)
{

    if (light->features.reduction_percent == 0) {
        return;
    }

    if (is_light_affected(&light->features)) {
        *led = (uint16_t)(*led) *
            (100 - light->features.reduction_percent) / 100;
    }
}


// ****************************************************************************
static void process_light(const CAR_LIGHT_T *light, LED_T *led, uint8_t *limit)
{
    LED_T result = 0;

    *limit = light->features.max_change_per_systick;

    // Handle the diagnostics functions like initializing, no-signal etc.
    // If one of them is active than it takes priority over everything else.
    // The diagnostics are disabled in shelf queen mode
    if (!global_flags.shelf_queen_mode ) {
        if (diagnostics) {
            if (diagnostics & light->diagnostics) {
                *led = config.diagnostics_brightness;
            }
            else {
                *led = 0;
            }
            return;
        }
    }

    set_car_light(&result, light, ALWAYS_ON);

    mix_car_light(&result, light, LIGHT_SWITCH_POSITION + light_switch_position);

    if (global_flags.reversing) {
        mix_car_light(&result, light, REVERSING_LIGHT);
    }

    if (!is_value_zero(light, TAIL_LIGHT) &&
        !is_value_zero(light, BRAKE_LIGHT) &&
        (   !is_value_zero(light, INDICATOR_LEFT) ||
            !is_value_zero(light, INDICATOR_RIGHT))) {
        // Special case for combined tail / brake / indicators
        combined_tail_brake_indicators(&result, light);
    }
    else {
        combined_tail_brake(&result, light);

        if (global_flags.blink_flag) {
            if (global_flags.blink_hazard ||
                global_flags.blink_indicator_left) {
                mix_car_light(&result, light, INDICATOR_LEFT);
            }
            if (global_flags.blink_hazard ||
                global_flags.blink_indicator_right) {
                mix_car_light(&result, light, INDICATOR_RIGHT);
            }
        }
    }

    simulate_weak_ground(&result, light);
    *led = result;
}


// ****************************************************************************
static void process_car_lights(void)
{
    int i;
    uint32_t leds_used;
    static uint8_t old_light_switch_position = 0xff;

    leds_used = process_light_programs();


    if (light_switch_position != old_light_switch_position) {
        old_light_switch_position = light_switch_position;
        printf("light_switch_position %d\n", light_switch_position);
    }

    // Prepare the utilized and active diagnostics flags (no-signal, initializing ...)
    // so they can be used in process_light()
    diagnostics = get_priority_run_state() & config.diagnostics_mask;

    // Handle LEDs connected to the TLC5940 locally
    for (i = 0; i < local_leds.led_count ; i++) {
        if (leds_used & (1 << i)) {
            continue;
        }
        process_light(&local_leds.car_lights[i], &light_setpoint[i],
            &max_change_per_systick[i]);
    }

    // Handle LEDs connected to a slave light controller
    for (i = 0; i < slave_leds.led_count ; i++) {
        if (leds_used & (1 << (16 + i))) {
            continue;
        }

        process_light(&slave_leds.car_lights[i], &light_setpoint[16 + i],
            &max_change_per_systick[16 + i]);
    }

    // Apply max_change_per_systick while copying from light_setpoint to
    // light_actual
    for (i = 0; i < MAX_LIGHTS ; i++) {
        if (max_change_per_systick[i] > 0) {
            light_actual[i] = calculate_step_value(
                light_actual[i], light_setpoint[i], max_change_per_systick[i]);
        }
        else {
            light_actual[i] = light_setpoint[i];
        }
    }

    // If one of the AUX channels has requested to disable all light outputs
    // then manually force all light values to 0,
    if (global_flags.outputs_disabled) {
        for (i = 0; i < MAX_LIGHTS ; i++) {
            light_actual[i] = 0;
        }
    }

    output_lights();
    if (config.flags.slave_output) {
        HAL_putc(STDOUT, SLAVE_MAGIC_BYTE);

        for (i = 0; i < slave_leds.led_count ; i++) {
            HAL_putc(STDOUT, gamma_table.gamma_table[light_actual[16 + i]] >> 2);
        }
    }
}


// ****************************************************************************
static void process_slave(void)
{
    static int state = 0;

    while (HAL_getchar_pending()) {
        uint8_t uart_byte;

        uart_byte = HAL_getchar();

        // The slave/preprocessor protocol is designed such that only the first
        // byte can have the MAGIC value. This allows us to be in sync at all
        // times.
        // If we receive the MAGIC value we know it is the first byte, so we
        // can kick off the state machine.
        if (uart_byte == SLAVE_MAGIC_BYTE) {
            state = 1;
        }
        else {
            if (state >= 1) {
                // Set both lights_setpoint and lights_actual as lights_setpoint
                // drives the switched light output and lights_actual drives
                // the TLC5940
                light_setpoint[state - 1] = uart_byte << 2;
                light_actual[state - 1]   = uart_byte << 2;
                ++state;

                // Once we got all 16 LED values we send the data to the LEDs
                // and reset the state machine to wait for the next packet
                if (state > 16) {
                    state = 0;
                    output_lights();
                }
            }
        }
    }
}


// ****************************************************************************
void process_lights(void)
{
    if (config.mode == SLAVE) {
        process_slave();
    }
    else {
        process_light_program_events();
        if (global_flags.systick) {
            process_car_lights();
        }
    }
}
