var symbols = (function () {
    "use strict";

    var symbols = {};
    var next_variable_index = 1;

    var undeclared_symbol = {"token": "UNDECLARED_SYMBOL", "opcode": 0};

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

    var hex = function (number) {
        var s = number.toString(16).toUpperCase();
        while (s.length < 8) {
            s = "0" + s;
        }

        return "0x" + s;
    }

    var get_symbol = function (name, parse_state) {
        if (parse_state === "EXPECTING_RUN_CONDITION") {
            return run_condition_tokens[name] || undeclared_symbol;
        }

        return symbols[name] || undeclared_symbol;
    }

    var add_symbol = function (name, type, index, location) {
        console.log("add_symbol():", name, type, index, location);
        symbols[name] = {"token": type, "opcode": next_variable_index++};
    }


    var get_reserved_word = function (name) {
        return reserved_words[name] || undeclared_symbol;
    }

    return {
        add_symbol: add_symbol,
        get_symbol: get_symbol,
        get_reserved_word: get_reserved_word
    };

})();
