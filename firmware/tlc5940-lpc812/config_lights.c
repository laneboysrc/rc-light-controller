#include <stdint.h>
#include <stdbool.h>
#include <globals.h>


// Instructions comprise of opcodes and parameters
#define INSTRUCTION_END_OF_PROGRAM \
    (OPCODE_END_OF_PROGRAM << 24)

#define INSTRUCTION_END_OF_PROGRAMS \
    (OPCODE_END_OF_PROGRAMS << 24)

#define INSTRUCTION_SET_IMMEDIATE(start, stop, value) \
    ((OPCODE_SET_I << 24) | (stop << 16) | (start << 8) | value)

#define INSTRUCTION_SET(start, stop, var) \
    ((OPCODE_SET << 24) | (stop << 16) | (start << 8) | var)

#define INSTRUCTION_FADE_IMMEDIATE(start, stop, value) \
    ((OPCODE_FADE_I << 24) | (stop << 16) | (start << 8) | value)

#define INSTRUCTION_FADE(start, stop, var) \
    ((OPCODE_FADE << 24) | (stop << 16) | (start << 8) | var)

#define INSTRUCTION_WAIT_IMMEDIATE(time_in_ms) \
    ((OPCODE_WAIT_I << 24) | time_in_ms)

#define INSTRUCTION_WAIT(var) \
    ((OPCODE_WAIT << 24) | (PARAMETER_TYPE_VARIABLE << 8) | var)

#define INSTRUCTION_GOTO(line_no) \
    ((OPCODE_GOTO << 24) | line_no)

#define INSTRUCTION_ASSIGN_IMMEDIATE(var, immediate) \
    ((OPCODE_ASSIGN_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_ASSIGN_VARIABLE(var, source) \
    ((OPCODE_ASSIGN << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_ASSIGN_LED(var, led) \
    ((OPCODE_ASSIGN << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_ASSIGN_RANDOM(var) \
    ((OPCODE_ASSIGN << 24) | (var << 16) | (PARAMETER_TYPE_RANDOM << 8))

#define INSTRUCTION_ASSIGN_STEERING(var) \
    ((OPCODE_ASSIGN << 24) | (var << 16) | (PARAMETER_TYPE_STEERING << 8))

#define INSTRUCTION_ASSIGN_THROTTLE(var) \
    ((OPCODE_ASSIGN << 24) | (var << 16) | (PARAMETER_TYPE_THROTTLE << 8))

#define INSTRUCTION_ADD_IMMEDIATE(var, immediate) \
    ((OPCODE_ADD_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_ADD_VARIABLE(var, source) \
    ((OPCODE_ADD << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_ADD_LED(var, led) \
    ((OPCODE_ADD << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SUBTRACT_IMMEDIATE(var, immediate) \
    ((OPCODE_SUBTRACT_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_SUBTRACT_VARIABLE(var, source) \
    ((OPCODE_SUBTRACT << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_SUBTRACT_LED(var, led) \
    ((OPCODE_SUBTRACT << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_MULTIPLY_IMMEDIATE(var, immediate) \
    ((OPCODE_MULTIPLY_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_MULTIPLY_VARIABLE(var, source) \
    ((OPCODE_MULTIPLY << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_MULTIPLY_LED(var, led) \
    ((OPCODE_MULTIPLYs << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_DIVIDE_IMMEDIATE(var, immediate) \
    ((OPCODE_DIVIDE_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_DIVIDE_VARIABLE(var, source) \
    ((OPCODE_DIVIDE << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_DIVIDE_LED(var, led) \
    ((OPCODE_DIVIDE << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_AND_IMMEDIATE(var, immediate) \
    ((OPCODE_AND_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_AND_VARIABLE(var, source) \
    ((OPCODE_AND << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_AND_LED(var, led) \
    ((OPCODE_AND << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_OR_IMMEDIATE(var, immediate) \
    ((OPCODE_OR_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_OR_VARIABLE(var, source) \
    ((OPCODE_OR << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_OR_LED(var, led) \
    ((OPCODE_OR << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_XOR_IMMEDIATE(var, immediate) \
    ((OPCODE_XOR_I << 24) | (var << 16) | immediate)

#define INSTRUCTION_XOR_VARIABLE(var, source) \
    ((OPCODE_XOR << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_XOR_LED(var, led) \
    ((OPCODE_XOR << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SKIP_IF_EQ_VV(var, source) \
    ((OPCODE_SKIP_IF_EQ_V << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_SKIP_IF_EQ_VL(var, led) \
    ((OPCODE_SKIP_IF_EQ_V << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SKIP_IF_EQ_VI(var, immediate) \
    ((OPCODE_SKIP_IF_EQ_VI << 24) | (var << 16) | immediate)

#define INSTRUCTION_SKIP_IF_EQ_LI(led, immediate) \
    ((OPCODE_SKIP_IF_EQ_LI << 24) | (led << 16) | immediate)

#define INSTRUCTION_SKIP_IF_NE_VV(var, source) \
    ((OPCODE_SKIP_IF_NE_V << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_SKIP_IF_NE_VL(var, led) \
    ((OPCODE_SKIP_IF_NE_V << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SKIP_IF_NE_VI(var, immediate) \
    ((OPCODE_SKIP_IF_NE_VI << 24) | (var << 16) | immediate)

#define INSTRUCTION_SKIP_IF_NE_LI(led, immediate) \
    ((OPCODE_SKIP_IF_NE_LI << 24) | (led << 16) | immediate)

#define INSTRUCTION_SKIP_IF_GT_VV(var, source) \
    ((OPCODE_SKIP_IF_NE_V << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_SKIP_IF_GT_VL(var, led) \
    ((OPCODE_SKIP_IF_GT_V << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SKIP_IF_GT_VI(var, immediate) \
    ((OPCODE_SKIP_IF_GT_VI << 24) | (var << 16) | immediate)

#define INSTRUCTION_SKIP_IF_GT_LI(led, immediate) \
    ((OPCODE_SKIP_IF_GT_LI << 24) | (led << 16) | immediate)

#define INSTRUCTION_SKIP_IF_GE_VV(var, source) \
    ((OPCODE_SKIP_IF_GE_V << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_SKIP_IF_GE_VL(var, led) \
    ((OPCODE_SKIP_IF_NE_V << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SKIP_IF_GE_VI(var, immediate) \
    ((OPCODE_SKIP_IF_GE_VI << 24) | (var << 16) | immediate)

#define INSTRUCTION_SKIP_IF_GE_LI(led, immediate) \
    ((OPCODE_SKIP_IF_GE_LI << 24) | (led << 16) | immediate)

#define INSTRUCTION_SKIP_IF_LT_VV(var, source) \
    ((OPCODE_SKIP_IF_LT_V << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_SKIP_IF_LT_VL(var, led) \
    ((OPCODE_SKIP_IF_LT_V << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SKIP_IF_LT_VI(var, immediate) \
    ((OPCODE_SKIP_IF_LT_VI << 24) | (var << 16) | immediate)

#define INSTRUCTION_SKIP_IF_LT_LI(led, immediate) \
    ((OPCODE_SKIP_IF_LT_LI << 24) | (led << 16) | immediate)

#define INSTRUCTION_SKIP_IF_LE_VV(var, source) \
    ((OPCODE_SKIP_IF_LE_V << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

#define INSTRUCTION_SKIP_IF_LE_VL(var, led) \
    ((OPCODE_SKIP_IF_LE_V << 24) | (var << 16) | (PARAMETER_TYPE_LED << 8) | led)

#define INSTRUCTION_SKIP_IF_LE_VI(var, immediate) \
    ((OPCODE_SKIP_IF_LE_VI << 24) | (var << 16) | immediate)

#define INSTRUCTION_SKIP_IF_LE_LI(led, immediate) \
    ((OPCODE_SKIP_IF_LE_LI << 24) | (led << 16) | immediate)

#define INSTRUCTION_SKIP_IF_ANY(run_state) \
    ((OPCODE_SKIP_IF_ANY << 24) | run_state)

#define INSTRUCTION_SKIP_IF_ALL(run_state) \
    ((OPCODE_SKIP_IF_ALL << 24) | run_state)

#define INSTRUCTION_SKIP_IF_NONE(run_state) \
    ((OPCODE_SKIP_IF_NONE << 24) | run_state)

#define INSTRUCTION_ABS_VARIABLE(var, source) \
    ((OPCODE_ABS << 24) | (var << 16) | (PARAMETER_TYPE_VARIABLE << 8) | source)

// FIXME: implement missing instruction variants with all parameter types

// ****************************************************************************
const CAR_LIGHT_ARRAY_T local_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LOCAL_LEDS,
        .version = CONFIG_VERSION
    },

    .led_count = 16,

    .car_lights = (const CAR_LIGHT_T [16]) {
        // LED 0
        {   .light_switch_position[1] = 255,
            .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 1
        {   .light_switch_position[1] = 255,
            .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 2
        {   .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 3
        {   .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 4
        {.light_switch_position[3] = 255, .light_switch_position[4] = 255},

        // LED 5
        {.light_switch_position[3] = 255, .light_switch_position[4] = 255},

        // LED 6
        {.indicator_left = 255},

        // LED 7
        {.indicator_right = 255},

        // LED 8
        {.light_switch_position[4] = 255},

        // LED 9
        {.brake_light = 255},

        // LED 10
        {.tail_light = 84, .brake_light = 255},

        // LED 11
        {.tail_light = 84, .brake_light = 255},

        // LED 12
        {.reversing_light = 255},

        // LED 13
        {.reversing_light = 255},

        // LED 14
        {.indicator_left = 255},

        // LED 15
        {.indicator_right = 255},
    }
};


// ****************************************************************************
const CAR_LIGHT_ARRAY_T slave_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = SLAVE_LEDS,
        .version = CONFIG_VERSION
    },

    .led_count = 16,

    .car_lights = (const CAR_LIGHT_T [16]) {{.always_on = 0}}
};

#if 0
// ****************************************************************************
__attribute__ ((section(".light_programs")))
const LIGHT_PROGRAMS_T light_programs = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LIGHT_PROGRAMS,
        .version = CONFIG_VERSION
    },

    .number_of_programs = 4,
    .start = {
        &light_programs.programs[0],
        &light_programs.programs[10],
        &light_programs.programs[22],
        &light_programs.programs[28],
    },

    .programs = {
        // Program 0
        RUN_WHEN_INITIALIZING,
        0x00000000,
        LED_USED(0),

        INSTRUCTION_FADE_IMMEDIATE(0, 0, 0),
        INSTRUCTION_SET_IMMEDIATE(0, 0, 33),
        INSTRUCTION_WAIT_IMMEDIATE(1100),
        INSTRUCTION_SET_IMMEDIATE(0, 0, 100),
        INSTRUCTION_WAIT_IMMEDIATE(60),
        INSTRUCTION_GOTO(1),
        INSTRUCTION_END_OF_PROGRAM,

        // Program 1
        RUN_WHEN_GEAR_CHANGED,
        0x00000000,
        LED_USED(1) + LED_USED(2),

        INSTRUCTION_FADE_IMMEDIATE(1, 2, 0),
        INSTRUCTION_SET_IMMEDIATE(1, 2, 20),
        INSTRUCTION_WAIT_IMMEDIATE(300),
        INSTRUCTION_FADE_IMMEDIATE(1, 2, 20),
        INSTRUCTION_SET_IMMEDIATE(1, 2, 100),
        INSTRUCTION_WAIT_IMMEDIATE(100),
        INSTRUCTION_SET_IMMEDIATE(1, 2, 20),
        INSTRUCTION_WAIT_IMMEDIATE(300),
        INSTRUCTION_END_OF_PROGRAM,

        // Program 2
        RUN_WHEN_NO_SIGNAL,
        0x00000000,
        LED_USED(14),

        INSTRUCTION_FADE_IMMEDIATE(14, 14, 0),
        INSTRUCTION_SET_IMMEDIATE(14, 14, 10),
        INSTRUCTION_END_OF_PROGRAM,

        // Program 3
        RUN_WHEN_NORMAL_OPERATION,
        RUN_ALWAYS,
        LED_USED(15),

        INSTRUCTION_FADE_IMMEDIATE(15, 15, 0),
        INSTRUCTION_ASSIGN_IMMEDIATE(0, 0),     // Pre-load 6-click var[0] with 0

        INSTRUCTION_WAIT_IMMEDIATE(20),
        INSTRUCTION_SKIP_IF_LE_VI(0, 1),
        INSTRUCTION_ASSIGN_IMMEDIATE(0, 0),
        INSTRUCTION_SKIP_IF_ANY(RUN_WHEN_LIGHT_SWITCH_POSITION_1 | RUN_WHEN_LIGHT_SWITCH_POSITION_2),
        INSTRUCTION_GOTO(15),

        INSTRUCTION_ASSIGN_THROTTLE(1),
        INSTRUCTION_ABS_VARIABLE(1, 1),
        INSTRUCTION_ASSIGN_RANDOM(2),
        INSTRUCTION_AND_IMMEDIATE(2, 0xff),
        INSTRUCTION_MULTIPLY_VARIABLE(1, 2),
        INSTRUCTION_DIVIDE_IMMEDIATE(1, 635),   // 635 = divide by 256, then by (255 * 100)

        INSTRUCTION_SET(15, 15, 1),
        INSTRUCTION_GOTO(2),

        INSTRUCTION_SET_IMMEDIATE(15, 15, 20),
        INSTRUCTION_GOTO(2),

        INSTRUCTION_END_OF_PROGRAM,

        INSTRUCTION_END_OF_PROGRAMS
    }
};
#endif

__attribute__ ((section(".light_programs")))
const LIGHT_PROGRAMS_T light_programs = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LIGHT_PROGRAMS,
        .version = CONFIG_VERSION
    },

    .number_of_programs = 7,
    .start = {
        &light_programs.programs[0],
        &light_programs.programs[10],
        &light_programs.programs[20],
        &light_programs.programs[31],
        &light_programs.programs[37],
        &light_programs.programs[46],
        &light_programs.programs[56],
    },

    .programs = {
        0x00000001,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03050000,
        0x030f0800,
        0x03070664,
        0x07000000,
        0x01000002,
        0xfe000000,

        0x00000002,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03010000,
        0x030f0400,
        0x03030264,
        0x07000000,
        0x01000002,
        0xfe000000,

        0x00000020,
        0x00000000,
        0x0000ffcf,
        0x04030000,
        0x040f0600,
        0x03030000,
        0x030d0700,
        0x030f0f00,
        0x03060664,
        0x030e0e64,
        0xfe000000,

        0x00000040,
        0x00000000,
        0x00000030,
        0x04050400,
        0x03050464,
        0xfe000000,

        0x00000004,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03050000,
        0x030d0800,
        0x03070664,
        0x030f0e64,
        0xfe000000,

        0x00000008,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03050000,
        0x030d0700,
        0x030f0f00,
        0x03060664,
        0x030e0e64,
        0xfe000000,

        0x00000010,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03060000,
        0x030e0800,
        0x03070764,
        0x030f0f64,
        0xfe000000,

        0xff000000,
    }
};
