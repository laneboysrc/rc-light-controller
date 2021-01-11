/*jslint vars: true, bitwise: true, continue: true */

var symbols = (function () {
    'use strict';

    var symbol_table = [];
    var forward_declaration_table = [];
    var next_variable_index = 0;
    var leds_used = 0;
    var number_of_light_switch_positions = 0;

    var undeclared_symbol = {'token': 'UNDECLARED_SYMBOL', 'opcode': 0};

    var parser;

    var run_condition_tokens = {
        'always': {'token': 'RUN_CONDITION_ALWAYS', 'opcode': 0x80000000},

        'light-switch-position-0': {'token': 'RUN_CONDITION', 'opcode': (1 << 0)},
        'light-switch-position-1': {'token': 'RUN_CONDITION', 'opcode': (1 << 1)},
        'light-switch-position-2': {'token': 'RUN_CONDITION', 'opcode': (1 << 2)},
        'light-switch-position-3': {'token': 'RUN_CONDITION', 'opcode': (1 << 3)},
        'light-switch-position-4': {'token': 'RUN_CONDITION', 'opcode': (1 << 4)},
        'light-switch-position-5': {'token': 'RUN_CONDITION', 'opcode': (1 << 5)},
        'light-switch-position-6': {'token': 'RUN_CONDITION', 'opcode': (1 << 6)},
        'light-switch-position-7': {'token': 'RUN_CONDITION', 'opcode': (1 << 7)},
        'light-switch-position-8': {'token': 'RUN_CONDITION', 'opcode': (1 << 8)},

        'neutral': {'token': 'RUN_CONDITION', 'opcode': (1 << 9)},
        'forward': {'token': 'RUN_CONDITION', 'opcode': (1 << 10)},
        'reversing': {'token': 'RUN_CONDITION', 'opcode': (1 << 11)},
        'braking': {'token': 'RUN_CONDITION', 'opcode': (1 << 12)},

        'indicator-left': {'token': 'RUN_CONDITION', 'opcode': (1 << 13)},
        'indicator-right': {'token': 'RUN_CONDITION', 'opcode': (1 << 14)},
        'hazard': {'token': 'RUN_CONDITION', 'opcode': (1 << 15)},
        'blink-flag': {'token': 'RUN_CONDITION', 'opcode': (1 << 16)},
        'blink-left': {'token': 'RUN_CONDITION', 'opcode': (1 << 17)},
        'blink-right': {'token': 'RUN_CONDITION', 'opcode': (1 << 18)},

        'winch-disabled': {'token': 'RUN_CONDITION', 'opcode': (1 << 19)},
        'winch-idle': {'token': 'RUN_CONDITION', 'opcode': (1 << 20)},
        'winch-in': {'token': 'RUN_CONDITION', 'opcode': (1 << 21)},
        'winch-out': {'token': 'RUN_CONDITION', 'opcode': (1 << 22)},

        'program-state-0': {'token': 'RUN_CONDITION', 'opcode': (1 << 24)},
        'program-state-1': {'token': 'RUN_CONDITION', 'opcode': (1 << 25)},
        'program-state-2': {'token': 'RUN_CONDITION', 'opcode': (1 << 26)},
        'program-state-3': {'token': 'RUN_CONDITION', 'opcode': (1 << 27)},
        'program-state-4': {'token': 'RUN_CONDITION', 'opcode': (1 << 28)},

        'no-signal': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 0)},
        'initializing': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 1)},
        'servo-output-setup-centre': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 2)},
        'servo-output-setup-left': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 3)},
        'servo-output-setup-right': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 4)},
        'reversing-setup-steering': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 5)},
        'reversing-setup-throttle': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 6)},
        'gear-changed': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 7)},
        'shelf-queen-mode': {'token': 'PRIORITY_RUN_CONDITION', 'opcode': (1 << 8)}
    };

    var car_state_tokens = {
        'light-switch-position-0': {'token': 'CAR_STATE', 'opcode': (1 << 0)},
        'light-switch-position-1': {'token': 'CAR_STATE', 'opcode': (1 << 1)},
        'light-switch-position-2': {'token': 'CAR_STATE', 'opcode': (1 << 2)},
        'light-switch-position-3': {'token': 'CAR_STATE', 'opcode': (1 << 3)},
        'light-switch-position-4': {'token': 'CAR_STATE', 'opcode': (1 << 4)},
        'light-switch-position-5': {'token': 'CAR_STATE', 'opcode': (1 << 5)},
        'light-switch-position-6': {'token': 'CAR_STATE', 'opcode': (1 << 6)},
        'light-switch-position-7': {'token': 'CAR_STATE', 'opcode': (1 << 7)},
        'light-switch-position-8': {'token': 'CAR_STATE', 'opcode': (1 << 8)},

        'neutral': {'token': 'CAR_STATE', 'opcode': (1 << 9)},
        'forward': {'token': 'CAR_STATE', 'opcode': (1 << 10)},
        'reversing': {'token': 'CAR_STATE', 'opcode': (1 << 11)},
        'braking': {'token': 'CAR_STATE', 'opcode': (1 << 12)},

        'indicator-left': {'token': 'CAR_STATE', 'opcode': (1 << 13)},
        'indicator-right': {'token': 'CAR_STATE', 'opcode': (1 << 14)},
        'hazard': {'token': 'CAR_STATE', 'opcode': (1 << 15)},
        'blink-flag': {'token': 'CAR_STATE', 'opcode': (1 << 16)},
        'blink-left': {'token': 'CAR_STATE', 'opcode': (1 << 17)},
        'blink-right': {'token': 'CAR_STATE', 'opcode': (1 << 18)},

        'winch-disabled': {'token': 'CAR_STATE', 'opcode': (1 << 19)},
        'winch-idle': {'token': 'CAR_STATE', 'opcode': (1 << 20)},
        'winch-in': {'token': 'CAR_STATE', 'opcode': (1 << 21)},
        'winch-out': {'token': 'CAR_STATE', 'opcode': (1 << 22)},

        'servo-output-setup-centre': {'token': 'CAR_STATE', 'opcode': (1 << 24)},
        'servo-output-setup-left': {'token': 'CAR_STATE', 'opcode': (1 << 25)},
        'servo-output-setup-right': {'token': 'CAR_STATE', 'opcode': (1 << 26)},
        'reversing-setup-steering': {'token': 'CAR_STATE', 'opcode': (1 << 27)},
        'reversing-setup-throttle': {'token': 'CAR_STATE', 'opcode': (1 << 28)},
    };

    var reserved_words = {
        'abs': {'token': 'ABS', 'opcode': 0x40000000},
        'all': {'token': 'ALL', 'opcode': 0x80000000},
        'any': {'token': 'ANY', 'opcode': 0x60000000},
        'aux': {'token': 'AUX'},
        'aux2': {'token': 'AUX2'},
        'aux3': {'token': 'AUX3'},
        'const': {'token': 'CONST'},
        'end': {'token': 'END', 'opcode': 0xfe000000},
        'fade': {'token': 'FADE', 'opcode': 0x04000000},
        // 'gear': {'token': 'GEAR'},
        'global': {'token': 'GLOBAL'},
        'goto': {'token': 'GOTO', 'opcode': 0x01000000},
        'if': {'token': 'IF'},
        'is': {'token': 'IS', 'opcode': 0x60000000},
        'led': {'token': 'LED'},
        'leds': {'token': 'LEDS'},
        'none': {'token': 'NONE', 'opcode': 0xa0000000},
        'not': {'token': 'NOT', 'opcode': 0xa0000000},
        'or': {'token': 'OR'},
        'random': {'token': 'RANDOM'},
        'run': {'token': 'RUN'},
        'skip': {'token': 'SKIP'},
        'sleep': {'token': 'SLEEP', 'opcode': 0x06000000},
        'steering': {'token': 'STEERING'},
        'stepsize': {'token': 'STEPSIZE'},
        'throttle': {'token': 'THROTTLE'},
        'use': {'token': 'USE'},
        'var': {'token': 'VAR'},
        'when': {'token': 'WHEN'},

        '=': {'token': '=', 'opcode': 0x10000000},
        '>': {'token': 'GT', 'opcode': 0x2c000000},
        '<': {'token': 'LT', 'opcode': 0x34000000},
        '+=': {'token': 'ADD_ASSIGN', 'opcode': 0x12000000},
        '-=': {'token': 'SUB_ASSIGN', 'opcode': 0x14000000},
        '*=': {'token': 'MUL_ASSIGN', 'opcode': 0x16000000},
        '/=': {'token': 'DIV_ASSIGN', 'opcode': 0x18000000},
        '&=': {'token': 'AND_ASSIGN', 'opcode': 0x1a000000},
        '|=': {'token': 'OR_ASSIGN', 'opcode': 0x1c000000},
        '^=': {'token': 'XOR_ASSIGN', 'opcode': 0x1e000000},
        '%=': {'token': 'MOD_ASSIGN', 'opcode': 0x38000000},
        '==': {'token': 'EQ', 'opcode': 0x20000000},
        '!=': {'token': 'NE', 'opcode': 0x24000000},
        '>=': {'token': 'GE', 'opcode': 0x28000000},
        '<=': {'token': 'LE', 'opcode': 0x30000000},
    };

    // Helper table to convert "if <test>" to the corresponding "skip if <test>"
    // function.
    var if_expressions = {
        '>': '<=',
        '<': '>=',
        '==': '!=',
        '!=': '==',
        '>=': '<',
        '<=': '>',
        'any': 'none',
        'none': 'any',
        'is': 'not',
        'not': 'is',
    }

    var MODULE = 'SYMBOL';


    // ****************************************************************************
    var dump_symbol_table = function () {
        var i, s, f;
        var msg = '\n';

        msg += 'Symbol table:\n';

        for (i = 0; i < symbol_table.length; i += 1) {
            s = symbol_table[i];
            msg += 'name="' + s.name + '", token=' + s.token + ' opcode=' + s.opcode + '\n';
        }

        msg += '\n';
        msg += 'Forward declarations to resolve:\n';
        if (forward_declaration_table.length === 0) {
            msg += '(none)\n';
        } else {
            for (i = 0; i < forward_declaration_table.length; i += 1) {
                f = forward_declaration_table[i];
                msg += 'label="' + f.symbol.name + '" pc=' + f.pc + ' opcode=' + f.symbol.opcode + '\n';
            }
        }
        msg += '\n';
        parser.yy.logger.log(MODULE, 'INFO', msg);
    };


    // *************************************************************************
    var check_for_light_switch_position = function (name) {
        var positions = {
            'light-switch-position-0': 1,
            'light-switch-position-1': 2,
            'light-switch-position-2': 3,
            'light-switch-position-3': 4,
            'light-switch-position-4': 5,
            'light-switch-position-5': 6,
            'light-switch-position-6': 7,
            'light-switch-position-7': 8,
            'light-switch-position-8': 9
        };

        if (positions[name]) {
            if (number_of_light_switch_positions < positions[name]) {
                number_of_light_switch_positions = positions[name];
            }
        }
    };


    // *************************************************************************
    var add_forward_declaration = function (symbol, decleration_pc, location) {
        var forward_decleration = {
            'symbol': symbol,
            'pc': decleration_pc,
            'location': location
        };

        forward_declaration_table.push(forward_decleration);
    };


    // *************************************************************************
    var get_forward_declerations = function () {
        return forward_declaration_table;
    };


    // *************************************************************************
    var remove_local_symbols = function () {
        var i;
        leds_used = 0;

        forward_declaration_table = [];

        for (i = symbol_table.length - 1; i >= 0; i -= 1) {
            if (symbol_table[i].token !== 'GLOBAL_VARIABLE') {
                symbol_table.splice(i, 1);
            }
        }
    };


    // *************************************************************************
    var set_symbol = function (name, token, opcode, location) {
        var i, s;
        for (i = 0; i < symbol_table.length; i += 1) {
            s = symbol_table[i];
            if (s.name === name) {
                if (s.opcode !== -1) {
                    parser.yy.emitter.yyerror('Redefinition of symbol ' + name, {
                        loc: location
                    });
                }
                symbol_table[i].token = token;
                symbol_table[i].opcode = opcode;

                parser.yy.logger.log(MODULE, 'INFO', 'Set "' + name + '" as token=' + token + ' opcode=' + opcode);
                return;
            }
        }

        parser.yy.logger.log(MODULE, 'ERROR', 'Could not find "' + name + '"');
    };


    // *************************************************************************
    var get_symbol = function (name, parse_state, location) {
        var i, s;
        if (parse_state === 'EXPECTING_RUN_CONDITION') {
            check_for_light_switch_position(name);
            return run_condition_tokens[name] || undeclared_symbol;
        }

        if (parse_state === 'EXPECTING_CAR_STATE') {
            check_for_light_switch_position(name);
            return car_state_tokens[name] || undeclared_symbol;
        }

        // See if we are dealing with a LOCAL symbol
        for (i = 0; i < symbol_table.length; i += 1) {
            s = symbol_table[i];

            if (s.token === 'GLOBAL_VARIABLE') {
                continue;
            }

            if (s.name === name) {
                if (s.token === 'LABEL') {
                    if (s.opcode === -1) {
                        add_forward_declaration(s, parser.yy.emitter.pc(), location);
                        parser.yy.logger.log(MODULE, 'INFO', 'Using forward declared label ' + name);
                    }
                }
                return s;
            }
        }

        // See if we are dealing with a GLOBAL symbol
        for (i = 0; i < symbol_table.length; i += 1) {
            s = symbol_table[i];

            if (s.token !== 'GLOBAL_VARIABLE') {
                continue;
            }
            if (s.name === name) {
                return s;
            }
        }

        parser.yy.logger.log(MODULE, 'INFO', 'Undeclared symbol ' + name);

        return undeclared_symbol;
    };


    // *************************************************************************
    var add_symbol = function (name, token, opcode, location) {
        var led_bit;
        var new_symbol = {
            'name': name,
            'token': token,
            'opcode': opcode
        };

        if (token === 'GLOBAL_VARIABLE'  ||  token === 'VARIABLE') {
            if (opcode === -1) {
                new_symbol.opcode = next_variable_index;
                next_variable_index += 1;
            }
        }

        if (token === 'LED_ID') {
            if (opcode < 0  ||  opcode > 31) {
                parser.yy.emitter.yyerror('LED index out of range (must be 0..31)', {
                    loc: location
                });
            } else {
                add_to_leds_used(opcode);
            }
        }

        symbol_table.push(new_symbol);

        if (token === 'LABEL'  &&  opcode === -1) {
            add_forward_declaration(new_symbol, parser.yy.emitter.pc(), location);
            parser.yy.logger.log(MODULE, 'INFO', 'Forward declaration of label ' + name);
        }
    };


    // *************************************************************************
    var get_reserved_word = function (name) {
        return reserved_words[name] ||
            parser.yy.logger.log(MODULE, 'FATAL', 'ASSERT: reserved word ' + name + ' is not in the table');
    };


    // *************************************************************************
    var get_if_expression = function (name) {
        if (if_expressions[name]) {
            return get_reserved_word(if_expressions[name]);
        }

        parser.yy.logger.log(MODULE, 'FATAL', 'ASSERT: IF test expression ' + name + ' is not supported');
    };


    // *************************************************************************
    var get_leds_used = function () {
        return leds_used;
    };


    // *************************************************************************
    var set_leds_used = function (led_bits) {
        parser.yy.logger.log(MODULE, 'DEBUG', 'set_leds_used() ' + led_bits);
        leds_used = led_bits;
    };


    // *************************************************************************
    var add_to_leds_used = function (led_number) {
        parser.yy.logger.log(MODULE, 'DEBUG', 'add_to_leds_used() ' + led_number);

        var led_bit = Math.pow(2, led_number);
        if (!(leds_used & led_bit)) {
            // Add LED to bit-field of leds_used
            leds_used += led_bit;
        }

        // IMPORTANT: do NOT use logical OR because JavaScript computes bitwise
        // logical operations as signed 32 bit Integer! So we end up with -1
        // instead of 0xffffffff ...
        //
        // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Bitwise_Operators

        // var led_bit = Math.pow(2, led_number);
        // leds_used |= led_bit;
    };


    // *************************************************************************
    var get_number_of_light_switch_positions = function () {
        return number_of_light_switch_positions;
    };


    // *************************************************************************
    var reset = function () {
        symbol_table = [];
        forward_declaration_table = [];
        next_variable_index = 0;
        leds_used = 0;
        number_of_light_switch_positions = 0;

        if (parser !== undefined) {
            parser.yy.line_is_empty = true;
            parser.yy.parse_state = 'UNKNOWN_PARSE_STATE';
        }

        add_symbol('clicks', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('light-switch-position', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('gear', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('servo', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('program-state-0', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('program-state-1', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('program-state-2', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('program-state-3', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
        add_symbol('program-state-4', 'GLOBAL_VARIABLE', next_variable_index);
        next_variable_index += 1;
    };


    // *************************************************************************
    var set_parser = function (e) {
        parser = e;
        reset();
    };


    // *************************************************************************
    return {
        set_parser: set_parser,
        add_symbol: add_symbol,
        get_symbol: get_symbol,
        set_symbol: set_symbol,
        get_reserved_word: get_reserved_word,
        get_if_expression: get_if_expression,
        get_number_of_light_switch_positions: get_number_of_light_switch_positions,
        add_to_leds_used: add_to_leds_used,
        set_leds_used: set_leds_used,
        get_leds_used: get_leds_used,
        get_forward_declerations: get_forward_declerations,
        remove_local_symbols: remove_local_symbols,
        dump_symbol_table: dump_symbol_table,
        reset: reset
    };

}());


// node.js exports; hide from browser where exports is undefined and use strict
// would trigger.
if (typeof exports !== 'undefined') {
    exports.symbols = symbols;
}
