#!/usr/bin/env node

// Command line tool to diassemble light programs.
//
// The binary light assembly code is fed via STDIN, the disassembled result
// is printed on STDOUT

'use strict';
/*jslint node: true, vars: true */

var disassembler = require('./disassembler.js').disassembler;

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
