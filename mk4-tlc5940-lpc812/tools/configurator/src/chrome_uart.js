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
    var connectionId;
    var receiveBuffer = '';
    var readLineResolver;
    var readTimeoutTimer;
    var timeoutMs = 1 * 1000;

    let last_sent = '';

    var init = async function (port) {
        if (typeof chrome === 'undefined'  ||  !chrome.serial) {
            throw 'chrome.serial not available. Not running Chrome/Chromium?';
        }

        return await new Promise(resolve => {
            chrome.serial.connect(port, ci => {
                if (!ci) {
                    throw 'Unable to connect to ' + port;
                }

                connectionId = ci.connectionId;
                console.log('Opened port', port, ci);
                resolve(chrome_uart);
            });
        });
    };

    var receiveCallback = function (info) {
        // console.log('receiveCallback', info);

        const dummy = {
            '?': 'Synchronized\r\n',
            'Synchronized\r\n': 'OK\r\n',
            '12000\r\n': 'OK\r\n',
            'A 0\r\n': '0\r\n'
        };

        if (readTimeoutTimer) {
            clearTimeout(readTimeoutTimer);
            readTimeoutTimer = undefined;
        }

        if (readLineResolver) {
            for (let q in dummy) {
                if (q == last_sent) {
                    readLineResolver(dummy[q]);
                    readLineResolver = undefined;
                    return;
                }
            }

            readLineResolver('0\r\n');
            readLineResolver = undefined;
        }
    };

    var open = async function (baudrate, bits, parity, stopbits) {
        console.log('open', baudrate, bits, parity, stopbits);

        return await new Promise(resolve => {
            chrome.serial.update(connectionId, {bitrate: baudrate}, result => {
                if (!result) {
                    throw 'Unable to update the serial port configuration';
                }

                chrome.serial.onReceive.addListener(receiveCallback);
                resolve();
            });
        });
    };

    var read = function () {
        console.log('read');
    };

    var readline = async function () {
        console.log('readline');

        return await new Promise(resolve => {
            if (timeoutMs) {
                readTimeoutTimer = setTimeout(receiveCallback, timeoutMs);
            }
            readLineResolver = resolve;
        });
    };

    var write = async function (data) {
        console.log('write', data);

        return await new Promise(resolve => {
            let enc = new TextEncoder();
            let binaryData = enc.encode(data);

            last_sent = data;

            chrome.serial.send(connectionId, binaryData, sendInfo => {
                if (sendInfo.hasOwnProperty('error')) {
                    throw 'Error while sending serial data:' + sendInfo.error;
                }

                resolve();
            });
        });
    };

    var flush = function () {
        console.log('flush');
        receiveBuffer = '';
    };

    var close = async function () {
        console.log('close');

        return await new Promise(resolve => {
            chrome.serial.disconnect(connectionId, result => {
                if (!result) {
                    throw 'Error closing serial port';
                }

                resolve();
            });
        });
    };

    var setUartTimeout = function (timeout) {
        console.log('setUartTimeout', timeout);
        timeoutMs = timeout * 1000;
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
        setTimeout: setUartTimeout
    };
})();
