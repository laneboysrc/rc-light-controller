"use strict";

var ui = (function () {

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
                    fields[i].title = "Click to change";
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
        var tooltip;

        tooltip = document.getElementsByName("help_led_light_switch");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "The virtual light switch has 9 positions. " +
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
            "countries.";
        }

        tooltip = document.getElementsByName("help_led_always_on");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "The LED will be always on at the given " +
            "brightness, regardless of the car state.";
        }

        tooltip = document.getElementsByName("help_led_tail");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "Tail light (aka. rear position light). This " +
            "function applies when the light position switch is at any state " +
            "other than 0.";
        }

        tooltip = document.getElementsByName("help_led_brake");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "Brake light function.\nHint: to do a " +
            "combined tail and brake light, set the \"tail\" entry to a low " +
            "value (e.g. 33%), and the \"brake\" entry to a high value " +
            "(e.g. 100%).";
        }

        tooltip = document.getElementsByName("help_led_reverse");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "Reversing light function; turns on when the " +
            "car is driving backwards.\nNote: see main configuration above " +
            "to configure your ESC type!";
        }

        tooltip = document.getElementsByName("help_led_indicator");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "Indicator, aka. turn signals.\nAlso applies " +
            "to the hazard light function.";
        }

        tooltip = document.getElementsByName("help_incandescent");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "FIXME: incandescent simulation help text";
        }

        tooltip = document.getElementsByName("help_weak_ground");
        for (var i = 0; i < tooltip.length; i++) {
            tooltip[i].title = "FIXME: weak ground help text";
        }
    };


    // *************************************************************************
    var init = function () {
        init_led_tables();
        init_led_editing();
        init_led_features();
        init_tooltips();
    };



    // *************************************************************************
    return {
        init: init
    };
})();
