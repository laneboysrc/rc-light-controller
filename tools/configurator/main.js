"use strict";

var app = (function () {
    var firmware;
    var config;
    var local_leds;
    var slave_leds;

    var LIGHT_SWITCH_POSITIONS = 9;
    var MAX_LIGHT_PROGRAMS = 25;
    var MAX_LIGHT_PROGRAM_VARIABLES = 100;

    var CONFIG_VERSION = 1;


    var SECTION_CONFIG = "Configuration";
    var SECTION_GAMMA = "Gamma table";
    var SECTION_LIGHT_PROGRAMS = "Light programs";
    var SECTION_LOCAL_LEDS = "Local LEDs";
    var SECTION_SLAVE_LEDS = "Slave LEDs";

    var SECTIONS = {
        0x01: SECTION_CONFIG,
        0x02: SECTION_GAMMA,
        0x10: SECTION_LOCAL_LEDS,
        0x20: SECTION_SLAVE_LEDS,
        0x30: SECTION_LIGHT_PROGRAMS,

        SECTION_CONFIG: 0x01,
        SECTION_GAMMA: 0x02,
        SECTION_LOCAL_LEDS: 0x10,
        SECTION_SLAVE_LEDS: 0x20,
        SECTION_LIGHT_PROGRAMS: 0x30
    };


    var MASTER_WITH_SERVO_READER = "Master, servo inputs";
    var MASTER_WITH_UART_READER = "Master, pre-processor input";
    var SLAVE = "Slave";

    var MODE = {
        0: MASTER_WITH_SERVO_READER,
        1: MASTER_WITH_UART_READER,
        2: SLAVE,

        MASTER_WITH_SERVO_READER: 0,
        MASTER_WITH_UART_READER: 1,
        SLAVE: 2
    };


    // *************************************************************************
    var parse_leds = function (section) {
        var data = firmware.data;
        var offset = firmware.offset[section];
        var result = {};

        var led_count = data[offset];
        result['led_count'] = led_count;

        var car_lights_offset = get_uint32(data, offset + 4);
        console.log('car_lights_offset=0x' + car_lights_offset.toString(16));

        function parse_led (data, offset) {
            var result = {}

            result['max_change_per_systick'] = data[offset];
            result['reduction_percent'] = data[offset + 1];

            var flags = get_uint16(data, offset + 2);

            function get_flag(bit_mask) {
                return (flags & bit_mask) ? true : false;
            }

            result['weak_light_switch_position0'] = get_flag(1 << 0);
            result['weak_light_switch_position1'] = get_flag(1 << 1);
            result['weak_light_switch_position2'] = get_flag(1 << 2);
            result['weak_light_switch_position3'] = get_flag(1 << 3);
            result['weak_light_switch_position4'] = get_flag(1 << 4);
            result['weak_light_switch_position5'] = get_flag(1 << 5);
            result['weak_light_switch_position6'] = get_flag(1 << 6);
            result['weak_light_switch_position7'] = get_flag(1 << 7);
            result['weak_light_switch_position8'] = get_flag(1 << 8);
            result['weak_tail_light'] = get_flag(1 << 9);
            result['weak_tail_light'] = get_flag(1 << 10);
            result['weak_reversing_light'] = get_flag(1 << 11);
            result['weak_indicator_left'] = get_flag(1 << 12);
            result['weak_indicator_right'] = get_flag(1 << 13);

            result['always_on'] = data[offset + 4];
            result['light_switch_position0'] = data[offset + 5];
            result['light_switch_position1'] = data[offset + 6];
            result['light_switch_position2'] = data[offset + 7];
            result['light_switch_position3'] = data[offset + 8];
            result['light_switch_position4'] = data[offset + 9];
            result['light_switch_position5'] = data[offset + 10];
            result['light_switch_position6'] = data[offset + 11];
            result['light_switch_position7'] = data[offset + 12];
            result['light_switch_position8'] = data[offset + 13];
            result['tail_light'] = data[offset + 14];
            result['brake_light'] = data[offset + 15];
            result['reversing_light'] = data[offset + 16];
            result['indicator_left'] = data[offset + 17];
            result['indicator_right'] = data[offset + 18];

            return result;
        }

        for (var i = 0; i < led_count; i++) {
            result[i] = parse_led(data, car_lights_offset + (i * 20));
        }

        return result;
    };


    // *************************************************************************
    var parse_configuration = function () {
        var data = firmware.data;
        var offset = firmware.offset[SECTION_CONFIG];

        var config = {};

        config['mode'] = get_uint32(data, offset);

        var flags = get_uint32(data, offset + 4);

        function get_flag(bit_mask) {
            return (flags & bit_mask) ? true : false;
        }

        config['slave_ouput'] = get_flag(1 << 0);
        config['preprocessor_output'] = get_flag(1 << 1);
        config['winch_output'] = get_flag(1 << 2);
        config['steering_wheel_servo_output'] = get_flag(1 << 3);
        config['gearbox_servo_output'] = get_flag(1 << 4);
        config['esc_forward_reverse'] = get_flag(1 << 5);
        config['ch3_is_local_switch'] = get_flag(1 << 6);
        config['ch3_is_momentary'] = get_flag(1 << 7);
        config['auto_brake_lights_forward_enabled'] = get_flag(1 << 8);
        config['auto_brake_lights_reverse_enabled'] = get_flag(1 << 9);
        config['brake_disarm_timeout_enabled'] = get_flag(1 << 10);

        config['auto_brake_counter_value_forward_min'] = get_uint16(data, offset + 8);
        config['auto_brake_counter_value_forward_max'] = get_uint16(data, offset + 10);
        config['auto_brake_counter_value_reverse_min'] = get_uint16(data, offset + 12);
        config['auto_brake_counter_value_reverse_max'] = get_uint16(data, offset + 14);
        config['auto_reverse_counter_value_min'] = get_uint16(data, offset + 16);
        config['auto_reverse_counter_value_max'] = get_uint16(data, offset + 18);
        config['brake_disarm_counter_value'] = get_uint16(data, offset + 20);
        config['blink_counter_value'] = get_uint16(data, offset + 22);
        config['indicator_idle_time_value'] = get_uint16(data, offset + 24);
        config['indicator_off_timeout_value'] = get_uint16(data, offset + 26);

        config['centre_threshold_low'] = get_uint16(data, offset + 28);
        config['centre_threshold'] = get_uint16(data, offset + 30);
        config['centre_threshold_high'] = get_uint16(data, offset + 32);
        config['blink_threshold'] = get_uint16(data, offset + 34);
        config['light_switch_positions'] = get_uint16(data, offset + 36);
        config['initial_endpoint_delta'] = get_uint16(data, offset + 38);
        config['ch3_multi_click_timeout'] = get_uint16(data, offset + 40);
        config['winch_command_repeat_time'] = get_uint16(data, offset + 42);

        config['baudrate'] = get_uint32(data, offset + 44);
        config['no_signal_timeout'] = get_uint16(data, offset + 48);

        return config;
    };


    // *************************************************************************
    var get_uint16 = function (data, offset) {
        return (data[offset + 1] << 8) + data[offset];
    };


    // *************************************************************************
    var get_uint32 = function (data, offset) {
        return (data[offset + 3] << 24) + (data[offset + 2] << 16) +
            (data[offset + 1] << 8) + data[offset];
    };


    // *************************************************************************
    var uint8_array_to_uint32 = function (uint8_array) {
        var uint32_array = [];
        var i = 0;

        while ((i + 4) <=  uint8_array.length) {
            uint32_array.push(get_uint32(uint8_array, i));
            i += 4;
        }
        return uint32_array;
    }


    // *************************************************************************
    var disassemble_light_programs = function () {
        var data = firmware.data;
        var offset = firmware.offset[SECTION_LIGHT_PROGRAMS];
        var first_program_offset = offset + 4 + (4 * MAX_LIGHT_PROGRAMS);

        var number_of_programs = get_uint32(data, offset);

        var instructions =
            uint8_array_to_uint32(data.slice(first_program_offset));

        var code = disassembler.disassemble(instructions);
        document.getElementById("light_programs").innerHTML = code;
    };


    // *************************************************************************
    var find_magic_markers = function (image_data) {
        // LBrc (LANE Boys RC) in little endian
        var ROM_MAGIC = [0x4c, 0x42, 0x72, 0x63];
        var ROM_MAGIC_LENGTH = 4;

        var result = {};

        for (var i = 0; i < image_data.length; i++) {
            if (image_data.slice(i, i + ROM_MAGIC_LENGTH).join() == ROM_MAGIC.join()) {

                var section_id = (image_data[i + 5] << 8) + image_data[i + 4];
                var version = (image_data[i + 7] << 8) + image_data[i + 6];

                if (typeof SECTIONS[section_id] === "undefined") {
                    console.log("Warning: unknown section " + i);
                    continue;
                }

                if (version != 1) {
                    throw new Error("Unknown configuration version " + version);
                }

                var section = SECTIONS[section_id];
                result[section] = i + 8;
            }
        }

        return result;
    };


    // *************************************************************************
    var load_firmware = function (intel_hex_data) {
        var ih = intel_hex;

        var image_data = ih.parse(intel_hex_data);
        var offset_list = find_magic_markers(image_data.data);

        return {
            data: image_data.data,
            offset: offset_list,
        }
    };


    // *************************************************************************
    var run = function () {
        firmware = load_firmware(default_firmware_image);
        config = parse_configuration();
        local_leds = parse_leds(SECTION_LOCAL_LEDS);
        slave_leds = parse_leds(SECTION_SLAVE_LEDS);
        disassemble_light_programs();
    };


    // *************************************************************************
    return {
        run: run
    };
})();


document.addEventListener("DOMContentLoaded", function () {
    app.run();
}, false);