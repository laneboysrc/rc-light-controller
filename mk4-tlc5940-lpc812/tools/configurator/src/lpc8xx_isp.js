/*jslint browser: true, bitwise: true, vars: true */

'use strict';

/*
    UART API (modeled after Pythons serial module)

    .open(baudrate, bits, parity, stopbits)
    .read()
    .readline()
    .write()
    .flush()
    .close()
    .setTimeout()

    Callbacks:
        progress
        message

*/

var lpc8xx_isp = (function () {
    var onProgressCallback = null;
    var onMessageCallback = null;
    var busy = false;
    var wait = false;

    // Code Read Protection (CRP) address and patterns
    const CRP_ADDRESS = 0x000002fc;

    // Prevents sampling of the ISP entry pin
    const NO_ISP = 0x4E697370;

    // Access to chip via the SWD pins is disabled; allow partial flash update
    const CRP1 = 0x12345678;

    // Access to chip via the SWD pins is disabled; most flash commands are disabled
    const CRP2 = 0x87654321;

    // Access to chip via the SWD pins is disabled; prevents sampling of the ISP
    // entry pin
    const CRP3 = 0x43218765;

    // RAM start address we use for programming and the Go command. We use
    // 0x10000300 because everything below may be locked by CRP.
    const RAM_BASE_ADDRESS = 0x10000000;
    const RAM_ADDRESS = RAM_BASE_ADDRESS + 0x300;

    const FLASH_BASE_ADDRESS = 0x00000000;
    const PAGE_SIZE = 64;
    const SECTOR_SIZE = 1024;

    var allow_code_protection = false;

    var message = function (msg) {
        console.log(msg);
        if (onMessageCallback) {
            onMessageCallback(msg);
        }
    };

    var send_command = async function (uart, command){
        // Send a command to the ISP and check that we receive and COMMAND_SUCCESS (0)
        // response.
        //
        // Note that this function assumes that ECHO is turned off.

        await uart.write(command + '\r\n');
        await uart.flush();
        var response = await uart.readline();
        if (response != '0\r\n') {
            throw 'ERROR: Command "' + command + '" failed. Return code: ' + response;
        }
    };

    var append_signature = function (bin){
        // Calculate the signature that the ISP uses to detect "valid code"

        // FIXME: Does JavaScript support 32 bit integers?

        var signature = 0;
        for (var i = 0 ; i < 8; i += 1) {
            var vector = i * 4;
            signature = signature + (
                (bin[vector + 3] << 24) +
                (bin[vector + 2] << 16) +
                (bin[vector + 1] << 8) +
                (bin[vector]));
        }
        signature = (signature ^ 0xffffffff) + 1;   // Two's complement

        var vector8 = 28;
        bin[vector8 + 3] = (signature >> 24) & 0xff;
        bin[vector8 + 2] = (signature >> 16) & 0xff;
        bin[vector8 + 1] = (signature >> 8) & 0xff;
        bin[vector8] = signature & 0xff;
    };

    var open_isp = async function (uart) {
        uart.open(115200, 8, 'n', 1);

        if (wait) {
            message('Waiting for LPC81x to enter ISP mode...');
        }

        for (;;) {
            await uart.flush();
            await uart.write('?');
            await uart.flush();

            var response = await uart.readline();
            if (response == 'Synchronized\r\n') {

                await uart.write('Synchronized\r\n');
                await uart.flush();
                await uart.readline();        // Discard echo

                response = await uart.readline();
                if (response != 'OK\r\n') {
                    throw 'ERROR: Expected "OK" after sending  "Synchronized", but received "' + response + '"';
                }

                // Send crystal frequency in kHz (always 12 MHz for the LPC81x)
                await uart.write('12000\r\n');
                await uart.flush();
                await uart.readline();        // Discard echo

                response = await uart.readline();
                if (response != 'OK\r\n') {
                    throw 'ERROR: Expected "OK" after sending crystal frequency, but received "' + response + '"';
                }

                await uart.write('A 0\r\n');  // Turn ECHO off
                await uart.flush();
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
                await uart.flush();
                await uart.readline();        // Discard echo

                response = await uart.readline();
                if (response != '1\r\n') {
                    if (! wait) {
                        throw 'ERROR: LPC81x not in ISP mode.';
                    }
                }
                else {
                    await uart.write('A 0\r\n');      // Turn ECHO off
                    await uart.flush();
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
                await uart.flush();

                response = await uart.readline();
                if (response == '1\r\n') {
                    await uart.setTimeout(5);
                    return;
                }
                if (! wait) {
                    throw 'ERROR: LPC81x not in ISP mode.';
                }
            }
        }
    };

    var program = async function (uart, bin) {
        // Write the given binary image file into the flash memory.

        // The image is checked whether it contains any of the code protection
        // values, and flashing is aborted (unless instructed with a flag)
        // so that we don't "brick" the ISP functionality.

        // Also the checksum of the vectors that the ISP uses to detect valid
        // flash is generated and added to the image before flashing.

        var used_sectors = (bin.length / SECTOR_SIZE);


        // Abort if the Code Read Protection in the image contains one of the
        // special patterns. We don't want to lock us out of the chip...
        if (!allow_code_protection) {
            var pattern = ((bin[CRP_ADDRESS + 3] << 24) + (bin[CRP_ADDRESS + 2] << 16) + (bin[CRP_ADDRESS + 1] << 8) + bin[CRP_ADDRESS]);

            if (pattern == NO_ISP) {
                throw 'ERROR: NO_ISP code read protection detected in image file';
            }

            if (pattern == CRP1) {
                throw 'ERROR: CRP1 code read protection detected in image file';
            }

            if (pattern == CRP2) {
                throw 'ERROR: CRP2 code read protection detected in image file';
            }

            if (pattern == CRP3) {
                throw 'ERROR: CRP3 code read protection detected in image file';
            }
        }


        // Calculate the signature that the ISP uses to detect "valid code"
        append_signature(bin);

        // Unlock the chip with the magic number
        await send_command(uart, 'U 23130');


        // Program the image
        for (let index = 0; index < used_sectors; index += 1) {
            if (onProgressCallback) {
                onProgressCallback(index / used_sectors);
            }

            let sector = index;

            // Erase the sector
            await send_command(uart, 'P ' + sector + ' ' + sector);
            await send_command(uart, 'E ' + sector + ' ' + sector);

            var address = sector * SECTOR_SIZE;
            var last_address = address + SECTOR_SIZE - 1;

            await send_command(uart, 'W ' + RAM_ADDRESS + ' ' + SECTOR_SIZE);
            await uart.write(bin.slice(address, last_address+1));
            await send_command(uart, 'P ' + sector + ' ' + sector);
            await send_command(uart, 'C ' + address + ' ' + RAM_ADDRESS + ' ' + SECTOR_SIZE);
        }

        if (onProgressCallback) {
            onProgressCallback(1.0);
        }
    };

    var reset_mcu = async function (uart) {
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

        await send_command(uart, 'W ' + RAM_ADDRESS + ' ' + reset_program.length);
        await uart.write(reset_program);
        await uart.flush();

        // Unlock the Go command
        await send_command(uart, 'U 23130');

        // Run the program from RAM. Note that this command does not respond with
        // COMMAND_SUCCESS as it directly executes.
        await uart.write('G ' + RAM_ADDRESS + ' T\r\n');
        await uart.flush();
    };

    var flash = async function (uart, bin) {
        if (busy) {
            message('Flashing already in progress');
            return;
        }

        busy = true;
        await open_isp(uart);

        message('Programming ...');
        await program(uart, bin);
        message('Starting the program...');
        await reset_mcu(uart);
        message('Done.');
        await uart.close();

        busy = false;
    };

    // *************************************************************************
    return {
        flash: flash,
        onMessageCallback: onMessageCallback,
        onProgressCallback: onProgressCallback,
    };
})();
