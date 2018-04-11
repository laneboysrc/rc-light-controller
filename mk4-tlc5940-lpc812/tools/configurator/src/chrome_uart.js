/*global chrome */

'use strict';

/*
   .open(baudrate, bits, parity, stopbits)
    .read()
    .readline()
    .write()
    .flush()
    .close()
    .setTimeout(timeout)

*/

var chrome_uart = (function () {

    var init = function (port) {
        return this;
    };

    var open = function (baudrate, bits, parity, stopbits) {

    };

    var read = function () {

    };

    var readline = function () {

    };

    var write = function () {

    };

    var flush = function () {

    };

    var close = function () {

    };

    var setTimeout = function (timeout) {

    };

    // *************************************************************************
    return {
        init: init,
        open: open,
        read: read,
        readline: readline,
        write: write,
        flush: flush,
        close: close,
        setTimeout: setTimeout
    };
})();
