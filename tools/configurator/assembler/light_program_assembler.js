#!/usr/bin/env node

"use strict";

var parser = require("./light_program").parser;
var symbols = require("./symbols").symbols;
var emitter = require("./emitter").emitter;
var logger = require("./log").logger;


// *****************************************************************************
var hex = function (number) {
    var s = number.toString(16).toUpperCase();
    while (s.length < 8) {
        s = "0" + s;
    }
    return "0x" + s;
}


// *****************************************************************************
function make_c_output(output_file, programs) {
    var part1 =
        "#include <globals.h>\n" +
        "\n" +
        "__attribute__ ((section(\".light_programs\")))\n" +
        "const LIGHT_PROGRAMS_T light_programs = {\n" +
        "    .magic = {\n" +
        "        .magic_value = ROM_MAGIC,\n" +
        "        .type = LIGHT_PROGRAMS,\n" +
        "        .version = CONFIG_VERSION\n" +
        "    },\n" +
        "\n" +
        "    .number_of_programs = ";

    var part1b =
        ",\n" +
        "    .start = {\n";

    var part2 =
        "        &light_programs.programs[";

    var part2b =
        "],\n";

    var part3 =
        "    },\n" +
        "\n" +
        "    .programs = {\n";

    var part4 =
        "        ";

    var part4b =
        ",\n";

    var part5 =
        "    }\n" +
        "};\n";

    // Output the light programs data structure

    var number_of_programs = programs.number_of_programs;
    var start_offset = programs.start_offset;
    var instructions = programs.instructions;

    output_file.write(part1);
    output_file.write(number_of_programs.toString());
    output_file.write(part1b);

    for (i = 0; i < number_of_programs; i++) {
        output_file.write(part2);
        output_file.write(start_offset[i].toString());
        output_file.write(part2b);
    }

    output_file.write(part3);

    for (var i = 0; i < instructions.length; i++) {
        output_file.write(part4);
        output_file.write(hex(instructions[i]));
        output_file.write(part4b);
        if (instructions[i] == 0xfe000000) {
            output_file.write("\n");
        }
    }

    output_file.write(part5);
}


// Ugly hack...
// We need parser, symbols, emitter and logger know of each-other.
// We do this by using the shared state of the parser, and setting the parser
// to the modules that can pull out the shared state from the parser to get
// the desired references.
parser.yy = {
    symbols: symbols,
    emitter: emitter,
    logger: logger
}

emitter.set_parser(parser);
symbols.set_parser(parser);

//logger.set_log_level("INFO");

if (!process.argv[2]) {
    console.log('Usage: ' + process.argv[1] + ' <light-program-file>');
    process.exit(1);
}

var source =
    require('fs').readFileSync(require('path').normalize(process.argv[2]), "utf8");

var programs = parser.parse(source);
var output_file = process.stdout;

make_c_output(output_file, programs);
