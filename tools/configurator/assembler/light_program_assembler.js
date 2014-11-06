#!/usr/bin/env node

"use strict";

var program = require('commander');
var fs = require('fs');
var path = require('path');

var parser = require("./build/parser").parser;
var symbols = require("./symbols").symbols;
var emitter = require("./emitter").emitter;
var logger = require("./log").logger;

var output_file = 1;    // File handle of stdout


// *****************************************************************************
var hex = function (number) {
    var s = number.toString(16).toLowerCase();
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

    fs.writeSync(output_file, part1);
    fs.writeSync(output_file, number_of_programs.toString());
    fs.writeSync(output_file, part1b);

    for (i = 0; i < number_of_programs; i++) {
        fs.writeSync(output_file, part2);
        fs.writeSync(output_file, start_offset[i].toString());
        fs.writeSync(output_file, part2b);
    }

    fs.writeSync(output_file, part3);

    for (var i = 0; i < instructions.length; i++) {
        fs.writeSync(output_file, part4);
        fs.writeSync(output_file, hex(instructions[i]));
        fs.writeSync(output_file, part4b);
        if (instructions[i] == 0xfe000000) {
            fs.writeSync(output_file, "\n");
        }
    }

    fs.writeSync(output_file, part5);
}

function increaseVerbosity(v, total) {
  return total + 1;
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

program
  .version('1.0.0')
  .usage('[options] <source>')
  .option('-o, --output <value>', 'Output file. If omitted, output is printed to stdout.')
  .option('-v, --verbose', 'Verbose output. Specify multiple times for more output.', increaseVerbosity, 0)
  .parse(process.argv);

var sources = program.args;

if (sources.length !== 1) {
    console.error("No source file given.");
    process.exit(1);
}

program.verbose = program.verbose || 0;
if (program.verbose < 1) {
    logger.set_log_level("FATAL");
}

if (program.output) {
    output_file = fs.openSync(program.output, "w");
}

var sourcecode = fs.readFileSync(path.normalize(sources[0]), "utf8");
try {
    var programs = parser.parse(sourcecode);
    make_c_output(output_file, programs);
}
catch (e) {
    var msg = "Errors occured while processing the light programs:\n";

    var errors = emitter.get_errors();
    if (errors.length > 0) {
        msg += "\n";
        for (var i = 0; i < errors.length; i++) {
            msg += errors[i].str + "\n\n";
        }
    }

    process.stderr.write(msg);

    fs.close(output_file);
    process.exit(1);
}
finally {
    fs.close(output_file);
}
