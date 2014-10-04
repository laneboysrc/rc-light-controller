#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#define CONFIG_VERSION 1


// Suppress unused parameter or variable warning
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#define __SYSTICK_IN_MS 20

// The ROM_MAGIC marker is used to identify the location of ROM constants when
// parsing a light controller binary by an external tool
#define ROM_MAGIC 0x6372424c                // LBrc (LANE Boys RC) in little endian

#define ST 0
#define TH 1
#define CH3 2

// Number of positions of our virtual light switch. Includes the "off"
// position 0.
// NOTE: if you change this value you need to adjust CAR_LIGHT_FUNCTION_T
// accordingly!
#define LIGHT_SWITCH_POSITIONS 9


// Convenience functions for min/max
#define MIN(x, y) ((x) < (y) ? x : (y))
#define MAX(x, y) ((x) > (y) ? x : (y))



// ****************************************************************************
typedef enum {
    // By specifying this unused value we force the enmeration to fit in a
    // uint16_t
    EMPTY = 0xffff,

    CONFIG_SECTION = 0x01,
    GAMMA_TABLE = 0x02,
    LOCAL_MONOCHROME_LEDS = 0x10,
    LOCAL_RGB_LEDS = 0x11,
    SLAVE_MONOCHROME_LEDS = 0x20,
    SLAVE_RGB_LEDS = 0x21,
    LIGHT_PROGRAMS = 0x30
} ROM_SECTION_T;


// ****************************************************************************
typedef struct {
    uint32_t magic_value;
    ROM_SECTION_T type;
    uint16_t version;
} MAGIC_T;


// ****************************************************************************
typedef struct {
    MAGIC_T magic;
    uint8_t gamma_table[256];
} GAMMA_TABLE_T;


// ****************************************************************************
typedef struct {
    uint16_t left;
    uint16_t centre;
    uint16_t right;
} SERVO_ENDPOINTS_T;


// ****************************************************************************
typedef struct {
    uint32_t raw_data;
    SERVO_ENDPOINTS_T endpoint;
    int16_t normalized;
    uint16_t absolute;
    bool reversed;
} CHANNEL_T;


// ****************************************************************************
typedef enum {
    GEAR_1 = 0,
    GEAR_2 = 1
} GEAR_T;


// ****************************************************************************
typedef enum {
    SERVO_OUTPUT_SETUP_OFF = 0,
    SERVO_OUTPUT_SETUP_CENTRE = 0x01,
    SERVO_OUTPUT_SETUP_LEFT = 0x02,
    SERVO_OUTPUT_SETUP_RIGHT = 0x04
} SERVO_OUTPUT_T;


// ****************************************************************************
typedef enum {
    REVERSING_SETUP_OFF = 0,
    REVERSING_SETUP_STEERING = 0x01,
    REVERSING_SETUP_THROTTLE = 0x02,
} REVERSING_SETUP_T;


// ****************************************************************************
typedef enum {
    WINCH_DISABLED = 0,
    WINCH_IDLE = 0x01,
    WINCH_IN = 0x02,
    WINCH_OUT = 0x04
} WINCH_T;


// ****************************************************************************
typedef struct {
    unsigned int systick : 1;               // Set for one mainloop every 20 ms
    unsigned int new_channel_data : 1;      // Set for one mainloop every time servo pulses were received
    unsigned int startup_mode_neutral : 1;
    unsigned int blink_flag : 1;            // Toggles with 1.5 Hz
    unsigned int blink_hazard : 1;          // Hazard lights active
    unsigned int blink_indicator_left : 1;  // Left indicator active
    unsigned int blink_indicator_right : 1; // Right indicator active
    unsigned int forward : 1;               // Set when the car is driving forward
    unsigned int braking : 1;               // Set when the brakes are enganged
    unsigned int reversing : 1;             // Set when the car is reversing

    unsigned int gear_changed : 1;          // Set for one mainloop when a new gear was selected
    unsigned int gear : 1;

    unsigned int servo_output_setup : 3;

    unsigned int reversing_setup : 2;

    unsigned int winch_mode : 3;
} GLOBAL_FLAGS_T;


// ****************************************************************************
typedef enum {
    MASTER_WITH_SERVO_READER,
    MASTER_WITH_UART_READER,
    SLAVE
} MASTER_MODE_T;


// ****************************************************************************
typedef struct {
    MAGIC_T magic;

    MASTER_MODE_T mode;

    struct {
        // If mode is MASTER_WITH_SERVO_READER  then all flags are mutually
        // exculsive.
        // If mode is MASTER_WITH_UART_READER then there can be one UART output
        // (slave, preprocessor or winch) and one servo output (steering wheel
        // or gearbox servo)
        unsigned int slave_output : 1;
        unsigned int preprocessor_output : 1;
        unsigned int winch_output : 1;
        unsigned int steering_wheel_servo_output : 1;
        unsigned int gearbox_servo_output : 1;

        unsigned int esc_forward_reverse : 1;
        unsigned int ch3_is_local_switch : 1;
        unsigned int ch3_is_momentary : 1;

        unsigned int auto_brake_lights_forward_enabled : 1;
        unsigned int auto_brake_lights_reverse_enabled : 1;

        // If ENABLE_BRAKE_DISARM_TIMEOUT is not set, the user has to go for
        // brake, then neutral, before reverse engages. Otherwise reverse
        // engages if the user stays in neutral for a few seconds.
        //
        // Tamiya ESC need this ENABLE_BRAKE_DISARM_TIMEOUT cleared.
        // The China ESC and HPI SC-15WP need ENABLE_BRAKE_DISARM_TIMEOUT set.
        unsigned int brake_disarm_timeout_enabled : 1;
    } flags;

    uint16_t auto_brake_counter_value_forward_min;
    uint16_t auto_brake_counter_value_forward_max;
    uint16_t auto_brake_counter_value_reverse_min;
    uint16_t auto_brake_counter_value_reverse_max;
    uint16_t auto_reverse_counter_value_min;
    uint16_t auto_reverse_counter_value_max;
    uint16_t brake_disarm_counter_value;

    uint16_t blink_counter_value;
    uint16_t indicator_idle_time_value;
    uint16_t indicator_off_timeout_value;

    // Centre threshold defines a range where we consider the servo being
    // centred. In order to prevent "flickering" especially for the brake and
    // reverse light the CENTRE_THRESHOLD_HIGH and CENTRE_THRESHOLD_LOW provide
    // a hysteresis that we apply to the throttle when processing drive_mode.
    uint16_t centre_threshold_low;
    uint16_t centre_threshold;
    uint16_t centre_threshold_high;
    uint16_t blink_threshold;

    uint16_t light_switch_positions;
    uint16_t initial_endpoint_delta;
    uint16_t ch3_multi_click_timeout;
    uint16_t winch_command_repeat_time;

    uint32_t baudrate;
} LIGHT_CONTROLLER_CONFIG_T;



// ****************************************************************************
// Definitions for the various light configuration structures
// The design goal was to handle the both RGB and monochrome LEDs with similar
// functions.
// Ideally this part would have been done in an object-oriented language...

typedef uint8_t MONOCHROME_LED_T;

typedef struct {    // 3-bytes, not packed
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_LED_T;


typedef struct {    // 4-bytes packed (2 bits free)
    // Simulation of incandescent lights
    uint8_t max_change_per_systick;

    // Simulation of a weak ground connection
    uint8_t reduction_percent;
    unsigned int light_switch_position_0 : 1;
    unsigned int light_switch_position_1 : 1;
    unsigned int light_switch_position_2 : 1;
    unsigned int light_switch_position_3 : 1;
    unsigned int light_switch_position_4 : 1;
    unsigned int light_switch_position_5 : 1;
    unsigned int light_switch_position_6 : 1;
    unsigned int light_switch_position_7 : 1;
    unsigned int light_switch_position_8 : 1;
    unsigned int tail_light : 1;
    unsigned int brake_light : 1;
    unsigned int reversing_light : 1;
    unsigned int indicator_left : 1;
    unsigned int indicator_right : 1;
} LIGHT_FEATURE_T;

// For standard car light functions we have an array of values, one per LED,
// where each entry corresponds to one light funciton. The user can assign
// multiple functions to a single LED (such as brake and tail light function)
// and the software will "mix" the final color value.

typedef struct {    // 20-bytes packed (1 byte free)
    LIGHT_FEATURE_T features;

    MONOCHROME_LED_T always_on;
    MONOCHROME_LED_T light_switch_position[LIGHT_SWITCH_POSITIONS];
    MONOCHROME_LED_T tail_light;
    MONOCHROME_LED_T brake_light;
    MONOCHROME_LED_T reversing_light;
    MONOCHROME_LED_T indicator_left;
    MONOCHROME_LED_T indicator_right;
} MONOCHROME_CAR_LIGHT_T;


typedef struct {    // 52-bytes packed (3 byte free)
    LIGHT_FEATURE_T features;

    RGB_LED_T always_on;
    RGB_LED_T light_switch_position[LIGHT_SWITCH_POSITIONS];
    RGB_LED_T tail_light;
    RGB_LED_T brake_light;
    RGB_LED_T reversing_light;
    RGB_LED_T indicator_left;
    RGB_LED_T indicator_right;
} RGB_CAR_LIGHT_T;


// In order to have most of the functions being generic to the LED type
// we define a super-structure that adds a LED type identifier.
// This identifier allows low-level code to cast to the proper car light
// structure (MONOCHROME_CAR_LIGHT_T or RGB_CAR_LIGHT_T).

typedef enum {
    MONOCHROME,
    RGB
} LED_TYPE_T;

typedef struct {    // 16-bytes packed (3 byte free)
    MAGIC_T magic;
    LED_TYPE_T led_type;
    uint8_t led_count;

    // Can either be MONOCHROME_CAR_LIGHT_T or RGB_CAR_LIGHT_T. It is actually
    // a pointer to the first element in an array of those structures.
    const void *car_lights;
} CAR_LIGHT_T;



// ****************************************************************************
// The entropy variable is incremented every mainloop. It can therefore serve
// as a random value in practical RC car application,
// Certainly not suitable for secure implementations...
extern uint32_t entropy;

extern const LIGHT_CONTROLLER_CONFIG_T config;
extern const CAR_LIGHT_T local_monochrome_leds;
extern const CAR_LIGHT_T local_rgb_leds;
extern const CAR_LIGHT_T slave_monochrome_leds;
extern const CAR_LIGHT_T slave_rgb_leds;
extern const GAMMA_TABLE_T gamma_table;

extern GLOBAL_FLAGS_T global_flags;
extern CHANNEL_T channel[3];
extern SERVO_ENDPOINTS_T servo_output_endpoint;


// ****************************************************************************
// Globally accessible functions from various modules
void SysTick_handler(void);

void load_persistent_storage(void);
void write_persistent_storage(void);

void init_servo_reader(void);
void read_all_servo_channels(void);
void SCT_irq_handler(void);

void init_uart_reader(void);
void read_preprocessor(void);

void process_ch3_clicks(void);

void process_drive_mode(void);

void process_indicators(void);
void toggle_hazard_lights(void);

void init_servo_output(void);
void process_servo_output(void);
void servo_output_setup_action(uint8_t ch3_clicks);
void gearbox_action(uint8_t ch3_clicks);

void process_winch(void);
void winch_action(uint8_t ch3_clicks);
bool abort_winching(void);

void process_channel_reversing_setup(void);
void reversing_setup_action(uint8_t ch3_clicks);

void output_preprocessor(void);

void init_lights(void);
void process_lights(void);
void next_light_sequence(void);
void light_switch_up(void);
void light_switch_down(void);
void toggle_light_switch(void);

#endif // __GLOBALS_H
