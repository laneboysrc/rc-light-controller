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
    var errors = [];
    var last_location;

    var parser;

    var MODULE = "EMIT";

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
        var forward_declarations = parser.yy.symbols.get_forward_declerations();

        for (var i = 0; i < forward_declarations.length; i++) {
            var f = forward_declarations[i];


            if (f.symbol.opcode < 0) {
                yyerror("Label '" + f.symbol.name + "' used but not defined.", {
                    loc: f.location
                });
            }
            else if (f.symbol.opcode == f.pc) {
                // Skip the declaration of the label
                continue;
            }
            else {
                var offset = start_offset[number_of_programs];
                offset += FIRST_INSTRUCTION_OFFSET;
                offset += f.pc;

                instruction_list[offset] =
                    (instruction_list[offset] & 0xff000000) |
                        (f.symbol.opcode & 0x00ffffff);
            }
        }
    };


    // *************************************************************************
    var add_led_to_list = function (led_index) {
        if (led_index < 0) {
            // "All used LEDs" requested
            var leds_used = parser.yy.symbols.get_leds_used();
            led_list = [];

            parser.yy.logger.log(MODULE, "INFO", "Adding all LEDs: " + hex(leds_used));

            for (var i = 0; i < NUMBER_OF_LEDS; i++) {
                if (leds_used & Math.pow(2, i)) {
                    led_list.push(i);
                }
            }
            return;
        }

        // Discard duplicates
        for (var i = 0; i < led_list.length; i++) {
            if (led_list[i] === led_index) {
                parser.yy.logger.log(MODULE, "WARNING", "Duplicate LED " + led_index +
                    " in list");
                return;
            }
        }

        if (led_list.length < NUMBER_OF_LEDS) {
            led_list.push(led_index);
        }
        else {
            throw new Error("led_list is full");
        }
    }


    // ****************************************************************************
    var emit_led_instruction = function (instruction, location) {
        var start;
        var stop;

        parser.yy.logger.log(MODULE, "INFO", "LED instruction: " + hex(instruction) +
            " (" + led_list.length + " leds)");

        if (led_list.length === 0) {
            throw new Error("Internal parser error: led_list.length is 0");
            return;
        }

        if (led_list.length > 1  &&  pc > 0  &&
                is_skip_if(instruction_list[instruction_list.length - 1])) {
            yyerror("Commands using multiple LEDs can not follow 'skip if'", {
                loc: location
            });
        }

        // Step 1: sort the LEDs by their index.
        led_list.sort(function(a, b) { return a - b; });

        // Step 2: Iterate through all items. If discontinuity is found emit a
        // single LED statement.

        start = stop = led_list[0];
        for (var i = 1; i < led_list.length; i++) {
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
        parser.yy.logger.log(MODULE, "INFO", "PRIORITY code: " + hex(priority_run_condition));
        parser.yy.logger.log(MODULE, "INFO", "RUN code: " + hex(run_condition));

        instruction_list.push(priority_run_condition);
        instruction_list.push(run_condition);
        instruction_list.push(0);   // Placeholder for "leds used"
    }


    // *************************************************************************
    var emit_end_of_program = function () {
        parser.yy.logger.log(MODULE, "INFO", "emit_end_of_program()");

        if (pc > 0  &&
            is_skip_if(instruction_list[instruction_list.length - 1])) {
            yyerror("Last operation in a program can not be 'skip if'.", {
                loc: last_location
            });
        }

        // Add end-of-program instruction
        instruction_list.push(0xfe000000);

        parser.yy.symbols.dump_symbol_table();

        // Fill in LEDS_USED word!
        instruction_list[start_offset[number_of_programs] + LEDS_USED_OFFSET] =
            parser.yy.symbols.get_leds_used();

        resolve_forward_declarations();

        // Prepare for the next program
        parser.yy.symbols.remove_local_symbols();
        ++number_of_programs;
        pc = 0;
        start_offset[number_of_programs] = instruction_list.length;
    }


    // *************************************************************************
    var emit = function (instruction, location) {
        parser.yy.logger.log(MODULE, "INFO", "INSTRUCTION: " + hex(instruction));

        last_location = location;

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

        // Print a summary
        var msg = "\n";
        msg += "Number of programs: " + number_of_programs + "\n";

        msg += "Start offset locations:\n";
        for (var i = 0; i < number_of_programs; i++) {
            msg += i + ": " + start_offset[i] + "\n";
        }
        msg += "\n";
        parser.yy.logger.log(MODULE, "INFO", msg);

        if (errors.length != 0) {
            throw new Error("Errors occured while processing the light programs:");
        }

        var light_switch_positions =
            parser.yy.symbols.get_number_of_light_switch_positions();

        var result = {
            "number_of_programs": number_of_programs,
            "start_offset": start_offset,
            "instructions": instruction_list,
            "light_switch_positions": light_switch_positions
        }

        return result;
    }


    // *************************************************************************
    var yyerror = function (str, hash) {
        parser.yy.logger.log(MODULE, "ERROR", str);

        errors.push({
            str: str,
            hash: hash
        });
    }


    // *************************************************************************
    var get_errors = function () {
        return errors;
    }


    // *************************************************************************
    var set_parser = function (p) {
        parser = p;
        parser.yy.parseError = yyerror;
        reset();
    }


    // *************************************************************************
    var reset = function () {
        number_of_programs = 0;
        for (var i = 0; i < start_offset.length; i++) {
            start_offset[i] = 0;
        }
        instruction_list = [];
        errors = [];
    }


    // *************************************************************************
    return {
        set_parser: set_parser,
        get_errors: get_errors,
        emit: emit,
        emit_run_condition: emit_run_condition,
        emit_led_instruction: emit_led_instruction,
        emit_end_of_program: emit_end_of_program,
        add_led_to_list: add_led_to_list,
        pc: get_pc,
        output_programs: output_programs,
        reset: reset
    };

})();


// node.js exports; hide from browser where exports is undefined and use strict
// would trigger.
if (typeof exports !== 'undefined') {
    exports.emitter = emitter;
}