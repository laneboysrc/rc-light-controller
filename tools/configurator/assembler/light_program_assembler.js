var parser = require("./light_program").parser;

if (!process.argv[2]) {
    console.log('Usage: ' + process.argv[1] + ' <light-program-file>');
    process.exit(1);
}

var source =
    require('fs').readFileSync(require('path').normalize(process.argv[2]), "utf8");

var programs = parser.parse(source);

console.log(programs);
