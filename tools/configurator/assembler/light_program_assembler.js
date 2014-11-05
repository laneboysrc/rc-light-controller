var parser = require("./light_program").parser;
var symbols = require("./symbols").symbols;
var emitter = require("./emitter").emitter;

// Ugly hack...
// We need parser, symbols and emitter know of each-other.
// The only way that I could make work in both the browser and within node
// was to set their references explicitely
//
// Maybe going forward best will be to set the parser into symbols and emitter,
// and have them pull out the other one via yy. This way we also should get
// access to error functions of the parser

parser.yy = {
    "symbols": symbols,
    "emitter": emitter,
}

symbols.set_emitter(emitter);
emitter.set_symbols(symbols);


if (!process.argv[2]) {
    console.log('Usage: ' + process.argv[1] + ' <light-program-file>');
    process.exit(1);
}

var source =
    require('fs').readFileSync(require('path').normalize(process.argv[2]), "utf8");

var programs = parser.parse(source);

console.log(programs);
