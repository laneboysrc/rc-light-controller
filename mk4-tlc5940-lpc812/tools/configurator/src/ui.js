'use strict';
/*jslint browser: true, vars: true */
/*global CodeMirror, tmpl */

var ui = (function () {
    var editor;
    var light_program_errors = [];

    // *************************************************************************
    var led_feature_click_handler = function (e) {
        var features_row = document.getElementById(e.target.name);
        var visible = Boolean(features_row.style.display !== 'none');
        var i;

        var features = document.getElementsByClassName('led_features');
        for (i = 0; i < features.length; i += 1) {
            features[i].style.display = 'none';
        }

        if (!visible) {
            features_row.style.display = '';
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

        cell.appendChild(input);
        input.focus();

        function blur_handler(e) {
            var blur_cell = e.target.parentNode;

            var newValue = parseInt(input.value, 10);
            if (isNaN(newValue)  ||  !isFinite(newValue) ||
                    newValue < input.min  ||  newValue > input.max) {
                newValue = originalValue;
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
            var led;
            var fields;
            var i;
            var spanner;

            for (led = 0; led < led_rows.length; led += 1) {
                fields = led_rows[led].getElementsByTagName('td');
                for (i = 0; i < fields.length; i += 1) {
                    fields[i].id = prefix + led + 'field' + i;
                    fields[i].addEventListener('click',
                        led_config_click_handler, true);
                }

                spanner = led_rows[led].getElementsByClassName('spanner')[0];
                spanner.name = prefix + led + 'features';
                spanner.addEventListener('click',
                    led_feature_click_handler, true);
            }
        }

        init_led_section('leds_master', 'master');
        init_led_section('leds_slave', 'slave');
    };


    // *************************************************************************
    var init_led_features = function () {

        function init_led_feature(section, prefix) {
            var elements;
            var led_section = document.getElementById(section);
            var led_rows = led_section.getElementsByClassName('led_features');
            var led;
            var i;

            for (led = 0; led < led_rows.length; led += 1) {
                led_rows[led].id = prefix + led + 'features';
                led_rows[led].style.display = 'none';

                elements = led_rows[led].getElementsByClassName('incandescent');
                elements[0].id = prefix + led + 'incandescent';

                elements = led_rows[led].getElementsByClassName('weak_ground');
                elements[0].id = prefix + led + 'weak_ground';

                elements = led_rows[led].getElementsByClassName('checkbox');
                for (i = 0; i < elements.length; i += 1) {
                    elements[i].id = prefix + led + 'checkbox' + i;
                }
            }
        }

        init_led_feature('leds_master', 'master');
        init_led_feature('leds_slave', 'slave');
    };


    // *************************************************************************
    var init_led_tables = function () {

        function init_led_table(section, led_offset) {
            var led_section = document.getElementById(section);
            var led_tbody = led_section.getElementsByTagName('tbody')[0];
            var template_function = tmpl('led_row_template');
            var i;
            var helper;

            var html = '<table>';
            for (i = 0; i < 16; i += 1) {
                html += template_function({
                    'even_odd': (i % 2) ? 'odd' : 'even',
                    'led_number': i + led_offset
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

        init_led_table('leds_master', 0);
        init_led_table('leds_slave', 16);
    };


    // *************************************************************************
    var init_tooltips = function () {
        function set_tooltip(element_name, help_text) {
            var i;
            var element = document.getElementsByName(element_name);

            for (i = 0; i < element.length; i += 1) {
                element[i].title = help_text;
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
            'Click to show/hide advanced LED features');

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
        init_led_features();
        init_tooltips();
        init_editor();
        init_keyhandler();
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
