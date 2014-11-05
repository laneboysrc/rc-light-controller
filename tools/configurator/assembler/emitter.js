var symbols = require('./symbols').symbols;

var emitter = (function () {
    "use strict";

    var MAX_LIGHT_PROGRAMS = 25;
    var MAX_LIGHT_PROGRAM_VARIABLES = 100;

    var NUMBER_OF_LEDS = 32;

    // Taken from globals.h of the light controller firmware:
    var FIRST_SKIP_IF_OPCODE  = 0x20;
    var LAST_SKIP_IF_OPCODE   = 0x37;
    var OPCODE_SKIP_IF_ANY    = 0x60;    // 011 + 29 bits run_state!
    var OPCODE_SKIP_IF_ALL    = 0x80;    // 100 + 29 bits run_state!
    var OPCODE_SKIP_IF_NONE   = 0xA0;    // 101 + 29 bits run_state!

    var LEDS_USED_OFFSET = 2;
    var FIRST_INSTRUCTION_OFFSET = 3;

    var number_of_programs = 0;
    var start_offset = new Array(MAX_LIGHT_PROGRAMS);
    var instruction_list = [];
    var pc = 0;
    var led_list = [];

    // var symbols;

    // *************************************************************************
    var hex = function (number) {
        var s = number.toString(16).toUpperCase();
        while (s.length < 8) {
            s = "0" + s;
        }

        return "0x" + s;
    }


    // *************************************************************************
    var is_skip_if = function (instruction) {
        var opcode = instruction >> 24;

        if (opcode >= FIRST_SKIP_IF_OPCODE  &&  opcode <= LAST_SKIP_IF_OPCODE) {
            return true;
        }

        // The skip if any/all/none opcode have only the top-most 3 bits distinct
        // so that we can use 29 bits for 'car state'
        if (((opcode & 0xe0) == OPCODE_SKIP_IF_ANY)  ||
            ((opcode & 0xe0) == OPCODE_SKIP_IF_ALL)  ||
            ((opcode & 0xe0) == OPCODE_SKIP_IF_NONE)) {
            return true;
        }

        return false;
    }


    // *************************************************************************
    var resolve_forward_declarations = function () {
        // FIXME: why is this necessary?
        var symbols = require('./symbols').symbols;

        var forward_declarations = symbols.get_forward_declerations();

        for (var i = 0; i < forward_declarations.length; i++) {
            var f = forward_declarations[i];

            if (f.index < 0) {
                console.log("[EMIT]    Label '" + f.label + "' used but not defined.");
                // yyerror
            }
            else if (f.index == f.pc) {
                // Skip the declaration of the label
                continue;
            }
            else {
                var offset = start_offset[number_of_programs];
                offset += FIRST_INSTRUCTION_OFFSET;
                offset += f.pc;

                instruction_list[offset] =
                    (instruction_list[offset] & 0xff000000) |
                        (f.index & 0x00ffffff);
            }
        }
    };


    // *************************************************************************
    var add_led_to_list = function (led_index) {
        if (led_index < 0) {
            // "All used LEDs" requested
            var leds_used = symbols.get_leds_used();
            led_list = [];

            console.log("[EMIT]    Adding all LEDs: " + hex(leds_used));

            for (var i = 0; i < NUMBER_OF_LEDS; i++) {
                if (leds_used & (1 << i)) {
                    led_list.push(i);
                }
            }
            return;
        }

        // Discard duplicates
        for (var i = 0; i < led_list.length; i++) {
            if (led_list[i] === led_index) {
                console.log("[EMIT]    WARNING: Duplicate LED " + led_index +
                    " in list");
                return;
            }
        }

        if (led_list.length < NUMBER_OF_LEDS) {
            led_list.push(led_index);
        }
        else {
            console.log("[EMIT]    ERROR: led_list is full");
            // yyerror
        }
    }


    // ****************************************************************************
    var emit_led_instruction = function (instruction, location) {
        var start;
        var stop;

        console.log("[EMIT]    LED instruction: " + hex(instruction) +
            " (" + led_list.length + " leds)");

        if (led_list.length === 0) {
            console.log("[EMIT]    emit_led_instruction(): led_list.length is 0");
            //yyerror(location, "emit_led_instruction(): led_list.length is 0");
            return;
        }

        if (led_list.length > 1  &&  pc > 0  &&
                is_skip_if(instruction_list[-1])) {
            console.log("[EMIT]    commands using multiple LEDs can not follow 'skip if'");
            // yyerror(location, "commands using multiple LEDs can not follow 'skip if'");
        }

        // Step 1: sort the LEDs by their index.
        led_list.sort(function(a, b) { return a - b; });

        // Step 2: Iterate through all items. If discontinuity is found emit a
        // single LED statement.

        start = stop = led_list[0];
        for (i = 1; i < led_list.length; i++) {
            if (led_list[i] != (stop + 1)) {
                emit(instruction | (stop << 16) | (start << 8));
                start = stop = led_list[i];
            }
            else {
                ++stop;
            }
        }
        emit(instruction | (stop << 16) | (start << 8));

        led_list = [];
    }


    // *************************************************************************
    var emit_run_condition = function (priority_run_condition, run_condition) {
        console.log("[EMIT]    PRIORITY code: " + hex(priority_run_condition));
        console.log("[EMIT]    RUN code: " + hex(run_condition));

        instruction_list.push(priority_run_condition);
        instruction_list.push(run_condition);
        instruction_list.push(0);   // Placeholder for "leds used"
    }


    // *************************************************************************
    var emit_end_of_program = function () {
        console.log("[EMIT]    emit_end_of_program()");

        if (pc > 0  &&  is_skip_if(instruction_list[-1])) {
            //yyerror(NULL, "Last operation in a program can not be 'skip if'.");
            console.log("[EMIT]    Last operation in a program can not be 'skip if'.");
        }

        // Add end-of-program instruction
        instruction_list.push(0xfe000000);


        // FIXME: why is this necessary?
        var symbols = require('./symbols').symbols;


        symbols.dump_symbol_table();

        // Fill in LEDS_USED word!
        instruction_list[start_offset[number_of_programs] + LEDS_USED_OFFSET] =
            symbols.get_leds_used();

        resolve_forward_declarations();

        // Prepare for the next program
        symbols.remove_local_symbols();
        ++number_of_programs;
        pc = 0;
        start_offset[number_of_programs] = instruction_list.length - 1;
    }


    // *************************************************************************
    var emit = function (instruction) {
        console.log("[EMIT]    INSTRUCTION: " + hex(instruction));

        instruction_list.push(instruction);
        ++pc;
    }


    // *************************************************************************
    var get_pc = function () {
        return pc;
    };


    // *************************************************************************
    var output_programs = function () {
        // Add the "END OF PROGRAMS" instruction to mark the end
        instruction_list.push(0xff000000);

        return {
            "number_of_programs": number_of_programs,
            "start_offset": start_offset,
            "instructions": instruction_list
        };
    }


    // *************************************************************************
    for (var i = 0; i < start_offset.length; i++) {
        start_offset[i] = 0;
    }

    return {
        emit: emit,
        emit_run_condition: emit_run_condition,
        emit_led_instruction: emit_led_instruction,
        emit_end_of_program: emit_end_of_program,
        add_led_to_list: add_led_to_list,
        pc: get_pc,
        output_programs: output_programs
    };

})();


// node.js exports; hide from browser where exports is undefined and use strict
// would trigger.
if (typeof exports !== 'undefined') {
    exports.emitter = emitter;
}