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

    var connectionInfo;

    var init = async function (port) {
        if (typeof chrome === 'undefined'  ||  !chrome.serial) {
            throw 'chrome.serial not available. Not running Chrome/Chromium?';
        }

        return await new Promise(resolve => {
            chrome.serial.connect(port, function (ci) {
                if (!ci) {
                    throw 'Unable to connect to ' + port;
                }

                connectionInfo = ci;
                resolve(chrome_uart);
            });
        });
    };

    var open = function (baudrate, bits, parity, stopbits) {
        console.log('open');
    };

    var read = function () {
        console.log('read');
    };

    var readline = function () {
        console.log('readline');
    };

    var write = function () {
        console.log('write');
    };

    var flush = function () {
        console.log('flush');
    };

    var close = function () {
        console.log('close');
    };

    var setTimeout = function (timeout) {
        console.log('setTimeout');
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
