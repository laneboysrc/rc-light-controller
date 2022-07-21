class flash_lpc8xx {
  constructor(uart) {
    this.uart = uart;

    this.progressCallback = null;
    this.messageCallback = null;

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

  _doProgressCallback(value) {
    if (this.progressCallback) {
      this.progressCallback(value);
    }
  }

  _doMessageCallback(value) {
    if (this.messageCallback) {
      this.messageCallback(value);
    }
  }

  _make_crlf_visible(string) {
    return string.replace(/\r/g, '\\r').replace(/\n/g, '\\n');
  }

  // Calculate the signature that the ISP uses to detect "valid code"
  _append_signature(bin) {
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

  async _expect(expected_string) {
    const answer = await this.uart.readline('\r\n');
    if (answer != expected_string) {
      throw('Expected "' + _make_crlf_visible(expected_string) + '", got "' + _make_crlf_visible(answer));
    }
    return answer;
  }

  _merge_uint8arrays(uint8array_list) {
    // Get the total length of all arrays
    let length = 0;
    uint8array_list.forEach(item => {
      length += item.length;
    });

    // Create a new array with total length and merge all source arrays
    let merged_array = new Uint8Array(length);
    let offset = 0;
    uint8array_list.forEach(item => {
      merged_array.set(item, offset);
      offset += item.length;
    });

    return merged_array;
  }

  async send_command(string) {
    this.uart.flush();
    await this.uart.write(string + '\r\n');
    await this._expect('0\r\n');
  }

  async initialization_sequence() {
    let answer;

    this._doMessageCallback("Performing ISP handshake ...");
    this.uart.flush();
    await this.uart.write('?')
    await this._expect('Synchronized\r\n');
    this._doProgressCallback(0.04);
    await this.uart.write('Synchronized\r\n');
    await this._expect('Synchronized\r\n');
    await this._expect('OK\r\n');
    this._doProgressCallback(0.06);

    this._doMessageCallback("Sending crystal frequency ...");
    await this.uart.write('12000\r\n');
    await this._expect('12000\r\n');
    await this._expect('OK\r\n');

    this._doMessageCallback("Turning ECHO off ...");
    await this.uart.write('A 0\r\n');
    await this._expect('A 0\r\n');
    await this._expect('0\r\n');

    this._doMessageCallback("ISP ready");
    this._doProgressCallback(0.1);
  }

  // Read the Part ID from the MCU and decode it in human readable form.
  async read_part_id() {
    let part_id;
    let part_name = "unknown";

    const known_parts = {
      0x00008100: "LPC810M021FN8",
      0x00008110: "LPC811M001JDH16",
      0x00008120: "LPC812M101JDH16",
      0x00008121: "LPC812M101JD20",
      0x00008122: "LPC812M101JDH20, LPC812M101JTB16",
      0x00008322: "LPC832M101FDH20",
      0x00008341: "LPC8341201FHI33",
      0x00008241: "LPC824M201JHI33",
      0x00008221: "LPC822M101JHI33",
      0x00008242: "LPC824M201JDH20",
      0x00008222: "LPC822M101JDH20"
    }

    await this.send_command('J');
    part_id = await this.uart.readline('\r\n');
    part_id = parseInt(part_id.trim(), 10);

    if (part_id in known_parts) {
      part_name = known_parts[part_id];
    }

    return { part_id: part_id,  part_name: part_name };
  }

  // Obtain the size of the Flash memory from the LPC81x.
  // If we are unable to identify the part we assume a default of 4 KBytes.
  async get_flash_size() {
    let part_id;

    const known_parts = {
      0x00008100: 4 * 1024,       // LPC810M021FN8
      0x00008110: 8 * 1024,       // LPC811M001JDH16
      0x00008120: 16 * 1024,      // LPC812M101JDH16
      0x00008121: 16 * 1024,      // LPC812M101JD20
      0x00008122: 16 * 1024,      // LPC812M101JDH20, LPC812M101JTB16
      0x00008322: 16 * 1024,      // LPC832M101FDH20
      0x00008341: 32 * 1024,      // LPC8341201FHI33
      0x00008241: 32 * 1024,      // LPC824M201JHI33
      0x00008221: 16 * 1024,      // LPC822M101JHI33
      0x00008242: 32 * 1024,      // LPC824M201JDH20
      0x00008222: 16 * 1024       // LPC822M101JDH20
    };

    const part = await this.read_part_id();
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
  async program(bin) {
    const used_sectors = bin.length / this.SECTOR_SIZE;

    // Abort if the Code Read Protection in the image contains one of the
    // special patterns. We don't want to lock us out of the chip...
    if (!this.allow_code_protection) {
      let pattern = ((bin[this.CRP_ADDRESS + 3] << 24) + (bin[this.CRP_ADDRESS + 2] << 16) + (bin[this.CRP_ADDRESS + 1] << 8) + bin[this.CRP_ADDRESS]);

      if (pattern == this.NO_ISP) {
        this._doMessageCallback('ERROR: NO_ISP code read protection detected in image file', 'fail');
        throw 'ERROR: NO_ISP code read protection detected in image file';
      }

      if (pattern == this.CRP1) {
        this._doMessageCallback('ERROR: CRP1 code read protection detected in image file', 'fail');
        throw 'ERROR: CRP1 code read protection detected in image file';
      }

      if (pattern == this.CRP2) {
        this._doMessageCallback('ERROR: CRP2 code read protection detected in image file', 'fail');
        throw 'ERROR: CRP2 code read protection detected in image file';
      }

      if (pattern == this.CRP3) {
        this._doMessageCallback('ERROR: CRP3 code read protection detected in image file', 'fail');
        throw 'ERROR: CRP3 code read protection detected in image file';
      }
    }

    // Calculate the signature that the ISP uses to detect "valid code"
    this._append_signature(bin);

    this._doMessageCallback("Unlocking the programming functions ...");

    // Obtain the flash size and determine the last sector number for the
    // prepare and erase command
    const flash_size = await this.get_flash_size();
    const last_sector = (flash_size / this.SECTOR_SIZE) - 1;

    // Unlock the chip with the magic number
    await this.send_command('U 23130');

    // Erase the whole chip
    this._doMessageCallback('Erasing the flash memory ...');
    this._doProgressCallback(0.15);
    await this.send_command('P 0 ' + last_sector);
    await this.send_command('E 0 ' + last_sector);

    this._doMessageCallback('Program the firmware image ...');
    for (let index = 0; index < used_sectors; index += 1) {
      this._doProgressCallback(0.2 + (0.8 * index / used_sectors));

      let sector = index;

      let address = sector * this.SECTOR_SIZE;
      let last_address = address + this.SECTOR_SIZE - 1;

      let data = bin.slice(address, last_address+1);

      await this.send_command('W ' + this.RAM_ADDRESS + ' ' + data.length);
      await this.uart.write(new Uint8Array(data));
      await this.send_command('P ' + sector + ' ' + sector);
      await this.send_command('C ' + address + ' ' + this.RAM_ADDRESS + ' ' + this.SECTOR_SIZE);
    }

    this._doProgressCallback(1.0);
    this._doMessageCallback('Programming done');
  }

  async read() {
    let image_data = new Uint8Array();
    let block;

    // The read command return "0<CR><LF>" as acknowledgement before sending
    // the flash data. Therefore we need to read an additional 3 bytes, and
    // strip those bytes off the read data.
    const ack_size = 3;

    this._doProgressCallback(0);

    let flash_size = await this.get_flash_size();
    this._doMessageCallback('Reading ' + flash_size + ' bytes ...');
    this._doProgressCallback(0.1);

    // WORK-AROUND
    // ===========
    //
    // In theory we should be able to read the whole firmware in one go
    // and have the underlying UART implementation (could be a serial port, or
    // WebUSB) handle the segmentation.
    //
    // Unfortunately due to a bug in the WebUSB programmer firmware when
    // the Programmer reads large amount of data from the MCU and sends it
    // back via USB we miss some bytes quite frequently.
    //
    // To work around, we can read the firmware in smaller chunks. However,
    // even with chunks as small as 16 Bytes we get occasional errors.
    // The smaller the chunk size, the less frequent errors occur, but also
    // transfer speed gets reduced.
    //
    // When bytes are missing, the WebUSB programmer does not receive the
    // expected amount of bytes, and the UART read function creates a timeout
    // error.
    //
    // This is an indication here that something failed, and we re-read the
    // same block from the MCU until it succeeds. Reading chunks of 256 bytes
    // is a reasonable compromise between error frequency and read speed.
    const read_size = 256;
    let address = this.FLASH_BASE_ADDRESS;
    while (address < (this.FLASH_BASE_ADDRESS + flash_size)) {
      await this.send_command('R ' + address + ' ' + read_size);

      try {
        // Use a timeout of approx 500 ms
        block = await this.uart.read(read_size + ack_size, 5);
      }
      catch(e) {
        // Timeout error, re-read the block
        console.error(e);
        continue;
      }

      image_data = this._merge_uint8arrays([image_data, block.slice(ack_size)]);
      this._doProgressCallback(0.1 + (0.9 * address / flash_size));
      address += read_size;
    }
    if (image_data.length !== flash_size) {
        throw 'Failed to read the whole Flash memory';
    }

    this._doProgressCallback(1.0);
    return image_data;
  }

  /*
  Reset the MCU to start the application.
  We do that by downloading a small binary into RAM. This binary corresponds
  to the following C code:

      SCB->AIRCR = 0x05FA0004;

  This code resets the ARM CPU by setting SYSRESETREQ. We load this
  isp_program into RAM and run it with the "Go" command.
  */
  async reset_mcu() {
    const reset_program = new Uint8Array([
      0x01, 0x4a, 0x02, 0x4b, 0x1a, 0x60, 0x70, 0x47,
      0x04, 0x00, 0xfa, 0x05, 0x0c, 0xed, 0x00, 0xe0
    ]);

    await this.send_command('W ' + this.RAM_ADDRESS + ' ' + reset_program.length);
    await this.uart.write(reset_program);

    // Unlock the Go command
    await this.send_command('U 23130');

    // Run the isp_program from RAM. Note that this command does not respond with
    // COMMAND_SUCCESS as it directly executes.
    await this.uart.write('G ' + this.RAM_ADDRESS + ' T\r\n');
  }

  set onMessageCallback(fn) {
    this.messageCallback = fn;
  }

  set onProgressCallback(fn) {
    this.progressCallback = fn;
  }

}
