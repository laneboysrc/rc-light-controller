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

let uart = {
  'write': send_isp,
  'readline': readline,
  'expect': expect,
};

// function set_communications_interface(device) {
//   uart = device;
// }

async function send_command(string) {
  flush();
  await uart.write(string + '\r\n');
  await uart.expect('0\r\n');
}

async function isp_initialization_sequence() {
  let answer;

  log("Performing ISP handshake ...");
  flush();
  await uart.write('?')
  await uart.expect('Synchronized\r\n');
  progressCallback(0.04);
  await uart.write('Synchronized\r\n');
  await uart.expect('Synchronized\r\n');
  await uart.expect('OK\r\n');
  progressCallback(0.06);

  log("Sending crystal frequency ...");
  await uart.write('12000\r\n');
  await uart.expect('12000\r\n');
  await uart.expect('OK\r\n');

  log("Turning ECHO off ...");
  await uart.write('A 0\r\n');
  await uart.expect('A 0\r\n');
  await uart.expect('0\r\n');

  log("ISP ready");
  progressCallback(0.1);
}

// Read the Part ID from the MCU and decode it in human readable form.
async function read_part_id() {
  let part_id;
  let part_name = "unknown";

  const known_parts = {
    0x00008100: "LPC810M021FN8",
    0x00008110: "LPC811M001JDH16",
    0x00008120: "LPC812M101JDH16",
    0x00008121: "LPC812M101JD20",
    0x00008122: "LPC812M101JDH20, LPC812M101JTB16"
  }

  await send_command('J');
  part_id = await uart.readline('\r\n');
  part_id = parseInt(part_id.trim(), 10);

  if (part_id in known_parts) {
    part_name = known_parts[part_id];
  }

  return { part_id: part_id,  part_name: part_name };
}

// Obtain the size of the Flash memory from the LPC81x.
// If we are unable to identify the part we assume a default of 4 KBytes.
async function get_flash_size() {
  let part_id;

  const known_parts = {
    0x00008100: 4 * 1024,       // LPC810M021FN8
    0x00008110: 8 * 1024,       // LPC811M001JDH16
    0x00008120: 16 * 1024,      // PC812M101JDH16
    0x00008121: 16 * 1024,      // LPC812M101JD20
    0x00008122: 16 * 1024       // LPC812M101JDH20, LPC812M101JTB16
  };

  const part = await read_part_id();
  part_id = part.part_id;

  if (part_id in known_parts) {
    return known_parts[part_id];
  }

  console.warn('Unknown part ID ' + part_id + ', using 4 Kbytes');
  return 4 * 1024;
}

// Write the given binary image file into the flash memory.
//
// The image is checked whether it contains any of the code protection
// values, and flashing is aborted (unless instructed with a flag)
// so that we don't "brick" the ISP functionality.
//
// Also the checksum of the vectors that the ISP uses to detect valid
// flash is generated and added to the image before flashing.
async function isp_program(bin) {
  const used_sectors = bin.length / SECTOR_SIZE;

  // Abort if the Code Read Protection in the image contains one of the
  // special patterns. We don't want to lock us out of the chip...
  if (!allow_code_protection) {
    let pattern = ((bin[CRP_ADDRESS + 3] << 24) + (bin[CRP_ADDRESS + 2] << 16) + (bin[CRP_ADDRESS + 1] << 8) + bin[CRP_ADDRESS]);

    if (pattern == NO_ISP) {
      log('ERROR: NO_ISP code read protection detected in image file', 'fail');
      throw 'ERROR: NO_ISP code read protection detected in image file';
    }

    if (pattern == CRP1) {
      log('ERROR: CRP1 code read protection detected in image file', 'fail');
      throw 'ERROR: CRP1 code read protection detected in image file';
    }

    if (pattern == CRP2) {
      log('ERROR: CRP2 code read protection detected in image file', 'fail');
      throw 'ERROR: CRP2 code read protection detected in image file';
    }

    if (pattern == CRP3) {
      log('ERROR: CRP3 code read protection detected in image file', 'fail');
      throw 'ERROR: CRP3 code read protection detected in image file';
    }
  }

  // Calculate the signature that the ISP uses to detect "valid code"
  append_signature(bin);

  log("Unlocking the programming functions ...");

  // Unlock the chip with the magic number
  await send_command('U 23130');

  // Erase the whole chip
  log('Erasing the flash memory ...');
  progressCallback(0.15);
  await send_command('P 0 15');
  await send_command('E 0 15');

  log('Program the firmware image ...');
  for (let index = 0; index < used_sectors; index += 1) {
    progressCallback(0.2 + (0.8 * index / used_sectors));

    let sector = index;

    let address = sector * SECTOR_SIZE;
    let last_address = address + SECTOR_SIZE - 1;

    let data = bin.slice(address, last_address+1);

    await send_command('W ' + RAM_ADDRESS + ' ' + data.length);
    await uart.write(new Uint8Array(data));
    await send_command('P ' + sector + ' ' + sector);
    await send_command('C ' + address + ' ' + RAM_ADDRESS + ' ' + SECTOR_SIZE);
  }

  progressCallback(1.0);
  log('Programming done');
}

/*
Reset the MCU to start the application.
We do that by downloading a small binary into RAM. This binary corresponds
to the following C code:

    SCB->AIRCR = 0x05FA0004;

This code resets the ARM CPU by setting SYSRESETREQ. We load this
isp_program into RAM and run it with the "Go" command.
*/
async function isp_reset_mcu() {
  const reset_program = new Uint8Array([
    0x01, 0x4a, 0x02, 0x4b, 0x1a, 0x60, 0x70, 0x47,
    0x04, 0x00, 0xfa, 0x05, 0x0c, 0xed, 0x00, 0xe0
  ]);

  await send_command('W ' + RAM_ADDRESS + ' ' + reset_program.length);
  await uart.write(reset_program);

  // Unlock the Go command
  await send_command('U 23130');

  // Run the isp_program from RAM. Note that this command does not respond with
  // COMMAND_SUCCESS as it directly executes.
  await uart.write('G ' + RAM_ADDRESS + ' T\r\n');
}

// Calculate the signature that the ISP uses to detect "valid code"
function append_signature(bin) {
  let signature = 0;
  const vector8 = 28;

  for (let i = 0 ; i < 7; i += 1) {
    let vector = i * 4;
    signature = signature + (
      (bin[vector + 3] << 24) +
      (bin[vector + 2] << 16) +
      (bin[vector + 1] << 8) +
      (bin[vector]));
  }
  signature = (signature ^ 0xffffffff) + 1;   // Two's complement

  bin[vector8 + 3] = (signature >> 24) & 0xff;
  bin[vector8 + 2] = (signature >> 16) & 0xff;
  bin[vector8 + 1] = (signature >> 8) & 0xff;
  bin[vector8] = signature & 0xff;
}
