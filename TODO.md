# Ideas regarding adding WS2812 support to Configurator

* Opcodes
    * 0x08..0x0f and 0x3a..0x3f are free
    * `extern-leds-set <immediate>`
        * The `immediate` value is the `PC` of the data to set, similar to `GOTO`
        * We do not use `leds-extern = <immediate>` as this would be a special case for all the other `=` and `+=` usage
    * `extern-leds-add <immediate>`
    - `extern-leds-count <immediate>` to define how many bytes to process
    * `data <immediate< <immediate> <immediate> <immediate>` to store 4 bytes
        * Note that an RGB LED counts as 3 LEDs
        * While this is inconvenient for RGB LEDs, we can overcome this by writing an external tool that generates `data` instructions
        * The values are stored little endian, so `data 0x01 0x02 0x03 0x04` is stored as uint32 0x04030201

* ISSUE: The disassmbler may misinterprete data instructions as other instructions.
    * May not matter, anyway the user should always save the config...
    * But: may be an issue with interpreting as goto or variable out of range

* ISSUE: how to distinguish `END_OF_PROGRAMS` and `END_OF_PROGRAM`?
    * MSB must not be 0xff or 0xfe -> change them to 0xfd during assembly
    * May not be an issue, switching between 0xff/0xfe and 0xfd may not even be visible
    * Or maybe only allow end markers to be 0xffffffff 0xfeffffff, which is easier to prevent?


## Opcode assignment

* 0x3a extern-leds-count
* 0x3b extern-leds-set
* 0x3c extern-leds-add


## Example

Light program for 4x addressable RGB LEDs.
3 colors time 4 LEDs = 12 bytes
Color order: R G B


    run always

    extern-leds-count 12        // 4 addressable LEDs, each having 3 colors

    loop:
        extern-leds-set STRIP_OFF
        sleep 300
        extern-leds-add STRIP_RED_HALF  // Normally We would use set instead of add; just a demo ...
        sleep 300
        goto loop

    STRIP_OFF:
        data 0x00 0x00 0x00 0x00    // R1 G1 B1 R2
        data 0x00 0x00 0x00 0x00    // G2 B2 R3 G3
        data 0 0 0 0                // B3 R4 G4 B4

    STRIP_RED_HALF:
        data 0x7f 0x00 0x00 0x80
        data 0x00 0x00 0x7f 0x00
        data 0x00 0x7f 0x00 0x00

    end

## Output configuration

* OUT/ISP
* TH/Tx
* OUT15S
* Inverted option
* Slave vs light program option

## Improvements

* Have the configurator pre-calculate inverted WS281x data bits

