'use strict';

/*global dfu  */



class flash_dfu {
    constructor() {
        this.progressCallback = null;
        this.messageCallback = null;
        this.cancelButtonEnableFunction = null;
        this.busy = false;
        this.cancel_ = false;

        this.logger = console;

        this.dfuLogger = function () {};
    }

    message(msg) {
        if (this.messageCallback) {
            this.messageCallback(msg);
        }
    }

    async reboot_into_bootloader(device) {
        this.message('Restarting into the bootloader ...');

        let serial_number = device.device_.serialNumber;

        await device.detach();
        try {
            await device.close();
            await device.waitDisconnected(5000);
            device = null;
        }
        catch (error) {
            this.logger.log('DFU detach failed: ' + error);
        }


        // Wait up to five seconds for the device to restart and USB to be detected
        const poll_time = 100;
        let timeout_count = 5000 / poll_time;
        while (timeout_count) {
            await new Promise(resolve => { setTimeout(resolve, poll_time); });

            let dfu_devices = await dfu.findAllDfuInterfaces();
            for (let dfu_device of dfu_devices) {
                if (dfu_device.device_.serialNumber == serial_number) {
                    device = dfu_device;
                    break;
                }
            }

            if (this.cancel_) {
                return null;
            }

            if (device) {
                break;
            }

            timeout_count -= 1;
        }

        if (device == null) {
            throw 'Unable to find device with serial number ' + serial_number + ' after DFU detach';
        }

        await device.open();
        return device;
    }

    async flash(device, bin) {
        let success = false;

        if (this.busy) {
            this.message('Flashing already in progress');
            return success;
        }
        this.busy = true;
        this.cancel_ = false;


        try {
            await device.open();

            let altMode = device.settings.alternate.interfaceProtocol;

            if (altMode == 0x01) {
                device = await this.reboot_into_bootloader(device);
            }

            if (this.cancel_) {
                return true;
            }

            let status = await device.getStatus();
            if (status.state == dfu.dfuERROR) {
                this.message('Clearing DFU status ...');
                await device.clearStatus();
            }

            if (this.cancel_) {
                return true;
            }

            // Obtain the transfer size and whether the device is manifestation tolerant
            this.message('Reading DFU information ...');

            let data = await device.readConfigurationDescriptor(0);
            let configDesc = dfu.parseConfigurationDescriptor(data);
            let funcDesc = null;
            let configValue = device.settings.configuration.configurationValue;
            if (configDesc.bConfigurationValue == configValue) {
                for (let desc of configDesc.descriptors) {
                    if (desc.bDescriptorType == 0x21 && desc.hasOwnProperty('bcdDFUVersion')) {
                        funcDesc = desc;
                        break;
                    }
                }
            }

            if (funcDesc == null) {
                throw 'Failed to read configuration descriptor to parse DFU attributes';
            }

            const transferSize = funcDesc.wTransferSize;
            const manifestationTolerant = ((funcDesc.bmAttributes & 0x04) != 0);


            device.logDebug = this.dfuLogger;
            device.logProgress = function(done, total) {
                if (this.progressCallback) {
                    this.progressCallback(done / total);
                }
            }.bind(this);

            if (this.cancel_) {
                return true;
            }

            // Point of no-return, disable cancel button
            if (this.cancelButtonEnableFunction) {
                this.cancelButtonEnableFunction(false);
            }

            this.message('Flashing firmware ...');
            await device.do_download(transferSize, new Uint8Array(bin.slice(8192)), manifestationTolerant);

            this.message('Waiting for device to restart ...');

            await device.waitDisconnected(5000);

            success = true;
        }
        catch (e) {
            this.message('Error: ' + e);
        }
        finally {
            this.busy = false;
        }

        return success;
    }

    async read_flash(device) {
        let bin = [];

        if (this.busy) {
            this.message('Reading already in progress');
            return bin;
        }
        this.busy = true;

        try {
            // FIXME: implement!
            bin = [];
        }
        catch (e) {
            this.message('Error: ' + e);
        }
        finally {
            this.busy = false;
        }

        return bin;
    }

    cancel() {
        this.cancel_ = true;
    }

    set onMessageCallback(fn) {
        this.messageCallback = fn;
    }

    set onProgressCallback(fn) {
        this.progressCallback = fn;
    }

    set setCancelButtonEnabled(fn) {
        this.cancelButtonEnableFunction = fn;
    }
}
