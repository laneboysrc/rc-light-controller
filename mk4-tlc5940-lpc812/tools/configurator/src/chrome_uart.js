/*global chrome */

'use strict';

/*
    .open(port, baudrate, bits, parity, stopbits)
    .readline()
    .write()
    .close()
    .setTimeout(timeout)
*/

var chrome_uart = (function () {
    var connectionId;
    var receiveBuffer = '';
    var receivedLines = [];
    var readLineResolver;
    var readTimeoutTimer;
    var timeoutMs = 0;

    var receiveCallback = function (info) {
        let received = '';

        if (typeof(info) !== 'undefined' && info.hasOwnProperty('data')) {
            for (let c of info.data) {
                received += String.fromCharCode(c);
            }
            // let dec = new TextDecoder();
            // received = dec.decode(info.data);
        }

        // console.log('receiveCallback "' + received + '"');

        for (let c of received) {
            receiveBuffer += c;
            if (c == '\n') {
                receivedLines.push(receiveBuffer);
                receiveBuffer = '';
            }
        }

        if (readLineResolver && receivedLines.length) {
            if (readTimeoutTimer) {
                clearTimeout(readTimeoutTimer);
                readTimeoutTimer = undefined;
            }

            let line = receivedLines.shift();
            // console.log('readline => ', line);
            readLineResolver(line);
            readLineResolver = undefined;
            return;
        }
        // console.log('receiveCallback end', receiveBuffer, receivedLines);
    };

    var readTimeoutHandler = function () {
        clearTimeout(readTimeoutTimer);
        readTimeoutTimer = undefined;
        if (readLineResolver) {
            let line = receiveBuffer;

            if (receivedLines.length) {
                line = receivedLines.shift();
            }

            // console.log('readline timeout => ', line);
            readLineResolver(line);
            readLineResolver = undefined;
        }
    }

    var open = async function (port, baudrate, bits, parity, stopbits) {
        // console.log('open', baudrate, bits, parity, stopbits);

        if (typeof chrome === 'undefined'  ||  !chrome.serial) {
            throw 'chrome.serial not available. Not running Chrome/Chromium?';
        }

        return new Promise((resolve, reject) => {
            chrome.serial.connect(port, {bitrate: baudrate}, ci => {
                if (chrome.runtime.lastError) {
                    reject('Unable to connect to ' + port);
                    return;
                }

                if (!ci) {
                    reject('Unable to connect to ' + port);
                    return;
                }

                connectionId = ci.connectionId;
                // console.log('Opened port', port, ci);

                chrome.serial.onReceive.addListener(receiveCallback.bind(this));

                chrome.serial.onReceiveError.addListener((info) => {
                    console.error(info.error);
                });

                resolve();
            });
        });
    };

    var readline = async function () {
        // console.log('readline', receiveBuffer, receivedLines);

        if (receivedLines.length) {
            return receivedLines.shift();
        }

        return new Promise(resolve => {
            if (timeoutMs) {
                readTimeoutTimer = setTimeout(readTimeoutHandler, timeoutMs);
            }
            readLineResolver = resolve;
        });
    };

    var write = async function (data) {
        // console.log('write', data);

        return new Promise((resolve, reject) => {
            let binaryData = new ArrayBuffer(data.length);
            let dataView = new Uint8Array(binaryData);

            for (let i=0; i<data.length; i+=1) {
                if (data instanceof Array) {
                    dataView[i] = data[i];
                }
                else {
                    dataView[i] = data.charCodeAt(i);
                }
            }

            chrome.serial.send(connectionId, binaryData, sendInfo => {
                // console.log('wrote', sendInfo.bytesSent, 'bytes');
                if (sendInfo.hasOwnProperty('error')) {
                    reject('Error while sending serial data:' + sendInfo.error);
                    return;
                }

                resolve();
            });
        });
    };

    var close = async function () {
        // console.log('close');

        for (let l of chrome.serial.onReceive.getListeners()) {
            chrome.serial.onReceive.removeListener(l.callback);
        }

        for (let l of chrome.serial.onReceiveError.getListeners()) {
            chrome.serial.onReceive.removeListener(l.callback);
        }

        if (connectionId) {
            return new Promise((resolve, reject) => {
                chrome.serial.disconnect(connectionId, result => {
                    connectionId = undefined;
                    if (!result) {
                        reject('Error closing serial port');
                        return;
                    }
                    resolve();
                });
            });
        }
    };

    var setUartTimeout = function (timeout) {
        // console.log('setUartTimeout', timeout);
        timeoutMs = timeout * 1000;
    };

    // *************************************************************************
    return {
        // init: init,
        open: open,
        readline: readline,
        write: write,
        close: close,
        setTimeout: setUartTimeout
    };
})();
