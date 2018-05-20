/*global chrome */

/*
    .open(port, baudrate, bits, parity, stopbits)
    .readline()
    .write()
    .close()
    .setTimeout(timeout)
*/

class chrome_uart {
    constructor() {
        this.connectionId = undefined;
        this.receiveBuffer = '';
        this.receivedLines = [];
        this.readLineResolver = undefined;
        this.readTimeoutTimer = undefined;
        this.timeoutMs = 0;
    }

    open(port, baudrate) {
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

                this.connectionId = ci.connectionId;
                // console.log('Opened port', port, ci);

                chrome.serial.onReceive.addListener(this.receiveCallback.bind(this));
                resolve();
            });
        });
    }

    receiveCallback(info) {
        let received = '';

        if (typeof(info) !== 'undefined' && info.hasOwnProperty('data')) {
            let a = new Uint8Array(info.data);
            a.forEach(c => {
                received += String.fromCharCode(c);
            });
        }

        // console.log('receiveCallback "' + received + '"');

        for (let c of received) {
            this.receiveBuffer += c;
            if (c == '\n') {
                this.receivedLines.push(this.receiveBuffer);
                this.receiveBuffer = '';
            }
        }

        if (this.readLineResolver && this.receivedLines.length) {
            if (this.readTimeoutTimer) {
                clearTimeout(this.readTimeoutTimer);
                this.readTimeoutTimer = undefined;
            }

            let line = this.receivedLines.shift();
            // console.log('readline => ', line);
            this.readLineResolver(line);
            this.readLineResolver = undefined;
            return;
        }
        // console.log('receiveCallback end', receiveBuffer, receivedLines);
    }

    readTimeoutHandler() {
        clearTimeout(this.readTimeoutTimer);
        this.readTimeoutTimer = undefined;
        if (this.readLineResolver) {
            let line = this.receiveBuffer;

            if (this.receivedLines.length) {
                line = this.receivedLines.shift();
            }

            // console.log('readline timeout => ', line);
            this.readLineResolver(line);
            this.readLineResolver = undefined;
        }
    }

    setTimeout(timeout) {
        // console.log('setUartTimeout', timeout);
        this.timeoutMs = timeout * 1000;
    }

    readline() {
        // console.log('readline', receiveBuffer, receivedLines);

        if (this.receivedLines.length) {
            return this.receivedLines.shift();
        }

        return new Promise(resolve => {
            if (this.timeoutMs) {
                this.readTimeoutTimer = setTimeout(this.readTimeoutHandler.bind(this), this.timeoutMs);
            }
            this.readLineResolver = resolve;
        });
    }

    write(data) {
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

            chrome.serial.send(this.connectionId, binaryData, sendInfo => {
                // console.log('wrote', sendInfo.bytesSent, 'bytes');
                if (sendInfo.hasOwnProperty('error')) {
                    reject('Error while sending serial data:' + sendInfo.error);
                    return;
                }

                resolve();
            });
        });
    }

    close() {
        // console.log('close');

        for (let l of chrome.serial.onReceive.getListeners()) {
            chrome.serial.onReceive.removeListener(l.callback);
        }

        for (let l of chrome.serial.onReceiveError.getListeners()) {
            chrome.serial.onReceive.removeListener(l.callback);
        }

        if (this.connectionId) {
            return new Promise((resolve, reject) => {
                chrome.serial.disconnect(this.connectionId, result => {
                    this.connectionId = undefined;
                    if (!result) {
                        reject('Error closing serial port');
                        return;
                    }
                    resolve();
                });
            });
        }
    }
}

