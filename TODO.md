
# Documentation

* Add Mk4P and Mk4S description
* Add Mk4P and Mk4S images
* Add Rev 3 image
* Swap Pre-Processor and Servo reader (prefer Pre-Processor)
* Describe how to deal with only two LED+ pads
* Connection table: add Pre-Processor to ST/Rx
* Add connecting LEDs in parallel
* Move parallel and serial LED connection to an advanced topics section in the back
* Add "resistor required" to the high current output
* Add separate power supply for LEDs section
* Add slave light controller section
* Add TH/Tx to output pins section (rename from Servo out)
* Split operating section into sub-sections
* Configurator: default to online version
* Configurator: add typical example (make special video?)
* Light programs: move use-case examples to the front
* Light programs: add link to example scripts
* Specification: make table for all variants

# Light programs

Implement a SKIP IF NOT function for reasier human reasoning.
This can be done in the assembler by inverting the expression.
No new command needed in the firmware.
Call the new function just IF


# Mk4S Switching version of Mk4
# Mk4P Pinheader version of Mk4


DONE LED driver for switching version

DONE Early init of GPIO controlled light outputs

DONE Add new convenience flags to configurator

DONE Move servo_output_enabled to config

DONE Change UART and servo out initialization to be able to use OUT or TH pins based on user wish

DONE Change diagnostics to use NULL for STDOUT when diagnostics is disabled; use printf instead of fprintf

DONE Software for version auto-detect support
    DONE Switching version has PIO0_14 (20) pulled to ground

DONE  LED+ on center pin, next to GND

DONE Version auto-detect support
    DONE How to differentiate Mk4P and Mk4S?
        DONE Switching version has PIO0_14 (20) pulled to ground
        DONE If left floating, we have the TLC5940 version (original Mk4 or new Mk4P)
        DONE PIO0_14 is not accessible on the 16 pin package of the original Mk4, so it should always be detected as high (pull-up)

DONE Check SW pin compatibility original version and Mk4P
