/*global chrome */

/*  API:

    .open(port, baudrate, bits, parity, stopbits)
    .read(count)
    .readline()
    .write(data)
    .close()
    .setTimeout(timeout)
*/

class chrome_uart {
    constructor() {
        this.connectionId = undefined;
        this.receiveBuffer = '';
        this.readResolver = undefined;
        this.readCount = 0;
        this.readlineResolver = undefined;
        this.readTimeoutTimer = undefined;
        this.timeoutMs = 0;
        this.eol = '\r\n';

        // this.logger = console;
        this.logger = {
            log: () => {},
            info: () => {},
            warn: () => {},
            error: () => {},
            dir: () => {}
        };
    }

    open(port, baudrate) {
        this.logger.log('open', baudrate);

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
                this.logger.log('Opened port', port, ci);

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

        this.logger.log('receiveCallback "' + received + '"');

        this.receiveBuffer += received;

        let crlf_index = this.receiveBuffer.indexOf(this.eol);

        if (this.readlineResolver && crlf_index >= 0) {
            if (this.readTimeoutTimer) {
                clearTimeout(this.readTimeoutTimer);
                this.readTimeoutTimer = undefined;
            }

            crlf_index += this.eol.length;
            let result = this.receiveBuffer.substr(0, crlf_index);
            this.receiveBuffer = this.receiveBuffer.substr(crlf_index);

            this.logger.log('readlineResolver returning "' + result + '"');
            this.readlineResolver(result);
            this.readlineResolver = undefined;
        }
        else if (this.readResolver  &&  this.receiveBuffer.length >= this.readCount) {
            if (this.readTimeoutTimer) {
                clearTimeout(this.readTimeoutTimer);
                this.readTimeoutTimer = undefined;
            }

            let result = this.receiveBuffer.substr(0, this.readCount);
            this.receiveBuffer = this.receiveBuffer.substr(this.readCount);
            this.readResolver(result);
            this.readResolver = undefined;
        }

        this.logger.log('receiveCallback end', this.receiveBuffer);
    }

    readTimeoutHandler() {
        this.logger.log('readTimeoutHandler');

        clearTimeout(this.readTimeoutTimer);
        this.readTimeoutTimer = undefined;

        if (this.readlineResolver) {
            this.readlineResolver(this.receiveBuffer);
            this.receiveBuffer = '';
            this.readlineResolver = undefined;
            return;
        }

        if (this.readResolver) {
            this.readResolver(this.receiveBuffer);
            this.receiveBuffer = '';
            this.readResolver = undefined;
        }
    }

    setTimeout(timeout) {
        this.logger.log('setTimeout', timeout);

        this.timeoutMs = timeout * 1000;
    }

    read(count) {
        this.logger.log('read', this.receiveBuffer);

        if (this.receiveBuffer.length >= count) {
            let result = this.receiveBuffer.substr(0, count);
            this.receiveBuffer = this.receiveBuffer.substr(count);
            return result;
        }

        return new Promise(resolve => {
            if (this.timeoutMs) {
                this.readTimeoutTimer = setTimeout(this.readTimeoutHandler.bind(this), this.timeoutMs);
            }
            this.readCount = count;
            this.readResolver = resolve;
        });
    }

    readline() {
        this.logger.log('readline', this.receiveBuffer);

        let crlf_index = this.receiveBuffer.indexOf(this.eol);
        if (crlf_index >= 0) {
            crlf_index += this.eol.length;
            let result = this.receiveBuffer.substr(0, crlf_index);
            this.receiveBuffer = this.receiveBuffer.substr(crlf_index);
            this.logger.log('readline returning "' + result + '"');
            return result;
        }

        return new Promise(resolve => {
            if (this.timeoutMs) {
                this.readTimeoutTimer = setTimeout(this.readTimeoutHandler.bind(this), this.timeoutMs);
            }
            this.readlineResolver = resolve;
        });
    }

    write(data) {
        this.logger.log('write', data);

        return new Promise((resolve, reject) => {
            let binaryData = new ArrayBuffer(data.length);
            let dataView = new Uint8Array(binaryData);

            for (let i=0; i<data.length; i+=1) {
                if (data.hasOwnProperty('charCodeAt')) {
                    dataView[i] = data.charCodeAt(i);
                }
                else {
                    dataView[i] = data[i];
                }
            }

            chrome.serial.send(this.connectionId, binaryData, sendInfo => {
                this.logger.log('wrote', sendInfo.bytesSent, 'bytes');
                if (sendInfo.hasOwnProperty('error')) {
                    reject('Error while sending serial data:' + sendInfo.error);
                    return;
                }

                resolve();
            });
        });
    }

    close() {
        this.logger.log('close');

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

