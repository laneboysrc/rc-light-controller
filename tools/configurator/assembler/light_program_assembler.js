#!/usr/bin/env node

var parser = require("./light_program").parser;
var symbols = require("./symbols").symbols;
var emitter = require("./emitter").emitter;
var logger = require("./log").logger;

// Ugly hack...
// We need parser, symbols and emitter know of each-other.
// The only way that I could make work in both the browser and within node
// was to set their references explicitely
//
// Maybe going forward best will be to set the parser into symbols and emitter,
// and have them pull out the other one via yy. This way we also should get
// access to error functions of the parser


logger.set_log_level("INFO");

parser.yy = {
    symbols: symbols,
    emitter: emitter,
    logger: logger
}

emitter.set_parser(parser);
symbols.set_parser(parser);


if (!process.argv[2]) {
    console.log('Usage: ' + process.argv[1] + ' <light-program-file>');
    process.exit(1);
}

var source =
    require('fs').readFileSync(require('path').normalize(process.argv[2]), "utf8");

var programs = parser.parse(source);

console.log(programs);
