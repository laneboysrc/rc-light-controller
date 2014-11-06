"use strict";

var ui = (function () {
    var editor;

    // *************************************************************************
    var led_feature_click_handler = function (e) {
        var features_row = document.getElementById(e.target.name);
        var visible = Boolean(features_row.style.display != "none");

        var features = document.getElementsByClassName("led_features");
        for (var i = 0; i < features.length; i++) {
            features[i].style.display = "none";
        }

        if (!visible) {
            features_row.style.display = "";
        }
    }


    // *************************************************************************
    var led_config_click_handler = function (e) {
        var item = e.target;

        if (item.nodeName != "TD") {
            return;
        }

        var cell = document.getElementById(item.id);

        var originalValue = parseInt(cell.textContent, 10);
        if (isNaN(originalValue) || !isFinite(originalValue)) {
            originalValue = 0;
        }

        var input = document.createElement("input");
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
            var input = e.target;
            var cell = input.parentNode;

            var newValue = parseInt(input.value);
            if (isNaN(newValue)  ||  !isFinite(newValue) ||
                    newValue < input.min  ||  newValue > input.max) {
                newValue = originalValue;
            }

            while (cell.firstChild) {
                cell.removeChild(cell.firstChild);
            }

            if (newValue != 0) {
                var textNode = document.createTextNode(newValue);
                cell.appendChild(textNode);
            }
        };

        function key_handler(e) {
            var input = e.target;

            switch (e.keyCode) {
                case 27: // ESC
                    input.value = originalValue;
                    input.blur();
                    break;

                case 13: // ENTER
                    input.blur();
                    break;

                default:
                    break;
            }
        };

        input.addEventListener("blur", blur_handler, false);
        input.addEventListener("keydown", key_handler, false);
    };


    // *************************************************************************
    var init_led_editing = function () {

        function init_led_section(section, prefix) {
            var led_section = document.getElementById(section);
            var led_rows = led_section.getElementsByClassName("led_config");

            for (var led = 0; led < led_rows.length; led++) {
                var fields = led_rows[led].getElementsByTagName("td");
                for (var i = 0; i < fields.length; i++) {
                    fields[i].id = "" + prefix + led + "field" + i;
                    fields[i].addEventListener(
                        "click", led_config_click_handler, true);
                }

                var spanner = led_rows[led].getElementsByClassName("spanner")[0];
                spanner.name = "" + prefix + led + "features"
                spanner.addEventListener(
                    "click", led_feature_click_handler, true);
            }
        }

        init_led_section("leds_master", "master");
        init_led_section("leds_slave", "slave");
    };


    // *************************************************************************
    var init_led_features = function () {

        function init_led_feature(section, prefix) {
            var elements;
            var led_section = document.getElementById(section);
            var led_rows = led_section.getElementsByClassName("led_features");

            for (var led = 0; led < led_rows.length; led++) {
                led_rows[led].id = "" + prefix + led + "features";
                led_rows[led].style.display = "none";

                elements = led_rows[led].getElementsByClassName("incandescent");
                elements[0].id = "" + prefix + led + "incandescent";

                elements = led_rows[led].getElementsByClassName("weak_ground");
                elements[0].id = "" + prefix + led + "weak_ground";

                elements = led_rows[led].getElementsByClassName("checkbox");
                for (var i = 0; i < elements.length; i++) {
                    elements[i].id = "" + prefix + led + "checkbox" + i;
                }
            }
        }

        init_led_feature("leds_master", "master");
        init_led_feature("leds_slave", "slave");
    };


    // *************************************************************************
    var init_led_tables = function () {

        function init_led_table(section, prefix) {
            var led_section = document.getElementById(section);
            var led_tbody = led_section.getElementsByTagName("tbody")[0];
            var template_function = tmpl("led_row_template");

            for (var i = 0; i < 16; i++) {
                var e = document.createElement("table");
                e.innerHTML = template_function({
                    "even_odd": (i % 2) ? "odd" : "even",
                    "led_number": i
                });
                var body = e.getElementsByTagName("tbody")[0];

                while (body.childNodes[0]) {
                    led_tbody.appendChild(body.childNodes[0]);
                }
            }
        }

        init_led_table("leds_master", "master");
        init_led_table("leds_slave", "slave");
    };


    // *************************************************************************
    var init_tooltips = function () {
        function set_tooltip(element_name, help_text) {
            var element = document.getElementsByName(element_name);
            for (var i = 0; i < element.length; i++) {
                element[i].title = help_text;
            }
        }

        set_tooltip("help_led_light_switch",
            "The virtual light switch has 9 positions. " +
            "A single click on AUX/CH3 increments the light switch position, " +
            "a double-click decrements the light switch position.\n" +
            "If you want an LED to be on at multiple positions simply fill " +
            "in values at all the desired light switch positions where the " +
            "LED should turn on.\n" +
            "Example: For parking lights (aka positioning lights), set a " +
            "value for light switch position 1. For main beam (aka. head " +
            "lamps) set a value at both light switch position 1 and 2. For " +
            "high beam set a value at light switch positions 1, 2 and 3. " +
            "This way the light behaviour corresponds to a real car in most " +
            "countries.");

        set_tooltip("help_led_always_on",
            "The LED will be always on at the given " +
            "brightness, regardless of the car state.");

        set_tooltip("help_led_tail",
            "Tail light (aka. rear position light). This " +
            "function applies when the light position switch is at any state " +
            "other than 0.");

        set_tooltip("help_led_brake",
            "Brake light function.\nHint: to do a " +
            "combined tail and brake light, set the \"tail\" entry to a low " +
            "value (e.g. 33%), and the \"brake\" entry to a high value " +
            "(e.g. 100%).");

        set_tooltip("help_led_reverse",
            "Reversing light function; turns on when the " +
            "car is driving backwards.\nNote: see main configuration above " +
            "to configure your ESC type!");

        set_tooltip("help_led_indicator",
            "Indicator, aka. turn signals.\nAlso applies " +
            "to the hazard light function.");

        set_tooltip("help_click_to_change", "Click to change");

        set_tooltip("help_toggle_features",
            "Click to show/hide advanced LED features");

        set_tooltip("help_incandescent",
            "LEDs emit light instantly, while incandescent lamps gradually " +
            "increase in brightness over a short amount of time.\n" +
            "By setting a value greater than 0 in this field, the LED only " +
            "changes brightness up to the given percentage in a period of " +
            "20 ms, and therfore fading in and out rather than instantly " +
            "turning on or off.\n" +
            "The smaller the value, the slower the LED fades. A value of 15% " +
            "results in a fade time of approx 130 ms from off to on, " +
            "which is a realistic simulation for incandescent bulbs in " +
            "vintage cars.\n" +
            "Note that slow fading causes visible steps at lower brightness " +
            "values due to the limited control range of the LED driver.");

        set_tooltip("help_weak_ground",
            "In old cars the ground connection often corrodes, especially on " +
            "rear light units. This causes current flow to be restricted, " +
            "which in turn causes lights to go dim whenever other lights in " +
            "the same fixture go on.\n" +
            "This can be simulated by setting a reduction value and ticking " +
            "the boxes corresponding to the light function that should cause " +
            "dimming.\n" +
            "Example: the tail light should be dimmed whenever the left " +
            "indicator goes on. Configure an LED to be a tail light by " +
            "setting a value in the Tail field. Click on the spanner of that " +
            "LED and enter 20% reduction, and tick the checkbox in the " +
            "left indicator column. The tail light LED will now dim by 20% " +
            "whenever the left indicator light is on.");
    };


    // *************************************************************************
    var init_editor = function () {
        editor = CodeMirror.fromTextArea(
            document.getElementById("light_programs"), {
            indentUnit: 4,
            tabSize: 4,
            lineNumbers: true,
            mode: "light-program",
            autoCloseBrackets: true,
            showCursorWhenSelecting: true,
            styleActiveLine: true,
            keyMap: "sublime",
            theme: "monokai",
            extraKeys: {
                "F11": function(cm) {
                    cm.setOption("fullScreen", !cm.getOption("fullScreen"));
                },
                "Shift-Delete": "deleteLine",
                "Esc": function(cm) {
                    if (cm.listSelections().length > 1) {
                        var range = cm.listSelections()[0];
                        cm.setSelection(range.anchor, range.head, {scroll: false});
                    }
                    else if (cm.getOption("fullScreen")) {
                        cm.setOption("fullScreen", false);
                    }
                    else {
                        cm.execCommand("clearSearch");
                    }
                }
            }
        });
    }


    // *************************************************************************
    var update_editor = function () {
        editor.setValue(document.getElementById("light_programs").value);
    }


    // *************************************************************************
    var get_editor_content = function () {
        return editor.getValue();
    }


    // *************************************************************************
    var init = function () {
        init_led_tables();
        init_led_editing();
        init_led_features();
        init_tooltips();
        init_editor();
    };


    // *************************************************************************
    return {
        init: init,
        update_editor: update_editor,
        get_editor_content: get_editor_content
    };
})();
