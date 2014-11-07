#!/usr/bin/env node
"use strict";

var disassembler = require("../disassembler.js").disassembler;

var input = '';

process.stdin.resume();

process.stdin.on('data', function (buffer) {
    input += buffer.toString();
});

process.stdin.on('end', function () {
    var instructions = disassembler.parse_c_code(input);
    var code = disassembler.disassemble(instructions);
    console.log(code);
});
