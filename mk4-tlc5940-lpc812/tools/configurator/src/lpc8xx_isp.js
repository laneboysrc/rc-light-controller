/*jslint browser: true, bitwise: true, vars: true */

/*
    UART API (modeled after Pythons serial module)

    .open(baudrate, bits, parity, stopbits)
    .readline()
    .write()
    .close()
    .setTimeout()

    Callbacks:
        progress
        message

*/

class _lpc8xx_isp {
    constructor() {
        this.progressCallback = null;
        this.messageCallback = null;
        this.wait = true;
        this.busy = false;

        // Code Read Protection (CRP) address and patterns
        this.CRP_ADDRESS = 0x000002fc;

        // Prevents sampling of the ISP entry pin
        this.NO_ISP = 0x4E697370;

        // Access to chip via the SWD pins is disabled; allow partial flash update
        this.CRP1 = 0x12345678;

        // Access to chip via the SWD pins is disabled; most flash commands are disabled
        this.CRP2 = 0x87654321;

        // Access to chip via the SWD pins is disabled; prevents sampling of the ISP
        // entry pin
        this.CRP3 = 0x43218765;

        // RAM start address we use for programming and the Go command. We use
        // 0x10000300 because everything below may be locked by CRP.
        this.RAM_BASE_ADDRESS = 0x10000000;
        this.RAM_ADDRESS = this.RAM_BASE_ADDRESS + 0x300;

        this.FLASH_BASE_ADDRESS = 0x00000000;
        this.PAGE_SIZE = 64;
        this.SECTOR_SIZE = 1024;

        this.allow_code_protection = false;
    }

    message(msg) {
        if (this.messageCallback) {
            this.messageCallback(msg);
        }
    }

    async send_command(uart, command){
        // Send a command to the ISP and check that we receive and COMMAND_SUCCESS (0)
        // response.
        //
        // Note that this function assumes that ECHO is turned off.

        await uart.write(command + '\r\n');
        let response = await uart.readline();
        if (response != '0\r\n') {
            throw 'ERROR: Command "' + command + '" failed. Return code: ' + response;
        }
    }

    append_signature(bin) {
        // Calculate the signature that the ISP uses to detect "valid code"

        let signature = 0;
        for (let i = 0 ; i < 7; i += 1) {
            let vector = i * 4;
            signature = signature + (
                (bin[vector + 3] << 24) +
                (bin[vector + 2] << 16) +
                (bin[vector + 1] << 8) +
                (bin[vector]));
        }
        signature = (signature ^ 0xffffffff) + 1;   // Two's complement

        let vector8 = 28;
        bin[vector8 + 3] = (signature >> 24) & 0xff;
        bin[vector8 + 2] = (signature >> 16) & 0xff;
        bin[vector8 + 1] = (signature >> 8) & 0xff;
        bin[vector8] = signature & 0xff;
    }

    async open_isp(uart, port) {
        await uart.open(port, 115200, 8, 'n', 1);
        await uart.setTimeout(0.3);

        if (this.wait) {
            this.message('Waiting for LPC81x to enter ISP mode...');
        }

        for (;;) {
            await uart.write('?');

            let response = await uart.readline();
            if (response == 'Synchronized\r\n') {

                await uart.write('Synchronized\r\n');
                await uart.readline();        // Discard echo

                response = await uart.readline();
                if (response != 'OK\r\n') {
                    throw 'ERROR: Expected "OK" after sending  "Synchronized", but received "' + response + '"';
                }

                // Send crystal frequency in kHz (always 12 MHz for the LPC81x)
                await uart.write('12000\r\n');
                await uart.readline();        // Discard echo

                response = await uart.readline();
                if (response != 'OK\r\n') {
                    throw 'ERROR: Expected "OK" after sending crystal frequency, but received "' + response + '"';
                }

                await uart.write('A 0\r\n');  // Turn ECHO off
                await uart.readline();        // Discard (last) echo

                response = await uart.readline();
                if (response != '0\r\n') {
                    throw 'ERROR: Expected "0" after turning ECHO off, but received "' + response + '"';
                }
                await uart.setTimeout(5);
                return;
            }

            else if (response == '?') {
                // We may already be in ISP mode, with ECHO being on.
                // We terminate with CR/LF, which should respond with "1\r\n" because
                // '?' is an invalid command.
                // We have to skip the ECHOed CR/LF though!
                await uart.write('\r\n');
                await uart.readline();        // Discard echo

                response = await uart.readline();
                if (response != '1\r\n') {
                    if (!this.wait) {
                        throw 'ERROR: LPC81x not in ISP mode.';
                    }
                }
                else {
                    await uart.write('A 0\r\n');      // Turn ECHO off
                    await uart.readline();            // Discard (last) echo

                    response = await uart.readline();
                    if (response != '0\r\n') {
                        throw 'ERROR: Expected "0" after turning ECHO off, but received "' + response + '"';
                    }
                    await uart.setTimeout(5);
                    return;
                }
            }

            else {
                // We may already be in ISP mode, with ECHO being off.
                // We send a CR/LF, which should respond with "1\r\n" because
                // '?' is an invalid command.
                await uart.write('\r\n');

                response = await uart.readline();
                if (response == '1\r\n') {
                    await uart.setTimeout(5);
                    return;
                }
                if (!this.wait) {
                    throw 'ERROR: LPC81x not in ISP mode.';
                }
            }
        }
    }

    async program(uart, bin) {
        // Write the given binary image file into the flash memory.

        // The image is checked whether it contains any of the code protection
        // values, and flashing is aborted (unless instructed with a flag)
        // so that we don't "brick" the ISP functionality.

        // Also the checksum of the vectors that the ISP uses to detect valid
        // flash is generated and added to the image before flashing.

        let used_sectors = (bin.length / this.SECTOR_SIZE);


        // Abort if the Code Read Protection in the image contains one of the
        // special patterns. We don't want to lock us out of the chip...
        if (!this.allow_code_protection) {
            let pattern = ((bin[this.CRP_ADDRESS + 3] << 24) + (bin[this.CRP_ADDRESS + 2] << 16) + (bin[this.CRP_ADDRESS + 1] << 8) + bin[this.CRP_ADDRESS]);

            if (pattern == this.NO_ISP) {
                throw 'ERROR: NO_ISP code read protection detected in image file';
            }

            if (pattern == this.CRP1) {
                throw 'ERROR: CRP1 code read protection detected in image file';
            }

            if (pattern == this.CRP2) {
                throw 'ERROR: CRP2 code read protection detected in image file';
            }

            if (pattern == this.CRP3) {
                throw 'ERROR: CRP3 code read protection detected in image file';
            }
        }


        // Calculate the signature that the ISP uses to detect "valid code"
        this.append_signature(bin);

        // Unlock the chip with the magic number
        await this.send_command(uart, 'U 23130');


        // Program the image
        for (let index = 0; index < used_sectors; index += 1) {
            if (this.progressCallback) {
                this.progressCallback(index / used_sectors);
            }

            let sector = index;

            // Erase the sector
            await this.send_command(uart, 'P ' + sector + ' ' + sector);
            await this.send_command(uart, 'E ' + sector + ' ' + sector);

            let address = sector * this.SECTOR_SIZE;
            let last_address = address + this.SECTOR_SIZE - 1;

            let data = bin.slice(address, last_address+1);

            await this.send_command(uart, 'W ' + this.RAM_ADDRESS + ' ' + data.length);
            await uart.write(data);
            await this.send_command(uart, 'P ' + sector + ' ' + sector);
            await this.send_command(uart, 'C ' + address + ' ' + this.RAM_ADDRESS + ' ' + this.SECTOR_SIZE);
        }

        if (this.progressCallback) {
            this.progressCallback(1.0);
        }
    }

    async read(uart) {
        // TODO
    }

    async reset_mcu(uart) {
        /*
        Reset the MCU to start the application.
        We do that by downloading a small binary into RAM. This binary corresponds
        to the following C code:

            SCB->AIRCR = 0x05FA0004;

        This code resets the ARM CPU by setting SYSRESETREQ. We load this
        program into RAM and run it with the "Go" command.
        */
        const reset_program = [
            0x01, 0x4a, 0x02, 0x4b, 0x1a, 0x60, 0x70, 0x47,
            0x04, 0x00, 0xfa, 0x05, 0x0c, 0xed, 0x00, 0xe0];

        await this.send_command(uart, 'W ' + this.RAM_ADDRESS + ' ' + reset_program.length);
        await uart.write(reset_program);

        // Unlock the Go command
        await this.send_command(uart, 'U 23130');

        // Run the program from RAM. Note that this command does not respond with
        // COMMAND_SUCCESS as it directly executes.
        await uart.write('G ' + this.RAM_ADDRESS + ' T\r\n');
    }

    async flash(uart, port, bin) {
        let success = false;

        if (this.busy) {
            this.message('Flashing already in progress');
            return success;
        }
        this.busy = true;

        try {
            await this.open_isp(uart, port);

            this.message('Programming ...');
            await this.program(uart, bin);
            this.message('Starting the program...');
            await this.reset_mcu(uart);
            this.message('Done.');
            success = true;
        }
        catch (e) {
            this.message('Error: ' + e);
        }
        finally {
            await uart.close();
            this.busy = false;
        }

        return success;
    }

    async read(uart, port) {
        let bin = [];

        if (this.busy) {
            this.message('Reading already in progress');
            return bin;
        }
        this.busy = true;

        try {
            await this.open_isp(uart, port);

            this.message('Reading ...');
            bin = await this.read(uart);
            this.message('Done.');
        }
        catch (e) {
            this.message(e);
        }
        finally {
            await uart.close();
            this.busy = false;
        }

        return bin;
    }
    set onMessageCallback(fn) {
        this.messageCallback = fn;
    }

    set onProgressCallback(fn) {
        this.progressCallback = fn;
    }

}

var lpc8xx_isp = new _lpc8xx_isp();
