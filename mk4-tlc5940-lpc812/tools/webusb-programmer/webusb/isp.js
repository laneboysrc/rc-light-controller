'use strict';

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

const allow_code_protection = false;


async function isp_initialization_sequence() {
  let answer;

  log("Power-cycling the light controller ...");
  await send_set_command(CMD_DUT_POWER_OFF);
  await send_set_command(CMD_OUT_ISP_LOW);
  await delay(500);
  await send_set_command(CMD_DUT_POWER_ON);
  await delay(100);
  progressCallback(0.02);

  log("Performing ISP handshake ...");
  flush();
  await send_isp('?')
  await expect('Synchronized\r\n');
  progressCallback(0.04);
  await send_isp('Synchronized\r\n');
  await expect('Synchronized\r\n');
  await expect('OK\r\n');
  progressCallback(0.06);

  log("Sending crystal frequency ...");
  await send_isp('12000\r\n');
  await expect('12000\r\n');
  await expect('OK\r\n');

  log("Turning ECHO off ...");
  await send_isp('A 0\r\n');
  await expect('A 0\r\n');
  await expect('0\r\n');

  log("ISP ready");
  await send_set_command(CMD_OUT_ISP_TRISTATE);
  progressCallback(0.1);
}

async function isp_program(bin) {
  // Write the given binary image file into the flash memory.

  // The image is checked whether it contains any of the code protection
  // values, and flashing is aborted (unless instructed with a flag)
  // so that we don't "brick" the ISP functionality.

  // Also the checksum of the vectors that the ISP uses to detect valid
  // flash is generated and added to the image before flashing.
  const used_sectors = bin.length / SECTOR_SIZE;

  // Abort if the Code Read Protection in the image contains one of the
  // special patterns. We don't want to lock us out of the chip...
  if (!allow_code_protection) {
    let pattern = ((bin[CRP_ADDRESS + 3] << 24) + (bin[CRP_ADDRESS + 2] << 16) + (bin[CRP_ADDRESS + 1] << 8) + bin[CRP_ADDRESS]);

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

  log("Unlocking the programming functions ...");

  // Unlock the chip with the magic number
  await send_isp_command('U 23130');

  // Erase the whole chip
  log('Erasing the flash memory ...');
  progressCallback(0.15);
  await send_isp_command('P 0 15');
  await send_isp_command('E 0 15');

  log('Program the firmware image ...');
  for (let index = 0; index < used_sectors; index += 1) {
    progressCallback(0.2 + (0.8 * index / used_sectors));

    let sector = index;

    let address = sector * SECTOR_SIZE;
    let last_address = address + SECTOR_SIZE - 1;

    let data = bin.slice(address, last_address+1);

    await send_isp_command('W ' + RAM_ADDRESS + ' ' + data.length);
    await send_isp_data(new Uint8Array(data));
    await send_isp_command('P ' + sector + ' ' + sector);
    await send_isp_command('C ' + address + ' ' + RAM_ADDRESS + ' ' + SECTOR_SIZE);
  }

  progressCallback(1.0);
  log('Programming done');
}

async function isp_reset_mcu() {
  /*
  Reset the MCU to start the application.
  We do that by downloading a small binary into RAM. This binary corresponds
  to the following C code:

      SCB->AIRCR = 0x05FA0004;

  This code resets the ARM CPU by setting SYSRESETREQ. We load this
  isp_program into RAM and run it with the "Go" command.
  */
  const reset_program = new Uint8Array([
      0x01, 0x4a, 0x02, 0x4b, 0x1a, 0x60, 0x70, 0x47,
      0x04, 0x00, 0xfa, 0x05, 0x0c, 0xed, 0x00, 0xe0]);

  await send_isp_command('W ' + RAM_ADDRESS + ' ' + reset_program.length);
  await send_isp_data(reset_program);

  // Unlock the Go command
  await send_isp_command('U 23130');

  // Run the isp_program from RAM. Note that this command does not respond with
  // COMMAND_SUCCESS as it directly executes.
  await send_isp('G ' + RAM_ADDRESS + ' T\r\n');
}

function append_signature(bin) {
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

  const vector8 = 28;
  bin[vector8 + 3] = (signature >> 24) & 0xff;
  bin[vector8 + 2] = (signature >> 16) & 0xff;
  bin[vector8 + 1] = (signature >> 8) & 0xff;
  bin[vector8] = signature & 0xff;
}