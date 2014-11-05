var symbols = (function () {
    "use strict";

    var symbol_table = [];
    var forward_declaration_table = [];
    var next_variable_index = 0;
    var leds_used = 0;

    var undeclared_symbol = {"token": "UNDECLARED_SYMBOL", "opcode": 0};

    var parser;

    var run_condition_tokens = {
        "always": {"token": "RUN_CONDITION_ALWAYS", "opcode": 0x80000000},

        "light-switch-position-0": {"token": "RUN_CONDITION", "opcode": (1 << 0)},
        "light-switch-position-1": {"token": "RUN_CONDITION", "opcode": (1 << 1)},
        "light-switch-position-2": {"token": "RUN_CONDITION", "opcode": (1 << 2)},
        "light-switch-position-3": {"token": "RUN_CONDITION", "opcode": (1 << 3)},
        "light-switch-position-4": {"token": "RUN_CONDITION", "opcode": (1 << 4)},
        "light-switch-position-5": {"token": "RUN_CONDITION", "opcode": (1 << 5)},
        "light-switch-position-6": {"token": "RUN_CONDITION", "opcode": (1 << 6)},
        "light-switch-position-7": {"token": "RUN_CONDITION", "opcode": (1 << 7)},
        "light-switch-position-8": {"token": "RUN_CONDITION", "opcode": (1 << 8)},

        "neutral": {"token": "RUN_CONDITION", "opcode": (1 << 9)},
        "forward": {"token": "RUN_CONDITION", "opcode": (1 << 10)},
        "reversing": {"token": "RUN_CONDITION", "opcode": (1 << 11)},
        "braking": {"token": "RUN_CONDITION", "opcode": (1 << 12)},

        "indicator-left": {"token": "RUN_CONDITION", "opcode": (1 << 13)},
        "indicator-right": {"token": "RUN_CONDITION", "opcode": (1 << 14)},
        "hazard": {"token": "RUN_CONDITION", "opcode": (1 << 15)},
        "blink-flag": {"token": "RUN_CONDITION", "opcode": (1 << 16)},
        "blink-left": {"token": "RUN_CONDITION", "opcode": (1 << 17)},
        "blink-right": {"token": "RUN_CONDITION", "opcode": (1 << 18)},

        "winch-disabled": {"token": "RUN_CONDITION", "opcode": (1 << 19)},
        "winch-idle": {"token": "RUN_CONDITION", "opcode": (1 << 20)},
        "winch-in": {"token": "RUN_CONDITION", "opcode": (1 << 21)},
        "winch-out": {"token": "RUN_CONDITION", "opcode": (1 << 22)},

        "no-signal": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 0)},
        "initializing": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 1)},
        "servo-output-setup-centre": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 2)},
        "servo-output-setup-left": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 3)},
        "servo-output-setup-right": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 4)},
        "reversing-setup-steering": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 5)},
        "reversing-setup-throttle": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 6)},
        "gear-changed": {"token": "PRIORITY_RUN_CONDITION", "opcode": (1 << 7)}
    };

    var car_state_tokens = {
        "light-switch-position-0": {"token": "CAR_STATE", "opcode": (1 << 0)},
        "light-switch-position-1": {"token": "CAR_STATE", "opcode": (1 << 1)},
        "light-switch-position-2": {"token": "CAR_STATE", "opcode": (1 << 2)},
        "light-switch-position-3": {"token": "CAR_STATE", "opcode": (1 << 3)},
        "light-switch-position-4": {"token": "CAR_STATE", "opcode": (1 << 4)},
        "light-switch-position-5": {"token": "CAR_STATE", "opcode": (1 << 5)},
        "light-switch-position-6": {"token": "CAR_STATE", "opcode": (1 << 6)},
        "light-switch-position-7": {"token": "CAR_STATE", "opcode": (1 << 7)},
        "light-switch-position-8": {"token": "CAR_STATE", "opcode": (1 << 8)},

        "neutral": {"token": "CAR_STATE", "opcode": (1 << 9)},
        "forward": {"token": "CAR_STATE", "opcode": (1 << 10)},
        "reversing": {"token": "CAR_STATE", "opcode": (1 << 11)},
        "braking": {"token": "CAR_STATE", "opcode": (1 << 12)},

        "indicator-left": {"token": "CAR_STATE", "opcode": (1 << 13)},
        "indicator-right": {"token": "CAR_STATE", "opcode": (1 << 14)},
        "hazard": {"token": "CAR_STATE", "opcode": (1 << 15)},
        "blink-flag": {"token": "CAR_STATE", "opcode": (1 << 16)},
        "blink-left": {"token": "CAR_STATE", "opcode": (1 << 17)},
        "blink-right": {"token": "CAR_STATE", "opcode": (1 << 18)},

        "winch-disabled": {"token": "CAR_STATE", "opcode": (1 << 19)},
        "winch-idle": {"token": "CAR_STATE", "opcode": (1 << 20)},
        "winch-in": {"token": "CAR_STATE", "opcode": (1 << 21)},
        "winch-out": {"token": "CAR_STATE", "opcode": (1 << 22)},

        "servo-output-setup-centre": {"token": "CAR_STATE", "opcode": (1 << 24)},
        "servo-output-setup-left": {"token": "CAR_STATE", "opcode": (1 << 25)},
        "servo-output-setup-right": {"token": "CAR_STATE", "opcode": (1 << 26)},
        "reversing-setup-steering": {"token": "CAR_STATE", "opcode": (1 << 27)},
        "reversing-setup-throttle": {"token": "CAR_STATE", "opcode": (1 << 28)},
    };

    var reserved_words = {
        "goto": {"token": "GOTO", "opcode": 0x01000000},
        "var": {"token": "VAR"},
        "led": {"token": "LED"},
        "leds": {"token": "LEDS"},
        "fade": {"token": "FADE", "opcode": 0x04000000},
        "stepsize": {"token": "STEPSIZE"},
        "sleep": {"token": "SLEEP", "opcode": 0x06000000},
        "skip": {"token": "SKIP"},
        "if": {"token": "IF"},
        "any": {"token": "ANY", "opcode": 0x60000000},
        "all": {"token": "ALL", "opcode": 0x80000000},
        "none": {"token": "NONE", "opcode": 0xa0000000},
        "not": {"token": "NOT", "opcode": 0xa0000000},
        "is": {"token": "IS", "opcode": 0x60000000},
        "run": {"token": "RUN"},
        "when": {"token": "WHEN"},
        "or": {"token": "OR"},
        "master": {"token": "MASTER"},
        "slave": {"token": "SLAVE"},
        "global": {"token": "GLOBAL"},
        "random": {"token": "RANDOM"},
        "steering": {"token": "STEERING"},
        "throttle": {"token": "THROTTLE"},
        "gear": {"token": "GEAR"},
        "abs": {"token": "ABS", "opcode": 0x40000000},
        "end": {"token": "END", "opcode": 0xfe000000},

        "=": {"token": "=", "opcode": 0x10000000},
        ">": {"token": "GT", "opcode": 0x2c000000},
        "<": {"token": "LT", "opcode": 0x34000000},
        "+=": {"token": "ADD_ASSIGN", "opcode": 0x12000000},
        "-=": {"token": "SUB_ASSIGN", "opcode": 0x14000000},
        "*=": {"token": "MUL_ASSIGN", "opcode": 0x16000000},
        "/=": {"token": "DIV_ASSIGN", "opcode": 0x18000000},
        "&=": {"token": "AND_ASSIGN", "opcode": 0x1a000000},
        "|=": {"token": "OR_ASSIGN", "opcode": 0x1c000000},
        "^=": {"token": "XOR_ASSIGN", "opcode": 0x1e000000},
        "==": {"token": "EQ", "opcode": 0x20000000},
        "!=": {"token": "NE", "opcode": 0x24000000},
        ">=": {"token": "GE", "opcode": 0x28000000},
        "<=": {"token": "LE", "opcode": 0x30000000},
    };

    var MODULE = "SYMBOL";


    // *************************************************************************
    var reset = function () {
        symbol_table = [];
        forward_declaration_table = [];
        next_variable_index = 0;
        leds_used = 0;

        add_symbol("clicks", "GLOBAL_VARIABLE", next_variable_index++);
    }


    // *************************************************************************
    var hex = function (number) {
        var s = number.toString(16).toUpperCase();
        while (s.length < 8) {
            s = "0" + s;
        }

        return "0x" + s;
    };


    // ****************************************************************************
    var dump_symbol_table = function () {
        var msg = "\n";

        msg += "Symbol table:\n";

        for (var i = 0; i < symbol_table.length; i++) {
            var s = symbol_table[i];
            msg += "name='" + s.name + "', token=" + s.token + " opcode=" + s.opcode + "\n";
        }

        msg += "\n";
        msg += "Forward declarations to resolve:\n";
        if (forward_declaration_table.length === 0) {
            msg += "(none)\n";
        }
        else {
            for (var i = 0; i < forward_declaration_table.length; i++) {
                var f = forward_declaration_table[i];
                msg += "label='" + f.symbol.name + "' pc=" + f.pc + " opcode=" + f.symbol.opcode + "\n";
            }
        }
        msg += "\n";
        parser.yy.logger.log(MODULE, "INFO", msg);
    }


    // *************************************************************************
    var add_forward_declaration = function (symbol, decleration_pc, location) {
        var forward_decleration = {
            "symbol": symbol,
            "pc": decleration_pc,
            "location": location
        };

        forward_declaration_table.push(forward_decleration);
    };


    // *************************************************************************
    var get_forward_declerations = function () {
        return forward_declaration_table;
    };


    // *************************************************************************
    var remove_local_symbols = function () {
        leds_used = 0;

        forward_declaration_table = [];

        for (var i = symbol_table.length - 1; i >= 0; i--) {
            if (symbol_table[i].token !== "GLOBAL_VARIABLE") {
                symbol_table.splice(i, 1);
            }
        }
    }


    // *************************************************************************
    var set_symbol = function (name, token, opcode, location) {
        for (var i = 0; i < symbol_table.length; i++) {
            var s = symbol_table[i];
            if (s.name === name) {
                if (s.opcode !== -1) {
                    throw new Error("[SYMBOLS] Redefinition of symbol " + name);
                    //yyerror("Redefinition of symbol " + name);
                }
                symbol_table[i].token = token;
                symbol_table[i].opcode = opcode;

                parser.yy.logger.log(MODULE, "INFO", "Set '" + name +"' as token=" + token + " opcode=" + opcode);
            }
        }

        parser.yy.logger.log(MODULE, "ERROR", "Could not find '" + name +"'");
    };


    // *************************************************************************
    var get_symbol = function (name, parse_state, location) {
        if (parse_state === "EXPECTING_RUN_CONDITION") {
            return run_condition_tokens[name] || undeclared_symbol;
        }

        if (parse_state === "EXPECTING_CAR_STATE") {
            return car_state_tokens[name] || undeclared_symbol;
        }

        // See if we are dealing with a LOCAL symbol
        for (var i = 0; i < symbol_table.length; i++) {
            var s = symbol_table[i];

            if (s.token === "GLOBAL_VARIABLE") {
                continue;
            }

            if (s.name === name) {
                if (s.token === "LABEL") {
                    if (s.opcode === -1) {
                        add_forward_declaration(s, parser.yy.emitter.pc(), location);
                        parser.yy.logger.log(MODULE, "INFO", "Using forward declared label " + name);
                    }
                }
                return s;
            }
        }

        // See if we are dealing with a GLOBAL symbol
        for (var i = 0; i < symbol_table.length; i++) {
            var s = symbol_table[i];

            if (s.token !== "GLOBAL_VARIABLE") {
                continue;
            }
            if (s.name === name) {
                return s;
            }
        }

        parser.yy.logger.log(MODULE, "INFO", "Undeclared symbol " + name);

        return undeclared_symbol;
    };


    // *************************************************************************
    var add_symbol = function (name, token, opcode, location) {
        var new_symbol = {
            "name": name,
            "token": token,
            "opcode": opcode
        };

        if (token === "GLOBAL_VARIABLE"  ||  token === "VARIABLE") {
            if (opcode === -1) {
                new_symbol.opcode =  next_variable_index++;
            }
        }

        if (token == "LED_ID") {
            if (opcode < 0  ||  opcode > 31) {
                throw new Error("LED index out of range (must be 0..15)");
                //yyerror(location, "LED index out of range (must be 0..15)");
            }
            else {
                // Add LED to bit-field of leds_used
                leds_used += Math.pow(2, opcode);
            }
        }

        symbol_table.push(new_symbol);

        if (token == "LABEL"  &&  opcode == -1) {
            add_forward_declaration(new_symbol, parser.yy.emitter.pc(), location);
            parser.yy.logger.log(MODULE, "INFO", "Forward declaration of label " + name);
        }
    };


    // *************************************************************************
    var get_reserved_word = function (name) {
        return reserved_words[name] ||
            parser.yy.logger.log(MODULE, "FATAL", "ASSERT: reserved word " + name + " is not in the table");
    };


    // *************************************************************************
    var get_leds_used = function () {
        return leds_used;
    };


    // *************************************************************************
    var set_parser = function (e) {
        parser = e;
    }


    // *************************************************************************
    reset();

    return {
        set_parser: set_parser,
        add_symbol: add_symbol,
        get_symbol: get_symbol,
        set_symbol: set_symbol,
        get_reserved_word: get_reserved_word,
        get_leds_used: get_leds_used,
        get_forward_declerations: get_forward_declerations,
        remove_local_symbols: remove_local_symbols,
        dump_symbol_table: dump_symbol_table,
        reset: reset
    };

})();

// node.js exports; hide from browser where exports is undefined and use strict
// would trigger.
if (typeof exports !== 'undefined') {
    exports.symbols = symbols;
}