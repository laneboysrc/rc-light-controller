'use strict';
/*jslint browser: true, bitwise: true, vars: true, plusplus: true */
/*global  */

var disassembler = (function () {

    var MAX_NUMBER_OF_INSTRUCTIONS = 16 * 1024 / 4;
    var NUMBER_OF_LEDS = 64;

    var asm = [];
    (function initialize_asm() {
        var i;
        for (i = 0; i < MAX_NUMBER_OF_INSTRUCTIONS; i += 1) {
            asm.push(0);
        }
    }());

    var leds_used = 0;
    var leds2_used = 0;
    var leds_to_declare_offset;
    var leds_used_instruction;
    var variables = {};
    var var_offsets = [];
    var current_program = 1;

    var offset = 0;
    var pc = 0;

    var STATE_PRIORITY = 0;
    var STATE_RUN = 1;
    var STATE_LEDS_USED = 2;
    var STATE_LEDS2_USED = 3;
    var STATE_PROGRAM = 4;
    var STATE_END_OF_PROGRAM = 5;
    var STATE_IGNORE = 6;

    var state  = STATE_PRIORITY;

    var opcodes = {
        'GOTO': 0x01,
        'SET': 0x02,
        'SET_I': 0x03,
        'FADE': 0x04,
        'FADE_I': 0x05,
        'SLEEP': 0x06,
        'SLEEP_I': 0x07,
        'ASSIGN': 0x10,
        'ASSIGN_I': 0x11,
        'ADD': 0x12,
        'ADD_I': 0x13,
        'SUBTRACT': 0x14,
        'SUBTRACT_I': 0x15,
        'MULTIPLY': 0x16,
        'MULTIPLY_I': 0x17,
        'DIVIDE': 0x18,
        'DIVIDE_I': 0x19,
        'AND': 0x1a,
        'AND_I': 0x1b,
        'OR': 0x1c,
        'OR_I': 0x1d,
        'XOR': 0x1e,
        'XOR_I': 0x1f,
        'MOD': 0x38,
        'MOD_I': 0x39,
        'ABS': 0x40,                // VAR = |VAR| (steering, throttle)
        'SKIP_IF_EQ_V': 0x20,       // ==       var, type, id
        'SKIP_IF_EQ_VI': 0x21,      // ==       var, immediate
        'SKIP_IF_EQ_L': 0x22,       // ==       led, type, id
        'SKIP_IF_EQ_LI': 0x23,      // ==       led, immediate
        'SKIP_IF_NE_V': 0x24,       // !=       var, type, id
        'SKIP_IF_NE_VI': 0x25,      // !=       var, immediate
        'SKIP_IF_NE_L': 0x26,       // !=       led, type, id
        'SKIP_IF_NE_LI': 0x27,      // !=       led, immediate
        'SKIP_IF_GE_V': 0x28,       // >=       var, type, id
        'SKIP_IF_GE_VI': 0x29,      // >=       var, immediate
        'SKIP_IF_GE_L': 0x2a,       // >=       led, type, id
        'SKIP_IF_GE_LI': 0x2b,      // >=       led, immediate
        'SKIP_IF_GT_V': 0x2c,       // >        var, type, id
        'SKIP_IF_GT_VI': 0x2d,      // >        var, immediate
        'SKIP_IF_GT_L': 0x2e,       // >        led, type, id
        'SKIP_IF_GT_LI': 0x2f,      // >        led, immediate
        'SKIP_IF_LE_V': 0x30,       // >=       var, type, id
        'SKIP_IF_LE_VI': 0x31,      // >=       var, immediate
        'SKIP_IF_LE_L': 0x32,       // >=       led, type, id
        'SKIP_IF_LE_LI': 0x33,      // >=       led, immediate
        'SKIP_IF_LT_V': 0x34,       // >        var, type, id
        'SKIP_IF_LT_VI': 0x35,      // >        var, immediate
        'SKIP_IF_LT_L': 0x36,       // >        led, type, id
        'SKIP_IF_LT_LI': 0x37,      // >        led, immediate
        'END_OF_PROGRAM': 0xfe,
        'END_OF_PROGRAMS': 0xff
    };

    var FIRST_SKIP_IF_OPCODE = 0x20;
    var LAST_SKIP_IF_OPCODE = 0x37;
    var OPCODE_SKIP_IF_ANY = 0x60;      // 011 + 29 bits run_state!
    var OPCODE_SKIP_IF_ALL = 0x80;      // 100 + 29 bits run_state!
    var OPCODE_SKIP_IF_NONE = 0xA0;     // 101 + 29 bits run_state!

    // var INSTRUCTION_MODIFIER_LED = 0x02000000;
    var INSTRUCTION_MODIFIER_IMMEDIATE = 0x01000000;

    var PARAMETER_TYPE_VARIABLE = 0;
    var PARAMETER_TYPE_LED = 1;
    var PARAMETER_TYPE_RANDOM = 2;

    var RUN_WHEN_NORMAL_OPERATION           = 0;
    var RUN_WHEN_NO_SIGNAL                  = (1 << 0);
    var RUN_WHEN_INITIALIZING               = (1 << 1);
    var RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE  = (1 << 2);
    var RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT    = (1 << 3);
    var RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT   = (1 << 4);
    var RUN_WHEN_REVERSING_SETUP_STEERING   = (1 << 5);
    var RUN_WHEN_REVERSING_SETUP_THROTTLE   = (1 << 6);
    var RUN_WHEN_GEAR_CHANGED               = (1 << 7);
    var RUN_WHEN_SHELF_QUEEN_MODE           = (1 << 8);

    var RUN_WHEN_LIGHT_SWITCH_POSITION_0    = (1 << 0);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_1    = (1 << 1);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_2    = (1 << 2);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_3    = (1 << 3);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_4    = (1 << 4);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_5    = (1 << 5);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_6    = (1 << 6);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_7    = (1 << 7);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_8    = (1 << 8);
    var RUN_WHEN_NEUTRAL                    = (1 << 9);
    var RUN_WHEN_FORWARD                    = (1 << 10);
    var RUN_WHEN_REVERSING                  = (1 << 11);
    var RUN_WHEN_BRAKING                    = (1 << 12);
    var RUN_WHEN_INDICATOR_LEFT             = (1 << 13);
    var RUN_WHEN_INDICATOR_RIGHT            = (1 << 14);
    var RUN_WHEN_HAZARD                     = (1 << 15);
    var RUN_WHEN_BLINK_FLAG                 = (1 << 16);
    var RUN_WHEN_BLINK_LEFT                 = (1 << 17);
    var RUN_WHEN_BLINK_RIGHT                = (1 << 18);
    var RUN_WHEN_WINCH_DISABLERD            = (1 << 19);
    var RUN_WHEN_WINCH_IDLE                 = (1 << 20);
    var RUN_WHEN_WINCH_IN                   = (1 << 21);
    var RUN_WHEN_WINCH_OUT                  = (1 << 22);
    var RUN_WHEN_PROGRAM_STATE_0            = (1 << 24);
    var RUN_WHEN_PROGRAM_STATE_1            = (1 << 25);
    var RUN_WHEN_PROGRAM_STATE_2            = (1 << 26);
    var RUN_WHEN_PROGRAM_STATE_3            = (1 << 27);
    var RUN_WHEN_PROGRAM_STATE_4            = (1 << 28);

    var RUN_ALWAYS                          = 0x80000000;

    var CAR_STATE_LIGHT_SWITCH_POSITION_0   = (1 << 0);
    var CAR_STATE_LIGHT_SWITCH_POSITION_1   = (1 << 1);
    var CAR_STATE_LIGHT_SWITCH_POSITION_2   = (1 << 2);
    var CAR_STATE_LIGHT_SWITCH_POSITION_3   = (1 << 3);
    var CAR_STATE_LIGHT_SWITCH_POSITION_4   = (1 << 4);
    var CAR_STATE_LIGHT_SWITCH_POSITION_5   = (1 << 5);
    var CAR_STATE_LIGHT_SWITCH_POSITION_6   = (1 << 6);
    var CAR_STATE_LIGHT_SWITCH_POSITION_7   = (1 << 7);
    var CAR_STATE_LIGHT_SWITCH_POSITION_8   = (1 << 8);
    var CAR_STATE_NEUTRAL                   = (1 << 9);
    var CAR_STATE_FORWARD                   = (1 << 10);
    var CAR_STATE_REVERSING                 = (1 << 11);
    var CAR_STATE_BRAKING                   = (1 << 12);
    var CAR_STATE_INDICATOR_LEFT            = (1 << 13);
    var CAR_STATE_INDICATOR_RIGHT           = (1 << 14);
    var CAR_STATE_HAZARD                    = (1 << 15);
    var CAR_STATE_BLINK_FLAG                = (1 << 16);
    var CAR_STATE_BLINK_LEFT                = (1 << 17);
    var CAR_STATE_BLINK_RIGHT               = (1 << 18);
    var CAR_STATE_WINCH_DISABLERD           = (1 << 19);
    var CAR_STATE_WINCH_IDLE                = (1 << 20);
    var CAR_STATE_WINCH_IN                  = (1 << 21);
    var CAR_STATE_WINCH_OUT                 = (1 << 22);
    var CAR_STATE_SERVO_OUTPUT_SETUP_CENTRE = (1 << 24);
    var CAR_STATE_SERVO_OUTPUT_SETUP_LEFT   = (1 << 25);
    var CAR_STATE_SERVO_OUTPUT_SETUP_RIGHT  = (1 << 26);
    var CAR_STATE_REVERSING_SETUP_STEERING  = (1 << 27);
    var CAR_STATE_REVERSING_SETUP_THROTTLE  = (1 << 28);


    // *************************************************************************
    var set_variable_used = function (index, current_program) {
        if (variables[index] === undefined) {
            variables[index] = {};
        }

        variables[index][current_program] = 1;
    };


    // *************************************************************************
    var decode_predefined_variables = function (index) {
        switch (index) {
            case 0:
                return 'clicks';
            case 1:
                return 'light-switch-position';
            case 2:
                return'gear';
            case 3:
                return'servo';
            case 4:
                return 'program-state-0';
            case 5:
                return 'program-state-1';
            case 6:
                return 'program-state-2';
            case 7:
                return 'program-state-3';
            case 8:
                return 'program-state-4';
            case 9:
                return 'steering';
            case 10:
                return 'throttle';
            case 11:
                return 'aux';
            case 12:
                return 'aux2';
            case 13:
                return 'aux3';
            case 14:
                return 'hazard';
            case 95:
                return 'aux6';
            case 96:
                return 'aux5';
            case 97:
                return 'aux4';
            case 98:
                return 'ch3-pin';
            case 99:
                return 'shelf-queen-mode';
            default:
                break;
        }

        return '';
    }


    // *************************************************************************
    var decode_priority_run_condition = function (instruction) {
        if (instruction === RUN_WHEN_NORMAL_OPERATION) {
            return;
        }

        if (instruction & RUN_WHEN_NO_SIGNAL) {
            asm[offset++].decleration = 'run when no-signal';
        }
        if (instruction & RUN_WHEN_INITIALIZING) {
            asm[offset++].decleration = 'run when initializing';
        }
        if (instruction & RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE) {
            asm[offset++].decleration = 'run when servo-output-setup-centre';
        }
        if (instruction & RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT) {
            asm[offset++].decleration = 'run when servo-output-setup-left';
        }
        if (instruction & RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT) {
            asm[offset++].decleration = 'run when servo-output-setup-right';
        }
        if (instruction & RUN_WHEN_REVERSING_SETUP_STEERING) {
            asm[offset++].decleration = 'run when reversing-setup-steering';
        }
        if (instruction & RUN_WHEN_REVERSING_SETUP_THROTTLE) {
            asm[offset++].decleration = 'run when reversing-setup-throttle';
        }
        if (instruction & RUN_WHEN_GEAR_CHANGED) {
            asm[offset++].decleration = 'run when gear-changed';
        }
        if (instruction & RUN_WHEN_SHELF_QUEEN_MODE) {
            asm[offset++].decleration = 'run when shelf-queen-mode';
        }
    };


    // *************************************************************************
    var decode_run_condition = function (instruction) {
        if (instruction === 0) {
            return;
        }

        if (instruction & RUN_ALWAYS) {
            asm[offset++].decleration = 'run always';
            return;
        }

        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_0) {
            asm[offset++].decleration = 'run when light-switch-position-0';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_1) {
            asm[offset++].decleration = 'run when light-switch-position-1';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_2) {
            asm[offset++].decleration = 'run when light-switch-position-2';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_3) {
            asm[offset++].decleration = 'run when light-switch-position-3';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_4) {
            asm[offset++].decleration = 'run when light-switch-position-4';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_5) {
            asm[offset++].decleration = 'run when light-switch-position-5';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_6) {
            asm[offset++].decleration = 'run when light-switch-position-6';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_7) {
            asm[offset++].decleration = 'run when light-switch-position-7';
        }
        if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_8) {
            asm[offset++].decleration = 'run when light-switch-position-8';
        }
        if (instruction & RUN_WHEN_NEUTRAL) {
            asm[offset++].decleration = 'run when neutral';
        }
        if (instruction & RUN_WHEN_FORWARD) {
            asm[offset++].decleration = 'run when forward';
        }
        if (instruction & RUN_WHEN_REVERSING) {
            asm[offset++].decleration = 'run when reversing';
        }
        if (instruction & RUN_WHEN_BRAKING) {
            asm[offset++].decleration = 'run when braking';
        }
        if (instruction & RUN_WHEN_INDICATOR_LEFT) {
            asm[offset++].decleration = 'run when indicator-left';
        }
        if (instruction & RUN_WHEN_INDICATOR_RIGHT) {
            asm[offset++].decleration = 'run when indicator-right';
        }
        if (instruction & RUN_WHEN_HAZARD) {
            asm[offset++].decleration = 'run when hazard';
        }
        if (instruction & RUN_WHEN_BLINK_FLAG) {
            asm[offset++].decleration = 'run when blink-flag';
        }
        if (instruction & RUN_WHEN_BLINK_LEFT) {
            asm[offset++].decleration = 'run when blink-left';
        }
        if (instruction & RUN_WHEN_BLINK_RIGHT) {
            asm[offset++].decleration = 'run when blink-right';
        }
        if (instruction & RUN_WHEN_WINCH_DISABLERD) {
            asm[offset++].decleration = 'run when winch-disabled';
        }
        if (instruction & RUN_WHEN_WINCH_IDLE) {
            asm[offset++].decleration = 'run when winch-idle';
        }
        if (instruction & RUN_WHEN_WINCH_IN) {
            asm[offset++].decleration = 'run when winch-in';
        }
        if (instruction & RUN_WHEN_WINCH_OUT) {
            asm[offset++].decleration = 'run when winch-out';
        }
        if (instruction & RUN_WHEN_PROGRAM_STATE_0) {
            asm[offset++].decleration = 'run when program-state-0';
        }
        if (instruction & RUN_WHEN_PROGRAM_STATE_1) {
            asm[offset++].decleration = 'run when program-state-1';
        }
        if (instruction & RUN_WHEN_PROGRAM_STATE_2) {
            asm[offset++].decleration = 'run when program-state-2';
        }
        if (instruction & RUN_WHEN_PROGRAM_STATE_3) {
            asm[offset++].decleration = 'run when program-state-3';
        }
        if (instruction & RUN_WHEN_PROGRAM_STATE_4) {
            asm[offset++].decleration = 'run when program-state-4';
        }
    };


    // *************************************************************************
    var decode_leds_used = function (instruction, instruction2) {
        var i;
        var any_led = false;

        leds_used = instruction;
        leds_to_declare_offset = offset++;
        asm[leds_to_declare_offset].leds_to_declare = leds_used;

        leds2_used = instruction2;
        asm[leds_to_declare_offset].leds2_to_declare = leds2_used;


        // console.log(instruction)

        if (leds_used === (Math.pow(2, 32) - 1)) {
            if (leds2_used === (Math.pow(2, 32) - 1)) {
                asm[leds_to_declare_offset].leds_to_declare = 0;
                asm[leds_to_declare_offset].leds2_to_declare = 0;

                asm[offset++].decleration = 'use all leds';
                asm[offset++].decleration = '';  // Empty line
            }
        }

        for (i = 0; i < NUMBER_OF_LEDS; i++) {
            if (i < 32) {
                if (leds_used & Math.pow(2, i)) {
                    asm[offset].led = i;
                    asm[offset++].decleration =
                        'led led' + i + ' = led[' + i + ']';
                    any_led = true;
                }
            }
            else {
                if (leds2_used & Math.pow(2, (i - 32))) {
                    asm[offset].led = i;
                    asm[offset++].decleration =
                        'led led' + i + ' = led[' + i + ']';
                    any_led = true;
                }
            }
        }

        if (any_led) {
            asm[offset++].decleration = '';  // Empty line
        }
    };


    // *************************************************************************
    var decode_leds = function (instruction) {
        var stop = (instruction & 0x00ff0000) >> 16;
        var start = (instruction & 0x0000ff00) >> 8;
        var result = '';
        var led_bit_mask = 0;

        // If all used LEDs are used in the instruction then output 'all leds'
        // instead of a giant list of leds.
        if (start === 0  &&  stop === (NUMBER_OF_LEDS - 1)) {
            return 'all leds';
        }

        while (start <= stop) {
            // Remember which LEDs are used here so that we can weed out unused
            // leds in the decleration later
            if (start < 32) {
                led_bit_mask = Math.pow(2, start);
                if (!(asm[leds_to_declare_offset].leds_to_declare & led_bit_mask)) {
                    asm[leds_to_declare_offset].leds_to_declare += led_bit_mask;
                }

                if (result !== '') {
                    result += ', ';
                }
                result += 'led' + start++;
            }
            else {
                led_bit_mask = Math.pow(2, (start - 32));
                if (!(asm[leds_to_declare_offset].leds2_to_declare & led_bit_mask)) {
                    asm[leds_to_declare_offset].leds2_to_declare += led_bit_mask;
                }

                if (result !== '') {
                    result += ', ';
                }
                result += 'led' + start++;
            }
        }

        return result;
    };


    // *************************************************************************
    var decode_right_parameter = function (instruction) {
        var parameter_type;
        var index;

        parameter_type = (instruction >> 8) & 0xff;
        index = instruction & 0xff;

        switch (parameter_type) {
        case PARAMETER_TYPE_VARIABLE:
            var name = decode_predefined_variables(index);
            if (name != '') {
                return name;
            }

            set_variable_used(index, current_program);
            return 'var' + index;

        case PARAMETER_TYPE_LED:
            return 'led' + index;

        case PARAMETER_TYPE_RANDOM:
            return 'random';

        default:
            return 'ERROR: unknown parameter type ' + parameter_type;
        }
    };

    // *************************************************************************
    var decode_variable_assignment = function (instruction, operator) {
        var index;
        var result = '';

        index = (instruction >> 16) & 0xff;

        var name = decode_predefined_variables(index);
        if (name != '') {
            result = name + ' ' + operator + ' ';
        }
        else {
            set_variable_used(index, current_program);
            result = 'var' + index + ' ' + operator + ' ';
        }

        if (instruction & INSTRUCTION_MODIFIER_IMMEDIATE) {
            var number = instruction & 0xffff;
            var decimal = number;
            if (decimal >= 0x8000) {
                decimal = -0x10000 + decimal;
            }
            return result + decimal +
                '   // 0x' + number.toString(16).toUpperCase();
        }

        return result + decode_right_parameter(instruction);
    };


    // *************************************************************************
    var decode_test_parameter = function (instruction) {
        var parameter_type;
        var index;

        if (instruction & INSTRUCTION_MODIFIER_IMMEDIATE) {
            var number = instruction & 0xffff;
            var decimal = number;
            if (decimal >= 0x8000) {
                decimal = -0x10000 + decimal;
            }
            return decimal + '   // 0x' + number.toString(16).toUpperCase();
        }

        parameter_type = (instruction >> 8) & 0xff;
        index = instruction & 0xff;
        set_variable_used(index, current_program);

        switch (parameter_type) {
        case PARAMETER_TYPE_VARIABLE:
            var name = decode_predefined_variables(index);
            if (name != '') {
                return name;
            }
            set_variable_used(index, current_program);
            return 'var' + index;

        case PARAMETER_TYPE_LED:
            return 'led' + index;

        default:
            return 'ERROR: unknown parameter type ' + parameter_type;
        }
    };


    // *************************************************************************
    var decode_skip_if = function (opcode, instruction) {
        var left;
        var right;

        if (opcode & 0x02) {
            left = 'led' + ((instruction >> 16) & 0xff);
        } else {
            var index = (instruction >> 16) & 0xff;

            var name = decode_predefined_variables(index);
            if (name != '') {
                left = name;
            }
            else {
                set_variable_used(index, current_program);
                left = 'var' + index;
            }
        }

        right = decode_test_parameter(instruction);

        switch (opcode) {
        case opcodes.SKIP_IF_EQ_V:
        case opcodes.SKIP_IF_EQ_VI:
        case opcodes.SKIP_IF_EQ_L:
        case opcodes.SKIP_IF_EQ_LI:
            return left + ' == ' + right;

        case opcodes.SKIP_IF_NE_V:
        case opcodes.SKIP_IF_NE_VI:
        case opcodes.SKIP_IF_NE_L:
        case opcodes.SKIP_IF_NE_LI:
            return left + ' != ' + right;

        case opcodes.SKIP_IF_GE_V:
        case opcodes.SKIP_IF_GE_VI:
        case opcodes.SKIP_IF_GE_L:
        case opcodes.SKIP_IF_GE_LI:
            return left + ' >= ' + right;

        case opcodes.SKIP_IF_GT_V:
        case opcodes.SKIP_IF_GT_VI:
        case opcodes.SKIP_IF_GT_L:
        case opcodes.SKIP_IF_GT_LI:
            return left + ' > ' + right;

        case opcodes.SKIP_IF_LE_V:
        case opcodes.SKIP_IF_LE_VI:
        case opcodes.SKIP_IF_LE_L:
        case opcodes.SKIP_IF_LE_LI:
            return left + ' <= ' + right;

        case opcodes.SKIP_IF_LT_V:
        case opcodes.SKIP_IF_LT_VI:
        case opcodes.SKIP_IF_LT_L:
        case opcodes.SKIP_IF_LT_LI:
            return left + ' < ' + right;

        default:
            return 'ERROR: unknown "skip if" opcode ' + opcode;
        }
    };


    // *************************************************************************
    var decode_car_state = function (instruction, singular, plural) {
        var result = '';
        var count = 0;

        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_0) {
            result +=  ' light-switch-position0';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_1) {
            result += ' light-switch-position1';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_2) {
            result += ' light-switch-position2';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_3) {
            result += ' light-switch-position3';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_4) {
            result += ' light-switch-position4';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_5) {
            result += ' light-switch-position5';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_6) {
            result += ' light-switch-position6';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_7) {
            result += ' light-switch-position7';
            ++count;
        }
        if (instruction & CAR_STATE_LIGHT_SWITCH_POSITION_8) {
            result += ' light-switch-position8';
            ++count;
        }
        if (instruction & CAR_STATE_NEUTRAL) {
            result += ' neutral';
            ++count;
        }
        if (instruction & CAR_STATE_FORWARD) {
            result += ' forward';
            ++count;
        }
        if (instruction & CAR_STATE_REVERSING) {
            result += ' reversing';
            ++count;
        }
        if (instruction & CAR_STATE_BRAKING) {
            result += ' braking';
            ++count;
        }
        if (instruction & CAR_STATE_INDICATOR_LEFT) {
            result += ' indicator-left';
            ++count;
        }
        if (instruction & CAR_STATE_INDICATOR_RIGHT) {
            result += ' indicator-right';
            ++count;
        }
        if (instruction & CAR_STATE_HAZARD) {
            result += ' hazard';
            ++count;
        }
        if (instruction & CAR_STATE_BLINK_FLAG) {
            result += ' blink-flag';
            ++count;
        }
        if (instruction & CAR_STATE_BLINK_LEFT) {
            result += ' blink-left';
            ++count;
        }
        if (instruction & CAR_STATE_BLINK_RIGHT) {
            result += ' blink-right';
            ++count;
        }
        if (instruction & CAR_STATE_WINCH_DISABLERD) {
            result += ' winch-disabled';
            ++count;
        }
        if (instruction & CAR_STATE_WINCH_IDLE) {
            result += ' winch-idle';
            ++count;
        }
        if (instruction & CAR_STATE_WINCH_IN) {
            result += ' winch-in';
            ++count;
        }
        if (instruction & CAR_STATE_WINCH_OUT) {
            result += ' winch-out';
            ++count;
        }
        if (instruction & CAR_STATE_SERVO_OUTPUT_SETUP_CENTRE) {
            result += ' servo-output-setup-centre';
            ++count;
        }
        if (instruction & CAR_STATE_SERVO_OUTPUT_SETUP_LEFT) {
            result += ' servo-output-setup-left';
            ++count;
        }
        if (instruction & CAR_STATE_SERVO_OUTPUT_SETUP_RIGHT) {
            result += ' servo-output-setup-right';
            ++count;
        }
        if (instruction & CAR_STATE_REVERSING_SETUP_STEERING) {
            result += ' reversing-setup-steering';
            ++count;
        }
        if (instruction & CAR_STATE_REVERSING_SETUP_THROTTLE) {
            result += ' reversing-setup-throttle';
            ++count;
        }

        return ' ' + ((count > 1) ? plural : singular) + result;
    };


    // *************************************************************************
    var process_opcode = function (opcode, instruction) {
        if ((opcode & 0xe0) === OPCODE_SKIP_IF_ANY) {
            asm[offset + pc++].code =
                'skip if' + decode_car_state(instruction, 'is', 'any');
            return STATE_PROGRAM;
        }
        if ((opcode & 0xe0) === OPCODE_SKIP_IF_ALL) {
            asm[offset + pc++].code =
                'skip if' + decode_car_state(instruction, 'all', 'all');
            return STATE_PROGRAM;
        }
        if ((opcode & 0xe0) === OPCODE_SKIP_IF_NONE) {
            asm[offset + pc++].code =
                'skip if' + decode_car_state(instruction, 'not', 'none');
            return STATE_PROGRAM;
        }


        if (opcode >= FIRST_SKIP_IF_OPCODE  &&  opcode <= LAST_SKIP_IF_OPCODE) {
            asm[offset + pc++].code =
                'skip if ' + decode_skip_if(opcode, instruction);
            return STATE_PROGRAM;
        }


        switch (opcode) {
        case opcodes.END_OF_PROGRAM:
            asm[offset + pc++].decleration = ''; // Empty line
            asm[offset + pc++].decleration = 'end';
            asm[offset + pc++].decleration = ''; // Empty line

            return STATE_END_OF_PROGRAM;

        case opcodes.GOTO:
            var address = (instruction & 0xffffff);

            if (asm[offset + address].label === null) {
                asm[offset + address].label = 'label' + address;
            }

            asm[offset + pc++].code = 'goto ' + asm[offset + address].label;
            break;

        case opcodes.SET:
            asm[offset + pc++].code =
                decode_leds(instruction) + ' = ' +
                    // SET is slightly different, only allows var.
                    // So we mask the rest off as to not misinterprete
                    // the other fields
                    decode_right_parameter(instruction & 0xff);
            break;

        case opcodes.SET_I:
            asm[offset + pc++].code =
                decode_leds(instruction) + ' = ' + (instruction & 0xff) + '%';
            break;

        case opcodes.SLEEP:
            asm[offset + pc++].code = 'sleep ' + decode_right_parameter(instruction);
            break;

        case opcodes.SLEEP_I:
            asm[offset + pc++].code = 'sleep ' + (instruction & 0xffff);
            break;

        case opcodes.FADE:
            asm[offset + pc++].code = 'fade ' +
                decode_leds(instruction) + ' stepsize ' +
                    decode_right_parameter(instruction & 0xff);
            break;

        case opcodes.FADE_I:
            if ((instruction & 0xff) !== 0) {
                asm[offset + pc++].code = 'fade ' +
                    decode_leds(instruction) + ' stepsize ' +
                        (instruction & 0xff) + '%';
            } else {
                asm[offset + pc++].code = 'fade ' +
                    decode_leds(instruction) + ' stepsize 0   // (no fading)';

            }
            break;

        case opcodes.ASSIGN:
        case opcodes.ASSIGN_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '=');
            break;

        case opcodes.ADD:
        case opcodes.ADD_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '+=');
            break;

        case opcodes.SUBTRACT:
        case opcodes.SUBTRACT_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '-=');
            break;

        case opcodes.MULTIPLY:
        case opcodes.MULTIPLY_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '*=');
            break;

        case opcodes.DIVIDE:
        case opcodes.DIVIDE_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '/=');
            break;

        case opcodes.AND:
        case opcodes.AND_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '&=');
            break;

        case opcodes.OR:
        case opcodes.OR_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '|=');
            break;

        case opcodes.XOR:
        case opcodes.XOR_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '^=');
            break;

        case opcodes.MOD:
        case opcodes.MOD_I:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '%=');
            break;

        case opcodes.ABS:
            asm[offset + pc++].code =
                decode_variable_assignment(instruction, '= abs');
            break;

        default:
            asm[offset + pc++].code =
                'TODO 0x' + instruction.toString(16);
            break;
        }

        return STATE_PROGRAM;
    };


    // *************************************************************************
    var process_instruction = function (instruction) {
        var opcode = (instruction >> 24) & 0xff;
        // console.log(opcode.toString(16));

        switch (state) {
        case STATE_PRIORITY:
            decode_priority_run_condition(instruction);
            state = STATE_RUN;
            break;

        case STATE_RUN:
            decode_run_condition(instruction);
            asm[offset++].decleration = '';  // Empty line
            state = STATE_LEDS_USED;
            break;

        case STATE_LEDS_USED:
            leds_used_instruction = instruction
            state = STATE_LEDS2_USED;
            break;

        case STATE_LEDS2_USED:
            decode_leds_used(leds_used_instruction, instruction);
            var_offsets.push(offset);
            state = STATE_PROGRAM;
            break;

        case STATE_PROGRAM:
            state = process_opcode(opcode, instruction);
            break;

        case STATE_END_OF_PROGRAM:
            if (opcode === opcodes.END_OF_PROGRAMS) {
                state = STATE_IGNORE;
            } else {
                offset += pc;
                pc = 0;

                ++current_program;

                decode_priority_run_condition(instruction);
                state = STATE_RUN;
            }
            break;

        case STATE_IGNORE:
            break;
        }
    };


    // *************************************************************************
    var determine_type_of_variables = function () {
        var index;
        var usage_count;
        var program;

        for (index in variables) {
            if (variables.hasOwnProperty(index)) {
                usage_count = 0;

                for (program in variables[index]) {
                    if (variables[index].hasOwnProperty(program)) {
                        ++usage_count;
                    }
                }

                variables[index].type = usage_count > 1 ? 'global var' : 'var';
            }
        }
    };


    // *************************************************************************
    var insert_variable_declerations = function (current_program) {
        var i;
        var indexes = [];
        var index;
        var result = '';

        for (index in variables) {
            if (variables.hasOwnProperty(index)) {
                indexes.push(index);
            }
        }

        indexes = indexes.sort();

        for (i = 0; i < indexes.length; i += 1) {
            index = indexes[i];
            if (variables[index][current_program] !== undefined) {
                result += variables[index].type + ' var' + index + '\n';
            }
        }

        if (result !== '') {
            result += '\n';
        }

        return result;
    };


    // *************************************************************************
    var output_source_code = function () {
        var source_code = '';
        var program = 1;
        var i;
        var led_bit_mask;
        var leds_to_declare;
        var leds2_to_declare;

        for (i = 0; i < (offset + pc); i++) {
            if (i === var_offsets[program - 1]) {
                source_code += insert_variable_declerations(program);

                ++program;
            }

            if (asm[i].leds_to_declare !== null) {
                leds_to_declare = asm[i].leds_to_declare;
            }
            if (asm[i].leds2_to_declare !== null) {
                leds2_to_declare = asm[i].leds2_to_declare;
            }
            if (asm[i].decleration !== null) {
                if (asm[i].led === null) {
                    source_code += asm[i].decleration + '\n';
                } else {
                    if (asm[i].led < 32) {
                        led_bit_mask = Math.pow(2, asm[i].led);
                        if (leds_to_declare & led_bit_mask) {
                            source_code += asm[i].decleration + '\n';
                        }
                    }
                    else {
                        led_bit_mask = Math.pow(2, (asm[i].led - 32));
                        if (leds2_to_declare & led_bit_mask) {
                            source_code += asm[i].decleration + '\n';
                        }
                    }
                }
            }
            if (asm[i].label !== null) {
                source_code += asm[i].label + ':\n';
            }
            if (asm[i].code !== null) {
                source_code += '    ' + asm[i].code + '\n';
            }
        }

        return source_code;
    };


    // *************************************************************************
    var disassemble = function (instructions) {
        var i;

        for (i = 0; i < asm.length; i++) {
            asm[i] = {
                'decleration' : null,
                'label' : null,
                'code' : null,
                'led' : null,
                'leds_to_declare' : null,
                'leds2_to_declare' : null
            };
        }

        variables = {};
        var_offsets = [];
        current_program = 1;

        offset = 0;
        pc = 0;
        state = STATE_PRIORITY;

        instructions.forEach(function (instruction) {
            // 'instruction' could be either a hex-string, or a number.
            // We apply the proper decoding, converting it into a
            // number for further processing.
            if (('' + instruction).startsWith('0x')) {
                instruction = parseInt(instruction, 16);
            }
            else {
                instruction = parseInt(instruction, 10);
            }
            process_instruction(instruction);
        });

        determine_type_of_variables();

        return output_source_code();
    };


    // *************************************************************************
    // Extract the instructions from a C source code fragment defining the
    // light_programs data structure LIGHT_PROGRAMS_T
    var parse_c_code = function (input) {
        var instructions = [];
        var lines = input.split('\n');
        var index;
        var line;
        var instruction;

        for (index = 0; index < lines.length; index += 1) {
            line = lines[index];
            instruction = line.match(/(0x[0-9a-fA-F]{8})/);
            if (instruction) {
                instructions.push(instruction[0]);
            }
        }

        return instructions;
    };


    // *************************************************************************
    // API of this module:
    // *************************************************************************
    return {
        disassemble: disassemble,
        parse_c_code: parse_c_code
    };
}());


// node.js exports; hide from browser where exports is undefined and use strict
// would trigger.
if (typeof exports !== 'undefined') {
    exports.disassembler = disassembler;
}
