"use strict";

var app = (function () {
    var el = {};  // Cache of document.getElementById

    var SYSTICK_IN_MS = 20;

    var firmware;
    var config;
    var config_version;
    var local_leds;
    var slave_leds;
    var gamma_object;
    var light_programs;

    var LIGHT_SWITCH_POSITIONS = 9;
    var MAX_LIGHT_PROGRAMS = 25;
    var MAX_LIGHT_PROGRAM_VARIABLES = 100;



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


    var ESC_FORWARD_BRAKE_REVERSE_TIMEOUT = "Forward/Brake/Reverse with timeout";
    var ESC_FORWARD_BRAKE_REVERSE = "Forward/Brake/Reverse no timeout";
    var ESC_FORWARD_REVERSE = "Forward/Reverse";
    var ESC_FORWARD_BRAKE = "Forward/Brake";

    var ESC_MODE = {
        0: ESC_FORWARD_BRAKE_REVERSE_TIMEOUT,
        1: ESC_FORWARD_BRAKE_REVERSE,
        2: ESC_FORWARD_REVERSE,
        3: ESC_FORWARD_BRAKE,

        ESC_FORWARD_BRAKE_REVERSE_TIMEOUT: 0,
        ESC_FORWARD_BRAKE_REVERSE: 1,
        ESC_FORWARD_REVERSE: 2,
        ESC_FORWARD_BRAKE: 3
    };


    var BAUDRATES = {
        0: 38400,
        1: 115200,

        38400: 0,
        115200: 1
    };


    // *************************************************************************
    var parse_leds = function (section) {
        var data = firmware.data;
        var offset = firmware.offset[section];
        var result = {};

        var led_count = data[offset];
        result['led_count'] = led_count;

        var car_lights_offset = get_uint32(data, offset + 4);

        function parse_led (data, offset) {
            var result = {}

            result['max_change_per_systick'] = data[offset];
            result['reduction_percent'] = data[offset + 1];

            var flags = get_uint16(data, offset + 2);

            function get_flag(bit_mask) {
                return Boolean(flags & bit_mask);
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
            result['weak_brake_light'] = get_flag(1 << 10);
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
    var parse_gamma = function () {
        var data = firmware.data;
        var offset = firmware.offset[SECTION_GAMMA];

        var gamma = {};

        gamma['gamma_value'] = String.fromCharCode(data[offset + 0]) +
            String.fromCharCode(data[offset + 1]) +
            String.fromCharCode(data[offset + 2]);

        return gamma;
    }


    // *************************************************************************
    var parse_configuration = function () {
        var data = firmware.data;
        var offset = firmware.offset[SECTION_CONFIG];

        var config = {};

        config['firmware_version'] = data[offset];

        config['mode'] = data[offset + 1];
        config['esc_mode'] = data[offset + 2];

        var flags = get_uint32(data, offset + 4);

        function get_flag(bit_mask) {
            return Boolean(flags & bit_mask);
        }

        config['slave_ouput'] = get_flag(1 << 0);
        config['preprocessor_output'] = get_flag(1 << 1);
        config['winch_output'] = get_flag(1 << 2);
        config['steering_wheel_servo_output'] = get_flag(1 << 3);
        config['gearbox_servo_output'] = get_flag(1 << 4);
        config['ch3_is_local_switch'] = get_flag(1 << 5);
        config['ch3_is_momentary'] = get_flag(1 << 6);
        config['auto_brake_lights_forward_enabled'] = get_flag(1 << 7);
        config['auto_brake_lights_reverse_enabled'] = get_flag(1 << 8);

        config['auto_brake_counter_value_forward_min'] =
            get_uint16(data, offset + 8);
        config['auto_brake_counter_value_forward_max'] =
            get_uint16(data, offset + 10);
        config['auto_brake_counter_value_reverse_min'] =
            get_uint16(data, offset + 12);
        config['auto_brake_counter_value_reverse_max'] =
            get_uint16(data, offset + 14);
        config['auto_reverse_counter_value_min'] =
            get_uint16(data, offset + 16);
        config['auto_reverse_counter_value_max'] =
            get_uint16(data, offset + 18);
        config['brake_disarm_counter_value'] = get_uint16(data, offset + 20);
        config['blink_counter_value'] = get_uint16(data, offset + 22);
        config['indicator_idle_time_value'] = get_uint16(data, offset + 24);
        config['indicator_off_timeout_value'] = get_uint16(data, offset + 26);

        config['centre_threshold_low'] = get_uint16(data, offset + 28);
        config['centre_threshold_high'] = get_uint16(data, offset + 30);
        config['blink_threshold'] = get_uint16(data, offset + 32);
        config['light_switch_positions'] = get_uint16(data, offset + 34);
        config['initial_endpoint_delta'] = get_uint16(data, offset + 36);
        config['ch3_multi_click_timeout'] = get_uint16(data, offset + 38);
        config['winch_command_repeat_time'] = get_uint16(data, offset + 40);

        // 2 byte padding

        config['baudrate'] = get_uint32(data, offset + 44);
        config['no_signal_timeout'] = get_uint16(data, offset + 48);
        config['number_of_gears'] = get_uint16(data, offset + 50);

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
    var set_uint16 = function (data, offset, value) {
        data[offset] = value & 0xff;
        data[offset + 1] = (value >> 8) & 0xff;
    };


    // *************************************************************************
    var set_uint32 = function (data, offset, value) {
        data[offset] = value & 0xff;
        data[offset + 1] = (value >> 8) & 0xff;
        data[offset + 2] = (value >> 16) & 0xff;
        data[offset + 3] = (value >> 24) & 0xff;
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

        return disassembler.disassemble(instructions);
    };


    // *************************************************************************
    var find_magic_markers = function (image_data) {
        // LBrc (LANE Boys RC) in little endian
        var ROM_MAGIC = [0x4c, 0x42, 0x72, 0x63];
        var ROM_MAGIC_LENGTH = 4;

        var result = {};

        for (var i = 0; i < image_data.length; i++) {
            if (image_data.slice(i, i + ROM_MAGIC_LENGTH).join() ==
                    ROM_MAGIC.join()) {

                var section_id = (image_data[i + 5] << 8) + image_data[i + 4];
                config_version = (image_data[i + 7] << 8) + image_data[i + 6];

                if (typeof SECTIONS[section_id] === "undefined") {
                    console.log("Warning: unknown section " + i);
                    continue;
                }

                if (config_version != 1) {
                    throw new Error("Unknown configuration version " +
                        config_version);
                }

                var section = SECTIONS[section_id];
                result[section] = i + 8;
            }
        }

        return result;
    };


    // *************************************************************************
    var parse_firmware_structure = function (intel_hex_data) {
        var image_data;
        var offset_list;

        image_data = intel_hex.parse(intel_hex_data);
        offset_list = find_magic_markers(image_data.data);

        return {
            data: image_data.data,
            offset: offset_list,
        }
    };


    // *************************************************************************
    var parse_firmware = function (intel_hex_data) {
        firmware = undefined;
        config = undefined;
        local_leds = undefined
        slave_leds = undefined;
        gamma_object = undefined;
        light_programs = "";

        el["light_programs"].value = "";
        el['light_programs_errors'].style.display = "none";

        try {
            firmware = parse_firmware_structure(intel_hex_data);
            config = parse_configuration();
            local_leds = parse_leds(SECTION_LOCAL_LEDS);
            slave_leds = parse_leds(SECTION_SLAVE_LEDS);
            light_programs = disassemble_light_programs();
            gamma_object = parse_gamma();

            update_ui();

            el["light_programs"].value = light_programs;
            ui.update_editor();
        }
        catch (e) {
            window.alert(
                "Unable to load Intel-hex formatted firmware image:\n" + e);
        }
    };


    // *************************************************************************
    var parse_light_program_code = function (light_programs) {
        el['light_programs_errors'].style.display = 'none';

        // If we run multiple times, we need to reset the modules inbetween,
        // especially if there was an error before.
        symbols.reset();
        emitter.reset();

        try {
            var machine_code = parser.parse(light_programs);
            return machine_code;
        }
        catch (e) {
            el['light_programs_errors'].innerHTML = e;
            el['light_programs_errors'].style.display = '';
            throw new Error(e);
        }
    };


    // *************************************************************************
    var assemble_gamma = function (gamma_object) {
        var data = firmware.data;
        var offset = firmware.offset[SECTION_GAMMA];

        data[offset] = gamma_object['gamma_value'].charCodeAt(0);
        data[offset + 1] = gamma_object['gamma_value'].charCodeAt(1);
        data[offset + 2] = gamma_object['gamma_value'].charCodeAt(2);

        var gamma_table = gamma.make_table(gamma_object['gamma_value']);
        for (var i = 0; i < gamma_table.length; i++) {
            data[offset + 4 + i] = gamma_table[i];
        }
    }


    // *************************************************************************
    var assemble_leds = function (section, led_object) {
        var data = firmware.data;
        var offset = firmware.offset[section];
        var car_lights_offset = get_uint32(data, offset + 4);

        function assemble_led(data, offset, led_object) {
            data[offset] = led_object['max_change_per_systick'];
            data[offset + 1] = led_object['reduction_percent'];

            var flags = 0;

            flags |= led_object['weak_light_switch_position0'] << 0;
            flags |= led_object['weak_light_switch_position1'] << 1;
            flags |= led_object['weak_light_switch_position2'] << 2;
            flags |= led_object['weak_light_switch_position3'] << 3;
            flags |= led_object['weak_light_switch_position4'] << 4;
            flags |= led_object['weak_light_switch_position5'] << 5;
            flags |= led_object['weak_light_switch_position6'] << 6;
            flags |= led_object['weak_light_switch_position7'] << 7;
            flags |= led_object['weak_light_switch_position8'] << 8;
            flags |= led_object['weak_tail_light'] << 9;
            flags |= led_object['weak_brake_light'] << 10;
            flags |= led_object['weak_reversing_light'] << 11;
            flags |= led_object['weak_indicator_left'] << 12;
            flags |= led_object['weak_indicator_right'] << 13;

            set_uint16(data, offset + 2, flags);

            data[offset + 4] = led_object['always_on'];
            data[offset + 5] = led_object['light_switch_position0'];
            data[offset + 6] = led_object['light_switch_position1'];
            data[offset + 7] = led_object['light_switch_position2'];
            data[offset + 8] = led_object['light_switch_position3'];
            data[offset + 9] = led_object['light_switch_position4'];
            data[offset + 10] = led_object['light_switch_position5'];
            data[offset + 11] = led_object['light_switch_position6'];
            data[offset + 12] = led_object['light_switch_position7'];
            data[offset + 13] = led_object['light_switch_position8'];
            data[offset + 14] = led_object['tail_light'];
            data[offset + 15] = led_object['brake_light'];
            data[offset + 16] = led_object['reversing_light'];
            data[offset + 17] = led_object['indicator_left'];
            data[offset + 18] = led_object['indicator_right'];
        }

        data[offset] = led_object['led_count'];

        for (var i = 0; i < led_object['led_count']; i++) {
            assemble_led(data, car_lights_offset + (i * 20), led_object[i]);
        }
    };


    // *************************************************************************
    var assemble_configuration = function (config) {
        var data = firmware.data;
        var offset = firmware.offset[SECTION_CONFIG];

        data[offset] = config['firmware_version'];
        data[offset + 1] = config['mode'];
        data[offset + 2] = config['esc_mode'];

        var flags = 0;
        flags |= (config['slave_ouput'] << 0);
        flags |= (config['preprocessor_output'] << 1);
        flags |= (config['winch_output'] << 2);
        flags |= (config['steering_wheel_servo_output'] << 3);
        flags |= (config['gearbox_servo_output'] << 4);
        flags |= (config['ch3_is_local_switch'] << 5);
        flags |= (config['ch3_is_momentary'] << 6);
        flags |= (config['auto_brake_lights_forward_enabled'] << 7);
        flags |= (config['auto_brake_lights_reverse_enabled'] << 8);
        set_uint32(data, offset + 4, flags);

        set_uint16(data, offset + 8, config['auto_brake_counter_value_forward_min']);
        set_uint16(data, offset + 10, config['auto_brake_counter_value_forward_max']);
        set_uint16(data, offset + 12, config['auto_brake_counter_value_reverse_min']);
        set_uint16(data, offset + 14, config['auto_brake_counter_value_reverse_max']);
        set_uint16(data, offset + 16, config['auto_reverse_counter_value_min']);
        set_uint16(data, offset + 18, config['auto_reverse_counter_value_max']);

        set_uint16(data, offset + 20, config['brake_disarm_counter_value']);
        set_uint16(data, offset + 22, config['blink_counter_value']);
        set_uint16(data, offset + 24, config['indicator_idle_time_value']);
        set_uint16(data, offset + 26, config['indicator_off_timeout_value']);

        set_uint16(data, offset + 28, config['centre_threshold_low']);
        set_uint16(data, offset + 30, config['centre_threshold_high']);
        set_uint16(data, offset + 32, config['blink_threshold']);
        set_uint16(data, offset + 34, config['light_switch_positions']);
        set_uint16(data, offset + 36, config['initial_endpoint_delta']);
        set_uint16(data, offset + 38, config['ch3_multi_click_timeout']);
        set_uint16(data, offset + 40, config['winch_command_repeat_time']);

        // 2 byte padding

        set_uint32(data, offset + 44, config['baudrate']);
        set_uint16(data, offset + 48, config['no_signal_timeout']);
        set_uint16(data, offset + 50, config['number_of_gears']);
    }


    // *************************************************************************
    var assemble_light_programs = function (light_programs) {
        var machine_code = parse_light_program_code(light_programs);

        var code_size = 4 + (4 * MAX_LIGHT_PROGRAMS) +
            (4 * machine_code.instructions.length);

        var code = new Array(code_size);

        set_uint32(code, 0, machine_code.number_of_programs);
        for (var i = 0; i < MAX_LIGHT_PROGRAMS; i++) {
            set_uint32(code, 4 + (4 * i), machine_code.start_offset[i]);
        }

        var first_program_offset = 4 + (4 * MAX_LIGHT_PROGRAMS);

        for (var i = 0; i < machine_code.instructions.length; i++) {
            set_uint32(code, first_program_offset + (4 * i),
                machine_code.instructions[i]);
        }

        var data = firmware.data;
        var offset = firmware.offset[SECTION_LIGHT_PROGRAMS];

        firmware.data = data.slice(0, offset).concat(code);


    }


    // *************************************************************************
    var assemble_firmware = function () {
        assemble_configuration(config);
        assemble_leds(SECTION_LOCAL_LEDS, local_leds);
        assemble_leds(SECTION_SLAVE_LEDS, slave_leds);
        assemble_light_programs(light_programs);
        assemble_gamma(gamma_object);
    }


    // *************************************************************************
    var load_default_firmware = function () {
        parse_firmware(default_firmware_image);
    }


    // *************************************************************************
    var load_firmware_from_disk = function () {
        if (this.files.length < 1) {
            return;
        }

        var reader = new FileReader();
        reader.onload = function (e) {
            parse_firmware(e.target.result);
        };
        reader.readAsText(this.files[0]);
    }


    // *************************************************************************
    var save_firmware = function () {
        // Update data based on UI
        update_config();

        try {
            assemble_firmware();
        }
        catch (e) {
            window.alert(
                "Failed to assemble the firmware.\n" +
                "Please check the light programs for correctness.\n" + e);
            return;
        }

        var intelhex = intel_hex.fromArray(firmware.data);

        var blob = new Blob([intelhex], {type: "text/plain;charset=utf-8"});

        var filename = window.prompt(
            'Filename for the firmware image:', 'light_controller.hex');

        if (filename != null  &&  filename != "") {
            saveAs(blob, filename);
        }
    };


    // *************************************************************************
    var load_configuration_from_disk = function () {
        if (this.files.length < 1) {
            return;
        }

        var reader = new FileReader();
        reader.onload = function (e) {
            try {
                var data = JSON.parse(e.target.result);

                config = data['config'];
                local_leds = data['local_leds'];
                slave_leds = data['slave_leds'];
                light_programs = data['light_programs'];
                gamma_object = data['gamma'];
            }
            catch (e) {
                window.alert(
                    "Failed to load configuration.\n" +
                    "File may not be a light controller configuration file (JSON format)");
            }

            ui.update_editor();
            update_ui();
        };
        reader.readAsText(this.files[0]);
    }


    // *************************************************************************
    var save_configuration = function () {
        // Update data based on UI
        update_config();

        var data = {};
        data['config'] = config;
        data['local_leds'] = local_leds;
        data['slave_leds'] = slave_leds;
        data['gamma'] = gamma_object;
        data['light_programs'] = light_programs;

        var configuration_string = JSON.stringify(data, null, 2);

        var blob = new Blob(
            [configuration_string], {type: "text/plain;charset=utf-8"});

        var filename = window.prompt(
            'Filename for the configuration file:',
            'light_controller.config.txt')

        if (filename != null  &&  filename != "") {
            saveAs(blob, filename);
        }
    };


    // *************************************************************************
    var update_led_feature_usage = function () {
        function set_feature_active(led_div_id, prefix) {
            var led_div = document.getElementById(led_div_id);
            var led_config_rows = led_div.getElementsByClassName("led_config");
            var led_feature_rows =
                led_div.getElementsByClassName("led_features");

            for (var i = 0; i < led_feature_rows.length; i++) {
                var advanced_feature_used = false;

                var incandescent = parseInt(document.getElementById(
                    prefix + i + "incandescent").value, 10);

                var weak_ground = parseInt(document.getElementById(
                    prefix + i + "weak_ground").value, 10);

                var any_checkbox_ticked = false;
                var checkboxes = led_feature_rows[i].getElementsByClassName(
                    "checkbox");
                for (var c = 0; c < checkboxes.length; c++) {
                    if (checkboxes[c].checked) {
                        any_checkbox_ticked = true;
                        break;
                    }
                }

                if (incandescent > 0  && incandescent < 100) {
                    advanced_feature_used = true;
                }

                if (weak_ground > 0  &&  any_checkbox_ticked) {
                    advanced_feature_used = true;
                }


                var spanner =
                    led_config_rows[i] .getElementsByClassName("spanner")[0];

                if (advanced_feature_used) {
                    addClass(spanner, "led_feature_active");
                }
                else {
                    removeClass(spanner, "led_feature_active");
                }

            }
        }

        set_feature_active("leds_master", "master");
        set_feature_active("leds_slave", "slave");
    };


    // *************************************************************************
    var update_led_fields = function () {
        function set_led_field(prefix, led_number, field_number, value) {
            var cell = document.getElementById(
                "" + prefix + led_number + "field" + field_number);

            while (cell.firstChild) {
                cell.removeChild(cell.firstChild);
            }

            value = Math.round(value * 100 / 255);

            if (value != 0) {
                var textNode = document.createTextNode(value);
                cell.appendChild(textNode);
            }
        }

        function set_led_feature(prefix, led_number, field_suffix, value) {
            var cell = document.getElementById(
                "" + prefix + led_number + field_suffix);

            if (cell.type == "checkbox") {
                cell.checked = Boolean(value);
            }
            else {
                cell.value = value;
            }
        }

        function set_led_fields(led_source, prefix) {
            for (var i = 0; i < led_source["led_count"]; i++) {
                var led = led_source[i];

                set_led_field(prefix, i, 0, led["always_on"]);
                set_led_field(prefix, i, 1, led["light_switch_position0"]);
                set_led_field(prefix, i, 2, led["light_switch_position1"]);
                set_led_field(prefix, i, 3, led["light_switch_position2"]);
                set_led_field(prefix, i, 4, led["light_switch_position3"]);
                set_led_field(prefix, i, 5, led["light_switch_position4"]);
                set_led_field(prefix, i, 6, led["light_switch_position5"]);
                set_led_field(prefix, i, 7, led["light_switch_position6"]);
                set_led_field(prefix, i, 8, led["light_switch_position7"]);
                set_led_field(prefix, i, 9, led["light_switch_position8"]);
                set_led_field(prefix, i, 10, led["tail_light"]);
                set_led_field(prefix, i, 11, led["brake_light"]);
                set_led_field(prefix, i, 12, led["reversing_light"]);
                set_led_field(prefix, i, 13, led["indicator_left"]);
                set_led_field(prefix, i, 14, led["indicator_right"]);

                set_led_feature(prefix, i, "incandescent",
                    Math.round(led["max_change_per_systick"] * 100 / 255));
                set_led_feature(
                    prefix, i, "weak_ground", led["reduction_percent"]);
                set_led_feature(
                    prefix, i, "checkbox0", led["weak_light_switch_position0"]);
                set_led_feature(
                    prefix, i, "checkbox1", led["weak_light_switch_position1"]);
                set_led_feature(
                    prefix, i, "checkbox2", led["weak_light_switch_position2"]);
                set_led_feature(
                    prefix, i, "checkbox3", led["weak_light_switch_position3"]);
                set_led_feature(
                    prefix, i, "checkbox4", led["weak_light_switch_position4"]);
                set_led_feature(
                    prefix, i, "checkbox5", led["weak_light_switch_position5"]);
                set_led_feature(
                    prefix, i, "checkbox6", led["weak_light_switch_position6"]);
                set_led_feature(
                    prefix, i, "checkbox7", led["weak_light_switch_position7"]);
                set_led_feature(
                    prefix, i, "checkbox8", led["weak_light_switch_position8"]);
                set_led_feature(
                    prefix, i, "checkbox9", led["weak_tail_light"]);
                set_led_feature(
                    prefix, i, "checkbox10", led["weak_brake_light"]);
                set_led_feature(
                    prefix, i, "checkbox11", led["weak_reversing_light"]);
                set_led_feature(
                    prefix, i, "checkbox12", led["weak_indicator_left"]);
                set_led_feature(
                    prefix, i, "checkbox13", led["weak_indicator_right"]);
            }
        }

        set_led_fields(local_leds, "master");
        set_led_fields(slave_leds, "slave");
    }


    // *************************************************************************
    var update_led_config = function () {
        function get_led_field(prefix, led_number, field_number) {
            var cell = document.getElementById(
                "" + prefix + led_number + "field" + field_number);

            var value = cell.textContent;
            if (value == "") {
                return 0;
            }

            return Math.round(parseInt(value, 10) * 255 / 100);
        }

        function get_led_feature(prefix, led_number, field_suffix) {
            var cell = document.getElementById(
                "" + prefix + led_number + field_suffix);

            if (cell.type == "checkbox") {
                return Boolean(cell.checked);
            }
            else {
                return parseInt(cell.value, 10);
            }
        }

        function get_led_fields(led_source, prefix) {
            for (var i = 0; i < led_source["led_count"]; i++) {
                var led = led_source[i];

                led["always_on"] = get_led_field(prefix, i, 0);
                led["light_switch_position0"] = get_led_field(prefix, i, 1);
                led["light_switch_position1"] = get_led_field(prefix, i, 2);
                led["light_switch_position2"] = get_led_field(prefix, i, 3);
                led["light_switch_position3"] = get_led_field(prefix, i, 4);
                led["light_switch_position4"] = get_led_field(prefix, i, 5);
                led["light_switch_position5"] = get_led_field(prefix, i, 6);
                led["light_switch_position6"] = get_led_field(prefix, i, 7);
                led["light_switch_position7"] = get_led_field(prefix, i, 8);
                led["light_switch_position8"] = get_led_field(prefix, i, 9);
                led["tail_light"] = get_led_field(prefix, i, 10);
                led["brake_light"] = get_led_field(prefix, i, 11);
                led["reversing_light"] = get_led_field(prefix, i, 12);
                led["indicator_left"] = get_led_field(prefix, i, 13);
                led["indicator_right"] = get_led_field(prefix, i, 14);

                led["max_change_per_systick"] = Math.round(
                    get_led_feature(prefix, i, "incandescent") * 255 / 100);
                led["reduction_percent"] =
                    get_led_feature(prefix, i, "weak_ground");
                led["weak_light_switch_position0"] =
                    get_led_feature(prefix, i, "checkbox0");
                led["weak_light_switch_position1"] =
                    get_led_feature(prefix, i, "checkbox1");
                led["weak_light_switch_position2"] =
                    get_led_feature(prefix, i, "checkbox2");
                led["weak_light_switch_position3"] =
                    get_led_feature(prefix, i, "checkbox3");
                led["weak_light_switch_position4"] =
                    get_led_feature(prefix, i, "checkbox4");
                led["weak_light_switch_position5"] =
                    get_led_feature(prefix, i, "checkbox5");
                led["weak_light_switch_position6"] =
                    get_led_feature(prefix, i, "checkbox6");
                led["weak_light_switch_position7"] =
                    get_led_feature(prefix, i, "checkbox7");
                led["weak_light_switch_position8"] =
                    get_led_feature(prefix, i, "checkbox8");
                led["weak_tail_light"] =
                    get_led_feature(prefix, i, "checkbox9");
                led["weak_brake_light"] =
                    get_led_feature(prefix, i, "checkbox10");
                led["weak_reversing_light"] =
                    get_led_feature(prefix, i, "checkbox11");
                led["weak_indicator_left"] =
                    get_led_feature(prefix, i, "checkbox12");
                led["weak_indicator_right"] =
                    get_led_feature(prefix, i, "checkbox13");
            }
        }

        get_led_fields(local_leds, "master");
        get_led_fields(slave_leds, "slave");
    };


    // *************************************************************************
    var update_config = function () {

        function update_int(key) {
            config[key] = parseInt(el[key].value, 10);
        }

        function update_boolean(key) {
            config[key] = Boolean(el[key].checked);
        }

        function update_time(key) {
            config[key] = Math.round(el[key].value / SYSTICK_IN_MS);
        }

        function update_gamma(key) {
            var g = parseFloat(el[key].value);

            if (isNaN(g)) {
                gamma_object[key] = "2.2";
                return
            }

            if (g < 0.1) {
                gamma_object[key] = "0.1";
                return
            }
            if (g > 9.9) {
                gamma_object[key] = "9.9";
                return
            }

            g = g.toString();
            if (g.length < 3) {
                // Append .0 if we are dealing with an integer value
                g += ".0";
            }

            gamma_object[key] = g
        }

        // Master/Slave
        update_int("mode");


        // ESC mode
        for (var i = 0; i <  el["esc"].length; i++) {
            if (el["esc"][i].checked) {
                config['esc_mode'] = parseInt(el["esc"][i].value, 10);
                break;
            }
        }


        // Output functions
        update_boolean('slave_output');
        update_boolean('preprocessor_output');
        update_boolean('steering_wheel_servo_output');
        update_boolean('gearbox_servo_output');
        update_boolean('winch_output');


        // CH3/AUX type
        config['ch3_is_momentary'] = false;
        config['ch3_is_local_switch'] = false;
        if (el["ch3"][1].checked) {
            config['ch3_is_momentary'] = true;
        }
        else if (el["ch3"][2].checked) {
            config['ch3_is_local_switch'] = true;
        }


        // Baudrate
        update_int("baudrate")


        // LEDs
        update_led_config();


        // Update advanced settings
        update_boolean("auto_brake_lights_forward_enabled");
        update_time("auto_brake_counter_value_forward_min");
        update_time("auto_brake_counter_value_forward_max");

        update_boolean("auto_brake_lights_reverse_enabled");
        update_time("auto_brake_counter_value_reverse_min");
        update_time("auto_brake_counter_value_reverse_max");

        update_time("brake_disarm_counter_value");

        update_time("auto_reverse_counter_value_min");
        update_time("auto_reverse_counter_value_max");

        update_time("blink_counter_value");
        update_time("indicator_idle_time_value");
        update_time("indicator_off_timeout_value");
        update_int("blink_threshold");

        update_int("centre_threshold_low");
        update_int("centre_threshold_high");
        update_int("initial_endpoint_delta");

        update_time("ch3_multi_click_timeout");
        update_time("winch_command_repeat_time");
        update_time("no_signal_timeout");
        update_int("number_of_gears");

        update_gamma("gamma_value");

        light_programs = ui.get_editor_content();
    }


    // *************************************************************************
    var update_ui = function () {
        // Master/Slave
        el["mode"].selectedIndex = config["mode"];

        // Firmware version
        el["firmware_version"].innerHTML =
            "" + config_version + "." + config['firmware_version'];

        // ESC type
        el["esc"][config['esc_mode']].checked = true;

        // Output functions
        el["winch_output"].checked = Boolean(config['winch_output']);
        el["gearbox_servo_output"].checked =
            Boolean(config['gearbox_servo_output']);
        el["steering_wheel_servo_output"].checked =
            Boolean(config['steering_wheel_servo_output']);
        el["preprocessor_output"].checked =
            Boolean(config['preprocessor_output']);
        el["slave_output"].checked = Boolean(config['slave_output']);


        // CH3/AUX type
        el["ch3"][0].checked = true;
        if (config['ch3_is_local_switch']) {
            el["ch3"][2].checked = true;
        }
        else if (config['ch3_is_momentary']) {
            el["ch3"][1].checked = true;
        }

        // Baudrate
        el["baudrate"].selectedIndex = BAUDRATES[config['baudrate']];


        // LEDs
        update_led_fields();

        // Update advanced settings
        el["auto_brake_lights_forward_enabled"].checked =
            Boolean(config["auto_brake_lights_forward_enabled"]);
        el["auto_brake_counter_value_forward_min"].value =
            config["auto_brake_counter_value_forward_min"] * SYSTICK_IN_MS;
        el["auto_brake_counter_value_forward_max"].value =
            config["auto_brake_counter_value_forward_max"] * SYSTICK_IN_MS;

        el["auto_brake_lights_reverse_enabled"].checked =
            Boolean(config["auto_brake_lights_reverse_enabled"]);
        el["auto_brake_counter_value_reverse_min"].value =
            config["auto_brake_counter_value_reverse_min"] * SYSTICK_IN_MS;
        el["auto_brake_counter_value_reverse_max"].value =
            config["auto_brake_counter_value_reverse_max"] * SYSTICK_IN_MS;

        el["brake_disarm_counter_value"].value =
            config["brake_disarm_counter_value"] * SYSTICK_IN_MS;

        el["auto_reverse_counter_value_min"].value =
            config["auto_reverse_counter_value_min"] * SYSTICK_IN_MS;
        el["auto_reverse_counter_value_max"].value =
            config["auto_reverse_counter_value_max"] * SYSTICK_IN_MS;

        el["blink_counter_value"].value =
            config["blink_counter_value"] * SYSTICK_IN_MS;
        el["indicator_idle_time_value"].value =
            config["indicator_idle_time_value"] * SYSTICK_IN_MS;
        el["indicator_off_timeout_value"].value =
            config["indicator_off_timeout_value"] * SYSTICK_IN_MS;
        el["blink_threshold"].value = config["blink_threshold"];

        el["centre_threshold_low"].value = config["centre_threshold_low"];
        el["centre_threshold_high"].value = config["centre_threshold_high"];
        el["initial_endpoint_delta"].value = config["initial_endpoint_delta"];

        el["ch3_multi_click_timeout"].value =
            config["ch3_multi_click_timeout"] * SYSTICK_IN_MS;
        el["winch_command_repeat_time"].value =
            config["winch_command_repeat_time"] * SYSTICK_IN_MS;
        el["no_signal_timeout"].value =
            config["no_signal_timeout"] * SYSTICK_IN_MS;
        el["number_of_gears"].value = config["number_of_gears"];

        el["gamma_value"].value = gamma_object["gamma_value"];


        // Show/hide various sections depending on the current settings
        update_section_visibility();

        // Highlight the spanner icon if incandescent of weak ground simulation
        // is active for a LED
        update_led_feature_usage();
    };


    // *************************************************************************
    var update_section_visibility = function () {
        function set_name(elements, name) {
            for (var i = 0; i < elements.length; i++) {
                elements[i].name = name;
            }
        }

        function set_visibility(elements, value) {
            for (var i = 0; i < elements.length; i++) {
                elements[i].style.display = value;
            }
        }

        function ensure_one_is_checked(name) {
            var elements = document.getElementsByName(name);
            for (var i = 0; i < elements.length; i++) {
                if (elements[i].checked) {
                    return;
                }
            }
            elements[0].checked = true;
        }

        var new_mode = parseInt(
            el["mode"].options[el["mode"].selectedIndex].value, 10);

        switch (new_mode) {
            case MODE['MASTER_WITH_SERVO_READER']:
                el["mode_master_servo"].style.display = "";
                el["mode_master_uart"].style.display = "none";
                el["mode_slave"].style.display = "none";
                el["config_light_programs"].style.display = "";
                el["config_leds"].style.display = "";
                el["config_basic_esc_type"].style.display = "";
                el["config_basic_ch3"].style.display = "";
                el["config_basic_output"].style.display = "";
                el["config_advanced"].style.display = "";
                set_visibility(el["single_output"], "");
                set_visibility(el["dual_output"], "none");
                set_name(el["dual_output_th"], "output_out");
                config["mode"] = new_mode;
                break;

            case MODE['MASTER_WITH_UART_READER']:
                el["mode_master_servo"].style.display = "none";
                el["mode_master_uart"].style.display = "";
                el["mode_slave"].style.display = "none";
                el["config_light_programs"].style.display = "";
                el["config_leds"].style.display = "";
                el["config_basic_esc_type"].style.display = "";
                el["config_basic_ch3"].style.display = "";
                el["config_basic_output"].style.display = "";
                el["config_advanced"].style.display = "";
                set_visibility(el["single_output"], "none");
                set_visibility(el["dual_output"], "");
                set_name(el["dual_output_th"], "output_th");
                config["mode"] = new_mode;
                break;

            case MODE['SLAVE']:
                el["mode_master_servo"].style.display = "none";
                el["mode_master_uart"].style.display = "none";
                el["mode_slave"].style.display = "";
                el["config_light_programs"].style.display = "none";
                el["config_leds"].style.display = "none";
                el["config_basic_esc_type"].style.display = "none";
                el["config_basic_ch3"].style.display = "none";
                el["config_basic_output"].style.display = "none";
                el["config_advanced"].style.display = "none";
                config["mode"] = new_mode;
                break;
        }

        ensure_one_is_checked("output_out");
        ensure_one_is_checked("output_th");

        el["leds_slave"].style.display =
            Boolean(el["slave_output"].checked) ? "" : "none";
    };


    // *************************************************************************
    var init_assembler = function () {
        parser.yy = {
            symbols: symbols,
            emitter: emitter,
            logger: logger,
        }

        logger.set_log_level("ERROR");

        emitter.set_parser(parser);
        symbols.set_parser(parser);
    }


    // *************************************************************************
    var init = function () {

        function set_led_feature_handler(led_id, prefix) {
            var led_section = document.getElementById(led_id);
            var feature_rows =
                led_section.getElementsByClassName("led_features");

            for (var row = 0; row < feature_rows.length; row++) {
                var input_elements =
                    feature_rows[row].getElementsByTagName("input");

                for (var i = 0; i < input_elements.length; i++) {
                    input_elements[i].addEventListener(
                        "change", update_led_feature_usage, true);
                }
            }
        }

        el["firmware_version"] = document.getElementById("firmware_version");
        el["save_config"] = document.getElementById("save_config");
        el["load_config"] = document.getElementById("load_config");
        el["save_firmware"] = document.getElementById("save_firmware");
        el["load_firmware"] = document.getElementById("load_firmware");

        el["mode"] = document.getElementById("mode");
        el["mode_master_servo"] = document.getElementById("mode_master_servo");
        el["mode_master_uart"] = document.getElementById("mode_master_uart");
        el["mode_slave"] = document.getElementById("mode_slave");

        el["config_leds"] = document.getElementById("config_leds");
        el["leds_master"] = document.getElementById("leds_master");
        el["leds_slave"] = document.getElementById("leds_slave");

        el["config_basic"] = document.getElementById("config_basic");
        el["config_basic_esc_type"] =
            document.getElementById("config_basic_esc_type");
        el["config_basic_ch3"] = document.getElementById("config_basic_ch3");
        el["config_basic_output"] =
            document.getElementById("config_basic_output");
        el["config_basic_baudrate"] =
            document.getElementById("config_basic_baudrate");

        el["baudrate"] = document.getElementById("baudrate");
        el["esc"] = document.getElementsByName("esc");
        el["ch3"] = document.getElementsByName("ch3");

        el["output_out"] = document.getElementsByName("output_out");
        el["single_output"] = document.getElementsByClassName("single_output");
        el["dual_output"] = document.getElementsByClassName("dual_output");
        el["dual_output_th"] =
            document.getElementsByClassName("dual_output_th");

        el["slave_output"] = document.getElementById("slave_output");
        el["preprocessor_output"] =
            document.getElementById("preprocessor_output");
        el["steering_wheel_servo_output"] =
            document.getElementById("steering_wheel_servo_output");
        el["gearbox_servo_output"] =
            document.getElementById("gearbox_servo_output");
        el["winch_output"] = document.getElementById("winch_output");

        el["config_light_programs"] =
            document.getElementById("config_light_programs");
        el["light_programs"] = document.getElementById("light_programs");
        el["light_programs_errors"] =
            document.getElementById("light_programs_errors");
        el["light_programs_assembler"] =
            document.getElementById("light_programs_assembler");


        el["config_advanced"] = document.getElementById("config_advanced");

        el["auto_brake_lights_forward_enabled"] =
            document.getElementById("auto_brake_lights_forward_enabled");
        el["auto_brake_counter_value_forward_min"] =
            document.getElementById("auto_brake_counter_value_forward_min");
        el["auto_brake_counter_value_forward_max"] =
            document.getElementById("auto_brake_counter_value_forward_max");

        el["auto_brake_lights_reverse_enabled"] =
            document.getElementById("auto_brake_lights_reverse_enabled");
        el["auto_brake_counter_value_reverse_min"] =
            document.getElementById("auto_brake_counter_value_reverse_min");
        el["auto_brake_counter_value_reverse_max"] =
            document.getElementById("auto_brake_counter_value_reverse_max");

        el["brake_disarm_timeout_enabled"] =
            document.getElementById("brake_disarm_timeout_enabled");
        el["brake_disarm_counter_value"] =
            document.getElementById("brake_disarm_counter_value");

        el["auto_reverse_counter_value_min"] =
            document.getElementById("auto_reverse_counter_value_min");
        el["auto_reverse_counter_value_max"] =
            document.getElementById("auto_reverse_counter_value_max");

        el["blink_counter_value"] =
            document.getElementById("blink_counter_value");
        el["indicator_idle_time_value"] =
            document.getElementById("indicator_idle_time_value");
        el["indicator_off_timeout_value"] =
            document.getElementById("indicator_off_timeout_value");
        el["blink_threshold"] = document.getElementById("blink_threshold");

        el["centre_threshold_low"] =
            document.getElementById("centre_threshold_low");
        el["centre_threshold_high"] =
            document.getElementById("centre_threshold_high");


        el["initial_endpoint_delta"] =
            document.getElementById("initial_endpoint_delta");
        el["ch3_multi_click_timeout"] =
            document.getElementById("ch3_multi_click_timeout");
        el["winch_command_repeat_time"] =
            document.getElementById("winch_command_repeat_time");
        el["no_signal_timeout"] = document.getElementById("no_signal_timeout");
        el["number_of_gears"] = document.getElementById("number_of_gears");

        el["gamma_value"] = document.getElementById("gamma_value");


        el["mode"].addEventListener(
            "change", update_section_visibility, false);

        el["config_basic_output"].addEventListener(
            "change", update_section_visibility, false);

        set_led_feature_handler("leds_master", "master");
        set_led_feature_handler("leds_slave", "slave");

        el["load_firmware"].addEventListener(
            "change", load_firmware_from_disk, false);

        el["save_firmware"].addEventListener(
            "click", save_firmware, false);

        el["load_config"].addEventListener(
            "change", load_configuration_from_disk, false);

        el["save_config"].addEventListener(
            "click", save_configuration, false);

        el["light_programs_assembler"].addEventListener(
            "click", function () {
                parse_light_program_code(ui.get_editor_content());
                // FIXME: add message that assembly succeeded
                console.log("Assembly successful.");
            } , false);

        init_assembler();
        load_default_firmware();
    };


    // *************************************************************************
    return {
        load: load_firmware_from_disk,
        init: init
    };
})();


// *****************************************************************************
document.addEventListener("DOMContentLoaded", function () {
    ui.init();
    app.init();
}, false);