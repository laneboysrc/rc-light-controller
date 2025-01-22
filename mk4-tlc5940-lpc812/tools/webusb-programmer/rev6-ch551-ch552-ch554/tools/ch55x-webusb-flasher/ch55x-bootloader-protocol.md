# WCH CH55x Bootloader Protocol

This document describes the API of the bootloader that ships with CH55x microcontrollers.

There is no official documentation from WCH on the bootloader protocol that I am aware of.

The information in this document was created based on open source 3rd party CH55x flashing programs (e.g. chprog https://github.com/ole00/chprog), and forums where people exctacted and reverse engineered the bootloader binary (mikrocontroller.net [https://www.mikrocontroller.net/topic/462538](https://www.mikrocontroller.net/topic/462538), in German).

--- ---------------------------------------------------------------------------

## Bootloader revisions

Three bootloader versions have been found in CH55x devices shipped to customers: v1, v2.3.1 and v2.4.0

The v2 versions are mostly compatible, but the v1 version design is very different from the v2 versions. Even worse, the same command value has a different meaning on v1 and v2.


--- ---------------------------------------------------------------------------

## v2 protocol

The v2 protocol uses USB BULK transfers on Endpoint 2. The programming software sends commands on the OUT Endpoint, and each command has a response on the IN Endpoint. Command and Response can have a maximum of 64 bytes as the Endpoint is configured with a buffer size of 64 bytes and they must fit into one USB BULK transfer.


### Commands

The structure of a command is as follows:

    <cmd> <len> <??> <data> ...

**cmd**: Command code requesting a specific operation

**len**: Number of *data* bytes

**??**: Not used by the bootloader. The original intention is unknown. Most programming software sends a value of 0x00 in this field.

The length field is only used by the bootloader in a few commands. While not strictly necessary for the bootloader versions that are shipping on CH55x microcontrollers, it is advisable to set the length field correctly for *all* commands.

For many commands the bootloader assumes a fixed command length. If less data is sent than the bootloader expects, then the remaining bytes may be residual data from previous communications.

The v2 bootloader has flags that determine the behaviour of some commands. Other commands control those flags.

All v2 bootloaders have a *write-protect flag* that is set when the bootloader starts and can only be cleared with the *0xa4 erase code flash* command. When the write-protect flag is set, the command *0xa5 write code flash* will fail.

The *verify-error flag* is only present on bootloader version v02.40. The flag is cleared when the bootloader starts. It gets set when a *0xa6 verify code flash* operation fails. Software can clear the flag by executing *0xa4 erase code flash page*.

> Discussion: most likely the verify-error flag has been introduced after an attack was published where the code flash could be read out using the verify command. Since we know the code of the bootloader, we can start verifying at the bootloader and work our way backwards through the code flash byte by byte, trying each possibly byte value until verify succeeds. In bootloader v02.40 this method would require "rebooting" the chip into the bootloader after each failed verify attempt, which requires special hardware and slows the attack down.


### Responses

Every command triggers a response with the following structure:

    0        1        2        3        4        5 ..
    <cmd>    <??>     <len>    <0x00>   <result> <data> ...

**cmd**: Command code that the response belongs to

**??**: Not set by the bootloader. Will contain random data and should be ignored.

**len**: Number of payload bytes. Includes *result* and *data* bytes.

**0x00**: This value is always 0x00

All responses return a *result* that usually indicates success or failure.


### 0xa1 - identify

              0     1     2     3     4     5 .. 20
    command:  0xa1  len   ??    ??    ??    "MCU ISP & WCH.CN"

              0     1     2     3     4     5
    response: 0xa1  ??    0x02  0x00  id    0x11      (success)
              0xa1  ??    0x02  0x00  0x00  0xf1      (failure)

Byte 3, 4 and 5 of the command are unused; we recommend sending 0x00 as value.

When the MCU receives this command, it checks bytes 5..21 whether they contain a special ASCII string. If the string matches "MCU ISP & WCH.CN" then the response holds the chip ID in BCD (e.g. 0x52 for CH552) and the value 0x11, which indicates success.
In case of mismatch, the response sends 0x00 as chip ID and 0xf1 as failure indication.


### 0xa2 - boot control
              0     1     2     3
    command:  0xa2  0x01  ??    val

              0     1     2     3     4     5
    response: 0xa2  ??    0x02  0x00  res   ??

If *val* (byte 3) is 0x01 then a software reset is executed. The CH55x immediately disconnects so no response will be received.

Any other value of *val* causes a write-protect flag for the code flash to be set.


### 0xa3 - set transport key

              0     1     2     3 .. 63
    command:  0xa3  len   ??    key

              0     1     2     3     4     5
    response: 0xa3  ??    0x02  0x00  sum   ??     (success)
              0xa3  ??    0x02  0x00  0xfe  ??     (fail, len was less than 30)

The *0xa5 write code flash* and *0xa6 verify code flash* commands transmit the code in a scrambled form. The key used for scrambling is 8 bytes long and can be set by the host with the command *0xa3 set transport key*.

After reset, the transport key is initialized using the rand() function of the compiler. Given that the CH55x does not have any sources of entropy, it is most likely that the rand() function always returns the same pseudo-random sequence after startup. However, since a host can not know exactly which compiler was used to build the bootloader, it can not rely on knowing the transport key. A host must therefore always set a key itself.

While the key is only 8 bytes long, the bootloader uses yet another scrambling method of obtaining the transport key bytes out of at least 30 bytes of data that the host must send when using the *0xa3 set transport key* command.

The following algorithm is used by the bootloader:

    // Note: snSum is either "0" or the sum of the 4 bytes of the
    // unique serial number, depending on whether the
    // "read serial and calculate snsum" operation was issued or not.
    // See description of command "0xa7 - read config data"

    transport_key[0] = key[(len / 7) * 4] ^ snSum;
    transport_key[1] = key[(len / 5) * 1] ^ snSum;
    transport_key[2] = key[(len / 7) * 1] ^ snSum;
    transport_key[3] = key[(len / 7) * 6] ^ snSum;
    transport_key[4] = key[(len / 7) * 3] ^ snSum;
    transport_key[5] = key[(len / 5) * 3] ^ snSum;
    transport_key[6] = key[(len / 7) * 5] ^ snSum;
    transport_key[7] = transport_key[0] + CHIP_ID;

All divisions are integer divisions.

When the *len* field of the command is less than 30, the bootloader will resond with *sum* being the value of *0xfe* (error). The bootkey is not changed in that case.

If the bootkey was successfully extracted, then the bootloader responds with *sum* being the sum of all eight *transport_key* bytes. Note that in theory this value could be *0xfe*, so be careful with error handling.

>Discussion: It is unclear what this scrambling protects against. If an attacker has access to the whole data transfer between the host and the CH55x, the attacker can easily re-calculate the transport key and unscramble the firmware image.


### 0xa4 - erase code flash page

              0     1     2     3
    command:  0xa4  0x01  ??    page

              0     1     2     3     4     5
    response: 0xa4  ??    0x02  0x00  0x00  ??     (success)
              0xa4  ??    0x02  0x00  0x40  ??     (fail: write error; should never happen)
              0xa4  ??    0x02  0x00  0xfe  ??     (fail: page < 8)

This command is supposed to erase a page of 2 Kbytes of the code flash. Note that on the CH55x *erasing* actually means *writing 0xff* into the flash memory.

**This function does not work at all.** The page number has to be 8 or greater, otherwise error *0xfe* will be returned and the write-protect flag will not be cleared. Regardless of which page number is given, this function  clears every 2nd word (!?!) of the first 4 Kbytes (v02.31) or 8 Kbytes (v02.40) of flash memory.

As side effect this function clears the write-protect flag. It is therefore necessary to call this function with *page* set to *8* before using the *0xa5 write code flash*  function.


### 0xa5 - write code flash

              0     1     2     3     4     5     6     7     8 ...
    command:  0xa5  len   ??    adrL  adrH  ??    ??    ??    data

              0     1     2     3     4     5
    response: 0xa5  ??    0x02  0x00  0x00  ??        (success)
              0xa5  ??    0x02  0x00  0x40  ??        (write operation failed)
              6 bytes, data from a previous response  (write protect enabeled)

Writes an even number of bytes starting at code flash address *adrH/adrL*. *adrH/adrL* must point to an even address.

This function only succeeds if the bootloader write-protect flag has been cleared by an earlier call to the *0xa4 erase code flash page* function.

*data* must be scrabled with the transport key (see *0xa3 set transport key*) as follows:

    for (i = 0; i < (len - 5); i++) {
        data[i] = data[i] ^ transport_key[i & 0x07];
    }


### 0xa6 - verify code flash

              0     1     2     3     4     5     6     7     8 ...
    command:  0xa6  len   ??    adrL  adrH  ??    ??    ??    data

              0     1     2     3     4     5
    response: 0xa6  ??    0x02  0x00  0x00  ??        (success)
              0xa6  ??    0x02  0x00  0xf5  ??        (verification failed)
              0xa6  ??    0x02  0x00  0xfe  ??        (len < 8, or a previous verify command failed)

Verifies that the code flash at address *adrH/adrL* match *data*.

At least 8 bytes must be verified at once.

The scrambling method for *data* is the same as for command *0xa5 write code flash*.


### 0xa7 - read config data

              0     1     2     3
    command:  0xa7  0x01  ??    cfg
              0xa7  0x01  ??    0x07        read configuration space at 0x3ff0
              0xa7  0x01  ??    0x08        read bootloader version
              0xa7  0x01  ??    0x10        read serial and calculate snsum
              0xa7  0x01  ??    0x1f        read all of the obove

              0     1     2     3     4     5     6
    response: 0xa7  ??    len   0x00  cfg   0x00  data  ...

The response *data* depends on which bits in *cfg* are set.

When bits *0* to *2* are set (`cfg & 0x07 == 0x07`) then the 10 configuration bytes at address *0x3ff0* are returned.

When bit *3* is set (`cfg & 0x08 == 0x08`) then 4 bytes of the bootloader version are returned. The bytes *0x00 0x02 0x03 0x01* would mean bootloader version *v02.31*.

When bit *4* is set (`cfg & 0x10 == 0x10`) then 4 bytes of the chip unique serial number are returned, plus another 4 bytes that hold random data.
In addition, the following side-effect is happening: The bootloader sums up the 4 bytes of the unique serial number and uses this value from now to XOR the transport key bytes before descrambling. After reset, before issuing this command this sum value is 0x00 (XOR has no effect).

When *cfg* is a combination of those patterns, then the data is simple appended. For example, if *cfg* is *0x0f*, then 10 configuration bytes followed by 4 bootloader bytes are returned.


### 0xa8 - write config data

              0     1     2     3     4     5 .. 14
    command:  0xa8  12    ??    cfg   ??    data ...

              0     1     2     3     4     5     6
    response: 0xa8  ??    0x02  0x00  0x00  ??        (success)
              0xa8  ??    0x02  0x00  0xfe  ??        (fail: cfg[2..0] is not 0x07)
              0xa8  ??    0x02  0x00  0x40  ??        (write operation failed)

Writes 10 bytes of data to the configuration space starting at *0x3ff0*. The lowest 3 bits of *cfg* must be set to *0x07* for this function to operate.


### 0xa9 - erase data flash

              0     1     2
    command:  0xa9  0x00  ??

              0     1     2     3     4     5
    response: 0xa9  ??    0x02  0x00  0x00  ??     (success)
              0xa9  ??    0x02  0x00  0x40  ??     (write operation failed)

Erases all 128 bytes in the data flash by writing *0xff* to them.


### 0xaa - write data flash

              0     1     2     3     4     5     6     7     8 ...
    command:  0xaa  len   ??    adrL  adrH  ??    ??    ??    data

              0     1     2     3     4     5
    response: 0xa9  ??    0x02  0x00  0x00  ??     (success)
              0xa9  ??    0x02  0x00  0x40  ??     (write operation failed)

Writes a number of bytes starting at data flash address adrH/adrL.

*data* must be scrabled with the transport key (see *0xa3 set transport key*) as follows:

    for (i = 0; i < (len - 5); i++) {
        data[i] = data[i] ^ transport_key[i & 0x07];
    }


### 0xab - read data flash

              0     1     2     3     4     5     6     7
    command:  0xab  0x05  ??    adr   ??    ??    ??    rlen

              0     1     2     3     4     5     6
    response: 0xa9  ??    len   0x00  0x00  ??    data ...

Returns *rlen* bytes of the data flash starting at address *adr* (low byte of the data flash address).

Note that there are no safety checks, ensure that *adr[0..rlen-1]* is valid and that *rlen* is less than 56 (to fit into the 64 bytes endpoint buffer).


--- ---------------------------------------------------------------------------

## V1 protocol

The V1 protocol uses USB BULK transfers on Endpoint 2. The programming software sends commands on the OUT Endpoint, and each command has a response on the IN Endpoint. Command and Response can have a maximum of 64 bytes as they must fit into one USB BULK transfer and the Endpoint is configured with a buffer size of 64 bytes.

> Important: the v1 protocol has not been verified by implementation; it is based on assembler files available for the bootloader. Act accordingly!


### Commands

The structure of a command is as follows:

    <cmd> <len> <data> ...

**cmd**: Command code requesting a specific function

**len**: Number of *data* bytes that follow


### Responses

    0        1        2 ..
    <result> <len>    <data> ...

**result**: The result code after executing the command

**len**: Number of *data* bytes.


### 0xa2 - identify

              0     1    2 .. 20
    command:  0xa2  19   "USB DBG CH559 & ISP"

              0     1
    response: id    0x00

When the MCU receives this command, it checks bytes 2..20 whether they contain a special ASCII string. If the string matches "USB DBG CH559 & ISP" then the response holds the chip ID in BCD (e.g. 0x52 for CH552).
In case of mismatch, the response sends *0xff* as result.


### 0xa5 - run mode

              0     1    2     3
    command:  0xa5  2    runL  runH

Exit the bootloader and soft-reboot the MCU, but only if *runL/runH* is not *0x0000*.


### 0xa6 - set transport key

              0     1    2 .. 5
    command:  0xa6  4    key

              0     1
    response: 0x00  0x00

The *0xa8 write code flash* and *0xa7 verify code flash* commands transmit the code in a scrambled form. The key used for scrambling is 4 bytes long and can be set by the host with the command *0xa6 set transport key*.

After reset, the transport key is initialized using the ASCII characters *Twch*.


### 0xa7 - verify code flash

              0     1     2     5     6 ...
    command:  0xa7  len   adrL  adrH  data

              0     1
    response: 0x00  0x00    (success)
              0xff  0x00    (verify failed)

*len* is the number of data bytes to verify (excluding adrL/adrH). *len* must be a multiple of 4 bytes.

The data is unscrambled as follows:

    uint16_t address = (adrH << 8) | adrL;
    for (i = 0; i < (len); i++) {
        uint8_t unscrambled = data[i] ^ key[i & 0x03];
        // compare unscrambled with byte at *address
        ++address;
    }


### 0xa8 - write code flash

              0     1     2     3     4 ..
    command:  0xa8  len   adrL  adrH  data

              0     1
    response: 0x00  0x00      (success)
              0xff  0x00      (fail)

*len* is the number of data bytes to verify (excluding adrL/adrH). *len* must be a multiple of 4 bytes.

The data is unscrambled as follows:

    uint16_t address = (adrH << 8) | adrL;
    for (i = 0; i < (len); i+=2) {
        uint16_t unscrambled =
            (data[i + 1] ^ transport_key[((i>>1) + 1) & 0x03]) << 8 |
             data[i]     ^ transport_key[ (i>>1)      & 0x03];
        result = write_code_flash(address, unscrambled);
        address += 2;
    }

This function also returns *0x00 (success)* when the write-protect bit is set and no bytes have been actually written.


### 0xa9 - erase code flash page

              0     1     2     3
    command:  0xa9  0x02  adrL  adrH

              0     1
    response: 0x00  0x00      (success)
              0xff  0x00      (fail)

This command erase 1 Kbytes of the code starting at address *adrL/adrH*.

This function clears the write-protect bit when succesful.


### 0xb5 - erase data flash

              0     1     2
    command:  0xb5  0x01  adrL

              0     1
    response: 0x00  0x00      (success)
              0xff  0x00      (fail)

Erases the data flash starting at *adrL* until the end (128th byte) by writing *0xff* to them.


### 0xb6- write data flash

              0     1     2     3     4 ..
    command:  0xb6  len   adrL  adrH  data

              0     1
    response: 0x00  0x00      (success)
              0xff  0x00      (fail)

Writes a number of bytes starting at data flash address *adrL*. *adrH* is not used.

*data* must be scrabled with the transport key (see *0xa3 set transport key*) as follows:

    uint16_t address = DATA_FLASH_ADDR | adrL;
    for (i = 0; i < (len); i++) {
        uint8_t unscrambled = data[i] ^ key[i & 0x03];
        // compare unscrambled with byte at *address
        ++address;
    }


### 0xb7 - read data flash

              0     1
    command:  0xb7  adrL

              0     1     2 ...
    response: 0x00  62   data

Reads the data flash starting at *adrL*. Fills the 64 bytes endpoint buffer with data bytes until its full.


### 0xb8 - write config data

              0     1     2     3
    command:  0xb8  0x02  cfgL  cfgH

              0     1
    response: 0x00  0x00      (success)
              0xff  0x00      (fail)

Write cfgL/cfgH to the word at address *0x3ff8*.


### 0xb9 - read config data

              0     1
    command:  0xb0  0x00

              0     1     2 ...
    response: 0x00  0x08  data

Reads the 8 configuration bytes starting at address 0x3ff8. This data includes the unique serial number.


### 0xba - write bootloader config

              0     1     2     3     4     5     6     7     8     9
    command:  0xba  4     cfg1L cfg1H cfg2L cfg2H
    command:  0xba  8     cfg1L cfg1H cfg2L cfg2H ??    ??    cfg3L cfg3H

              0     1
    response: 0x00  0x00      (success)
              0xff  0x00      (fail)

Flashes *cfg1L/cfg1H* to *0x3ff4* if *cfg2L/cfg2H* (not a typo!) was changed.
Flashes *cfg2L/cfg2H* to *0x3ff6* if *cfg2L/cfg2H* was changed.
Flashes *cfg3L/cfg3H* to *0x3ff8* if *cfg3L/cfg3H* was changed.


### 0xbb - detect

              0     1
    command:  0xbb  0x00

              0     1
    response: 0x11  0x00

This command always returns 0x11 and *len* being 0. It can be used to detect the v1 bootloader as on v2 bootloders this command is not implemented, and v2 bootloaders return the *cmd* number (*0xbb* in this case) as first byte.
