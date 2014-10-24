// This file was modified from source code found at
//
//      https://github.com/bminer/intel-hex.js
//
// which had the following license attached:
//
//
//
// The MIT License (MIT)
//
// Copyright (c) 2013. Blake C. Miner.
// http://blakeminer.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


// The original file was very node.js specific, exporting the parse
// function and using new constructs like const and Buffer.
// This was back-ported to ECMAScript v5.

var intel_hex = (function () {
    "use strict";

    // Intel Hex record types
    var DATA = 0;
    var END_OF_FILE = 1;
    var EXT_SEGMENT_ADDR = 2;
    var START_SEGMENT_ADDR = 3;
    var EXT_LINEAR_ADDR = 4;
    var START_LINEAR_ADDR = 5;

    var EMPTY_VALUE = 0xff;

    // Number of characters of the smallest possible Intel-hex record
    var SMALLEST_LINE = 11;


    // intel_hex.parse(data)
    //
    // data: Intel Hex file (string in ASCII format or Buffer Object)
    //
    // returns an Object with the following properties:
    //  data: data as array, padded with 0xff
    //      where data is empty.
    //  startSegmentAddress: the address provided by the last start segment
    //      address record; 'null' if not present in the intel-hex file.
    //  startLinearAddress: the address provided by the last start linear
    //      address record; 'null', if not present in the intel-hex file.
    //
    // Special thanks to: http://en.wikipedia.org/wiki/Intel_HEX
    //
    var parse = function (data) {

        //Initialization
        var buf = [];
        var highAddress = 0;    // Upper address
        var startSegmentAddress = null;
        var startLinearAddress = null;
        var lineNum = 0;     // Line number in the Intel Hex string
        var pos = 0;            // Current position in the Intel Hex string

        while ((pos + SMALLEST_LINE) <= data.length) {
            // Parse an entire line
            if (data.charAt(pos++) != ":") {
                throw new Error("Line " + (lineNum + 1) +
                    " does not start with a colon (:).");
            }
            else{
                ++lineNum;
            }

            // Number of bytes (hex digit pairs) in the data field
            var dataLength = parseInt(data.substr(pos, 2), 16);
            pos += 2;

            // Get 16-bit address (big-endian)
            var lowAddress = parseInt(data.substr(pos, 4), 16);
            pos += 4;

            // Record type
            var recordType = parseInt(data.substr(pos, 2), 16);
            pos += 2;

            //Data field (hex-encoded string)
            var dataField = data.substr(pos, dataLength * 2);
            var dataFieldBuf = [];
            for (var i = 0; i < dataField.length; i += 2) {
                dataFieldBuf.push(parseInt(dataField.substr(i, 2), 16));
            }
            pos += dataLength * 2;

            //Checksum
            var checksum = parseInt(data.substr(pos, 2), 16);
            pos += 2;

            //Validate checksum
            var calcChecksum = (dataLength + (lowAddress >> 8) + lowAddress +
                recordType) & 0xff;

            for (var i = 0; i < dataLength; i++) {
                calcChecksum = (calcChecksum + dataFieldBuf[i]) & 0xff;
            }
            calcChecksum = (0x100 - calcChecksum) & 0xff;

            if (checksum != calcChecksum) {
                throw new Error("Invalid checksum on line " + lineNum +
                    ": got " + checksum + ", but expected " + calcChecksum);
            }

            // Parse the record based on its recordType
            switch (recordType) {
                case DATA:
                    var absoluteAddress = highAddress + lowAddress;

                    // Write over skipped bytes with EMPTY_VALUE
                    while (absoluteAddress > buf.length) {
                        buf.push(EMPTY_VALUE);
                    }

                    // Write the dataFieldBuf to buf
                    buf = buf.concat(dataFieldBuf)
                    break;

                case END_OF_FILE:
                    if (dataLength != 0) {
                        throw new Error("Invalid EOF record on line " +
                            lineNum + ".");
                    }

                    return {
                        // FIXME: needs conversion to array
                        "data": buf,
                        "startSegmentAddress": startSegmentAddress,
                        "startLinearAddress": startLinearAddress
                    };
                    break;

                case EXT_SEGMENT_ADDR:
                    if (dataLength != 2 || lowAddress != 0){
                        throw new Error(
                            "Invalid extended segment address record on line " +
                                lineNum + ".");
                    }

                    highAddress = parseInt(dataField, 16) << 4;
                    break;

                case START_SEGMENT_ADDR:
                    if (dataLength != 4 || lowAddress != 0) {
                        throw new Error(
                            "Invalid start segment address record on line " +
                                lineNum + ".");
                    }

                    startSegmentAddress = parseInt(dataField, 16);
                    break;

                case EXT_LINEAR_ADDR:
                    if (dataLength != 2 || lowAddress != 0) {
                        throw new Error(
                            "Invalid extended linear address record on line " +
                            lineNum + ".");
                    }

                    highAddress = parseInt(dataField, 16) << 16;
                    break;

                case START_LINEAR_ADDR:
                    if (dataLength != 4 || lowAddress != 0) {
                        throw new Error(
                            "Invalid start linear address record on line " +
                            lineNum + ".");
                    }

                    startLinearAddress = parseInt(dataField, 16);
                    break;

                default:
                    throw new Error("Invalid record type (" + recordType +
                        ") on line " + lineNum);
                    break;
            }

            // Advance to the next line
            while (data.charAt(pos) == "\r"  ||  data.charAt(pos) == "\n") {
                pos++;
            }
        }

        throw new Error(
            "Unexpected end of input: missing or invalid EOF record.");
    };


    // *************************************************************************
    // API of this module:
    // *************************************************************************
    return {
        parse: parse,
    }
})();



// node.js exports; hide from browser where exports is undefined and use strict
// would trigger.
if (typeof exports !== 'undefined') {
    exports.intel_hex = intel_hex;
}