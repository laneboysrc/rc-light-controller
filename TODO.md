DONE Function to invert 15S (for turning off an external boost/buck converter)
    DONE Make it work on the slave too!

# GIT Cleanup

DONE Move Mk1..Mk3 to a separate "legacy" branch
DONE List obsolete branch in HARDWARE.md

# Documentation

DONE Update German version

DONE* Make photos
    DONE Mk4, Mk4P and Mk4S for intro
    DONE Mk4P image
    DONE Mk4S image
    DONE Mk4P/S right side connector OUT/ISP
    DONE 2x Mk4P/S right side connector, one with jumper, one with batt lead

* Make diagrams
    DONE Two position switch
    DONE Push button implementing Two position switch
    DONE Two-position switch with up/down buttons
    DONE Momentary push button

DONE Describe two position switch, push button, and how to find out
DONE Mk4P and Mk4S power options
DONE Mk4P and Mk4S connectors
DONE Mk4P and Mk4S master slave and mixing S and P
DONE Add Mk4P and Mk4S description
DONE Synchronization issues when using 2 master
DONE USB-to-Serial not recognized + Driver
DONE Mk4S not dimmable
DONE Swap Pre-Processor and Servo reader (prefer Pre-Processor)
DONE Connection table: add Pre-Processor to ST/Rx
DONE Describe how to deal with only two LED+ pads
DONE Add separate power supply for LEDs section
DONE Add light bar resistor calculation
DONE Add connecting LEDs in parallel
DONE Move parallel and serial LED connection to an advanced topics section in the back
DONE Add "resistor required" to the high current output
DONE Add slave light controller section
DONE Add TH/Tx to output pins section (rename from Servo out)
DONE Split operating section into sub-sections
DONE Configurator: default to online version
DONE LPC81x_ISP: where to download?
DONE Specification: make table for all variants
DONE Add waterproofing info

DONE Configurator: add typical example (make special video?)

# Light programs

DONE Add link to example scripts

DONE Implement a SKIP IF NOT function for reasier human reasoning.
    This can be done in the assembler by inverting the expression.
    No new command needed in the firmware.
    Call the new function just IF

DONE Document that some variables can not be used in *skip if*

DONE Move use-case examples to the front


# Configurator

DONE Warning messages when configuration not suitable for Pre-Processor simulator
    DONE Baudrate wrong
    DONE UART used up
    DONE UART not on TH/Tx
DONE Append CR to end of light program (to prevent issue when CR is missing after END)
DONE Remember last used config filename and hex filename?
DONE Show light program size in bytes


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
