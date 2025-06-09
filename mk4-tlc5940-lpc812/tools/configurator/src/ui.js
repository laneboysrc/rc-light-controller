'use strict';
/*global CodeMirror, tmpl */

var ui = (function () {
    var editor;
    var light_program_errors = [];

    let current_tab = 0;
    const tab_classes = ['led_table_car', 'led_table_diagnostics'];

    // *************************************************************************
    var tab_next_handler = function (e) {
        current_tab += 1;
        update_tab_visibility();
    };


    // *************************************************************************
    var tab_previous_handler = function (e) {
        current_tab -= 1;
        update_tab_visibility();
    };

    // *************************************************************************
    var led_prefix_and_id_parser = function (led_string) {
        const [prefix] = led_string.split(/\d/, 1);
        const led_id = parseInt(led_string.substr(prefix.length));
        return [prefix, led_id];
    }

    // *************************************************************************
    var load_led_name = function (prefix, led_id) {
        console.log(`prefix: ${prefix}, led_id: ${led_id}`, app.get_data()[prefix + '_leds'])
        const leds = app.get_data()[prefix + '_leds'];
        const led = leds[led_id];
        return led.name;
    }

    // *************************************************************************
    var save_led_name = function (prefix, led_id, new_name) {
        const leds = app.get_data()[prefix + '_leds'];
        const led = leds[led_id];
        led.name = new_name;
    }

    // *************************************************************************
    var led_feature_click_handler = function (e) {
        const features_rows = document.getElementsByClassName(e.target.name);
        const current_led_feature_hidden = features_rows[0].classList.contains('hidden');

        // Close all led_feature rows
        var features = document.getElementsByClassName('led_features');
        for (let i = 0; i < features.length; i += 1) {
            features[i].classList.add('hidden');
        }

        // Show all LED names
        const led_names = document.querySelectorAll('.led_config .led_name');
        for (let led_name of led_names) {
            led_name.classList.remove('hidden');
        }

        // If the led_feature row where the spanner is clicked is currently
        // closed, then open it.
        if (current_led_feature_hidden) {
            // Load the current LED name into the edit field
            const [prefix, led_id] = led_prefix_and_id_parser(e.target.name);
            const edit_field = document.querySelector('#' + prefix + led_id + 'name_input');
            edit_field.value = load_led_name(prefix, led_id);

            // Unhide the led_feature rows
            for (let i = 0; i < features_rows.length; i++) {
                features_rows[i].classList.remove('hidden');
            }

            // Hide the LED name (since we have the edit field visible)
            document.querySelector('#' + prefix + led_id + 'name').classList.add('hidden');
        }
    };


    // *************************************************************************
    var led_config_click_handler = function (e) {
        var item = e.target;

        if (item.nodeName !== 'TD') {
            return;
        }

        var cell = document.getElementById(item.id);

        var originalValue = parseInt(cell.textContent, 10);
        if (isNaN(originalValue) || !isFinite(originalValue)) {
            originalValue = 0;
        }

        var input = document.createElement('input');
        input.type = 'number';
        input.min = 0;
        input.max = 100;
        input.step = 1;
        input.value = originalValue;

        while (cell.firstChild) {
            cell.removeChild(cell.firstChild);
        }

        // input.setSelectionRange(0, 99);
        cell.appendChild(input);
        input.focus();
        input.select();

        function blur_handler(e) {
            var blur_cell = e.target.parentNode;

            var newValue = parseInt(input.value, 10);
            if (isNaN(newValue)  ||  !isFinite(newValue)) {
                newValue = 0;
            }
            else if (newValue < input.min) {
                newValue = parseInt(input.min, 10);
            }
            else if (newValue > input.max) {
                newValue = parseInt(input.max, 10);
            }

            while (blur_cell.firstChild) {
                blur_cell.removeChild(cell.firstChild);
            }

            if (newValue !== 0) {
                var textNode = document.createTextNode(newValue);
                blur_cell.appendChild(textNode);
            }
        }

        function key_handler(e) {
            var target = e.target;

            switch (e.keyCode) {
            case 27: // ESC
                target.value = originalValue;
                target.blur();
                break;

            case 13: // ENTER
                target.blur();
                break;

            default:
                break;
            }
        }

        input.addEventListener('blur', blur_handler, false);
        input.addEventListener('keydown', key_handler, false);
    };


    // *************************************************************************
    var init_led_editing = function () {

        function init_led_section(section, prefix) {
            var led_section = document.getElementById(section);
            var led_rows = led_section.getElementsByClassName('led_config');
            var fields;
            var i;
            var spanner;

            for (let led_id = 0; led_id < led_rows.length; led_id += 1) {
                fields = led_rows[led_id].querySelectorAll('td.led_table_car');
                for (i = 0; i < fields.length; i += 1) {
                    fields[i].id = prefix + led_id + 'field' + i;
                    fields[i].addEventListener('click', led_config_click_handler, true);
                }

                fields = led_rows[led_id].querySelectorAll('td.led_table_diagnostics input');
                for (i = 0; i < fields.length; i += 1) {
                    // The field order in the HTML is different than the
                    // bit-field in led_id.diagnostics
                    // We therefore have to apply some mapping. We would break
                    // backwards compatibility if we re-arrange the
                    // led.diagnostics order (dicdated by PRIORITY_RUN_CONDITION)

                    // The index is the UI index, the output is the index within
                    // the led.diagnostics bit field
                    const diag_ui_map = [
                        1,  // RUN_WHEN_INITIALIZING
                        0,  // RUN_WHEN_NO_SIGNAL
                        5,  // RUN_WHEN_REVERSING_SETUP_STEERING
                        6,  // RUN_WHEN_REVERSING_SETUP_THROTTLE
                        3,  // RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT
                        2,  // RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE
                        4,  // RUN_WHEN_SERVO_OUTPUT_SETUP_RIGH
                    ];

                    fields[i].id = prefix + led_id + 'diag' + diag_ui_map[i];
                }

                // Update the the edited LED name straight away so that
                const edit_field = document.querySelector('#' + prefix + led_id + 'name_input');
                const label_field = document.querySelector('#' + prefix + led_id + 'name');
                edit_field.addEventListener('input', () => {
                    save_led_name(prefix, led_id, edit_field.value);
                    label_field.textContent = edit_field.value;
                });

                spanner = led_rows[led_id].getElementsByClassName('spanner')[0];
                spanner.name = prefix + led_id + 'features';
                spanner.addEventListener('click', led_feature_click_handler, true);
            }
        }


        init_led_section('leds_master', 'master');
        init_led_section('leds_slave', 'slave');
        init_led_section('leds_slaveA', 'slaveA');
        init_led_section('leds_slaveB', 'slaveB');

        let tab_buttons = document.getElementsByClassName('tab-next');
        for (let i = 0; i < tab_buttons.length; i += 1) {
            tab_buttons[i].addEventListener('click', tab_next_handler, true);
        }
        tab_buttons = document.getElementsByClassName('tab-previous');
        for (let i = 0; i < tab_buttons.length; i += 1) {
            tab_buttons[i].addEventListener('click', tab_previous_handler, true);
        }
    };


    // *************************************************************************
    var update_tab_visibility = function () {
        // Support wrap around through simply incrementing and decrementing
        // current_tab before calling this function.
        if (current_tab < 0) {
            current_tab = tab_classes.length - 1;
        }
        if (current_tab >= tab_classes.length) {
            current_tab = 0;
        }

        for (let i = 0; i < tab_classes.length; i++) {
            const elements = document.getElementsByClassName(tab_classes[i]);
            for (let j = 0; j < elements.length; j++) {
                const e = elements[j]
                if (i === current_tab) {
                    e.classList.remove('hidden');
                }
                else {
                    e.classList.add('hidden');
                }
            }
        }
    }


    // *************************************************************************
    var clear_led_tables = function () {
        var i;
        var rows;

        rows = document.querySelectorAll('.led_config');
        for (i = 0; i < rows.length; i += 1) {
            var row = rows[i];
            row.parentNode.removeChild(row);
        }
    };


    // *************************************************************************
    var init_led_tables = function () {

        function init_led_table(section, led_offset, prefix) {
            var led_section = document.getElementById(section);
            var led_tbody = led_section.getElementsByTagName('tbody')[0];
            var template_function = tmpl('led_row_template');
            var i;
            var helper;


            var html = '<table>';
            for (i = 0; i < 16; i += 1) {
                html += template_function({
                    'even_odd': (i % 2) ? 'odd' : 'even', 'led_index': i,
                    'led_number': i + led_offset, 'prefix': prefix,
                });
            }
            html += '</table>';

            // IE does not allow adding rows to a table via appendChild.
            //
            // http://stackoverflow.com/questions/7180072/script-600-error-invalid-target-element-for-this-operation
            //
            // The soluition is to have a hidden div where we append a dummy
            // table, and then move the children over to the actual table. This
            // relies on having a tbody element.

            helper = document.getElementById('table_helper');
            helper.innerHTML = html;

            helper = helper.getElementsByTagName('tbody')[0];

            while (helper.firstChild !== null) {
                led_tbody.appendChild(helper.firstChild);
            }
        }

        clear_led_tables();
        init_led_table('leds_master', 0, 'master');
        init_led_table('leds_slave', 16, 'slave');
        init_led_table('leds_slaveA', 32, 'slaveA');
        init_led_table('leds_slaveB', 48, 'slaveB');
    };


    // *************************************************************************
    var init_tooltips = function () {
        function set_tooltip(name, help_text) {
            const elements = document.querySelectorAll(`[data-tooltip="${name}"]`);

            for (let i = 0; i < elements.length; i += 1) {
                elements[i].title = help_text;
            }
        }

        set_tooltip('help_led_light_switch',
            'The virtual light switch has 9 positions. ' +
            'A single click on AUX/CH3 increments the light switch position, ' +
            'a double-click decrements the light switch position.\n' +
            'If you want an LED to be on at multiple positions simply fill ' +
            'in values at all the desired light switch positions where the ' +
            'LED should turn on.\n' +
            'Example: For parking lights (aka positioning lights), set a ' +
            'value for light switch position 1. For main beam (aka. head ' +
            'lamps) set a value at both light switch position 1 and 2. For ' +
            'high beam set a value at light switch positions 1, 2 and 3. ' +
            'This way the light behaviour corresponds to a real car in most ' +
            'countries.');

        set_tooltip('help_led_always_on',
            'The LED will be always on at the given ' +
            'brightness, regardless of the car state.');

        set_tooltip('help_led_tail',
            'Tail light (aka. rear position light). This ' +
            'function applies when the light position switch is at any state ' +
            'other than 0.');

        set_tooltip('help_led_brake',
            'Brake light function.\nHint: to do a ' +
            'combined tail and brake light, set the \'tail\' entry to a low ' +
            'value (e.g. 33%), and the \'brake\' entry to a high value ' +
            '(e.g. 100%).');

        set_tooltip('help_led_reverse',
            'Reversing light function; turns on when the ' +
            'car is driving backwards.\nNote: see main configuration above ' +
            'to configure your ESC type!');

        set_tooltip('help_led_indicator',
            'Indicator, aka. turn signals.\nAlso applies ' +
            'to the hazard light function.');

        set_tooltip('help_click_to_change', 'Click to change');

        set_tooltip('help_toggle_features',
            'Click to show/hide advanced LED features and give the LED a name');

        set_tooltip('help_incandescent',
            'LEDs emit light instantly, while incandescent lamps gradually ' +
            'increase in brightness over a short amount of time.\n' +
            'By setting a value greater than 0 in this field, the LED only ' +
            'changes brightness up to the given percentage in a period of ' +
            '20 ms, and therfore fading in and out rather than instantly ' +
            'turning on or off.\n' +
            'The smaller the value, the slower the LED fades. A value of 15% ' +
            'results in a fade time of approx 130 ms from off to on, ' +
            'which is a realistic simulation for incandescent bulbs in ' +
            'vintage cars.\n' +
            'Note that slow fading causes visible steps at lower brightness ' +
            'values due to the limited control range of the LED driver.');

        set_tooltip('help_weak_ground',
            'In old cars the ground connection often corrodes, especially on ' +
            'rear light units. This causes current flow to be restricted, ' +
            'which in turn causes lights to go dim whenever other lights in ' +
            'the same fixture go on.\n' +
            'This can be simulated by setting a reduction value and ticking ' +
            'the boxes corresponding to the light function that should cause ' +
            'dimming.\n' +
            'Example: the tail light should be dimmed whenever the left ' +
            'indicator goes on. Configure an LED to be a tail light by ' +
            'setting a value in the Tail field. Click on the spanner of that ' +
            'LED and enter 20% reduction, and tick the checkbox in the ' +
            'left indicator column. The tail light LED will now dim by 20% ' +
            'whenever the left indicator light is on.');

        set_tooltip('help_led_initializing',
            'Define which LEDs light up after power-on of the light ' +
            'controller, when the light controller deterines center position ' +
            'for steering and throttle channels.');

        set_tooltip('help_led_no_signal',
            'Define which LEDs light up when the light controller does not ' +
            'receive any valid servo signal.');

        set_tooltip('help_led_reversing',
            'Once installed in the car, you can reverse the direction of ' +
            'steering and throttle channels if necessary by performing ' +
            '7-clicks (refer to the user manual for details).\n' +
            ' Define which LED patterns to show during this activity.');

        set_tooltip('help_led_servo_setup',
            'When you connect a servo to the light controller output, you ' +
            'can configure center, left and right endpoints of the servo ' +
            'independently\n' +
            '(8-clicks; refer to the user manual for details).\n' +
            'Define which LED patterns to show during this setup.');

        set_tooltip('help_led_name',
            'You can give LEDs a name so that their function is easier ' +
            'to recognize.\nThese names can also be used in light programs.');
    };


    // *************************************************************************
    var init_keyhandler = function () {
        document.addEventListener('keydown', function (e) {
            var modifier = navigator.platform.match('Mac') ? e.metaKey : e.ctrlKey;

            // Ctrl-S is 'save configuration'
            if (modifier  &&  e.which === 83) {
                e.preventDefault();
                document.getElementById('save_config').click();
                return false;
            }

            // Ctrl-H is 'save firmware image'
            if (modifier  &&  e.which === 72) {
                e.preventDefault();
                document.getElementById('save_firmware').click();
                return false;
            }
        });
    };


    // *************************************************************************
    var init_editor = function () {
        var i;
        var oldEditors;

        // Remove any remains e.g. when the user was saving the Configurator
        // via the browsers Save As function.
        oldEditors = document.querySelectorAll('.CodeMirror');
        for (i = 0; i < oldEditors.length; i += 1) {
            var oldEditor = oldEditors[i];
            oldEditor.parentNode.removeChild(oldEditor);
        }

        editor = CodeMirror.fromTextArea(document.getElementById('light_programs'),
            {
                indentUnit: 4,
                tabSize: 4,
                lineNumbers: true,
                mode: 'light-program',
                autoCloseBrackets: true,
                showCursorWhenSelecting: true,
                styleActiveLine: true,
                keyMap: 'sublime',
                theme: 'monokai',
                gutters: ['CodeMirror-lint-markers'],
                lint: false,
                extraKeys: {
                    'Shift-Delete': 'deleteLine',
                    'F11': function (cm) {
                        cm.setOption('fullScreen', !cm.getOption('fullScreen'));
                    },
                    'Shift-Ctrl-A': function () {
                        document.getElementById('light_programs_assembler').click();
                    },
                    'Esc': function (cm) {
                        if (cm.listSelections().length > 1) {
                            var range = cm.listSelections()[0];
                            cm.setSelection(range.anchor, range.head, {scroll: false});
                        } else if (cm.getOption('fullScreen')) {
                            cm.setOption('fullScreen', false);
                        } else {
                            cm.execCommand('clearSearch');
                        }
                    }
                }
            });
    };


    // *************************************************************************
    var update_errors = function (errors) {
        light_program_errors = errors;
        if (light_program_errors.length > 0) {
            editor.setOption('lint', function () {
                return light_program_errors;
            });
        } else {
            editor.setOption('lint', false);
        }
    };


    // *************************************************************************
    var update_editor = function () {
        editor.setValue(document.getElementById('light_programs').value);
    };


    // *************************************************************************
    var refresh_editor = function () {
        editor.refresh();
    };


    // *************************************************************************
    var get_editor_content = function () {
        return editor.getValue();
    };


    // *************************************************************************
    var init = function () {
        init_led_tables();
        init_led_editing();
        init_tooltips();
        init_editor();
        init_keyhandler();
        update_tab_visibility();
    };


    // *************************************************************************
    return {
        init: init,
        update_editor: update_editor,
        update_errors: update_errors,
        refresh_editor: refresh_editor,
        get_editor_content: get_editor_content
    };
}());
