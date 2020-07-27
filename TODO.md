# Mk4S Switching version of Mk4
# Mk4P Pinheader version of Mk4

* LED driver for switching version

* Able to move Pre-processor output to OUT/ISP

DONE Software for version auto-detect support
    DONE Switching version has PIO0_14 (20) pulled to ground

DONE  LED+ on center pin, next to GND

DONE Version auto-detect support
    DONE How to differentiate Mk4P and Mk4S?
        DONE Switching version has PIO0_14 (20) pulled to ground
        DONE If left floating, we have the TLC5940 version (original Mk4 or new Mk4P)
        DONE PIO0_14 is not accessible on the 16 pin package of the original Mk4, so it should always be detected as high (pull-up)

DONE Check SW pin compatibility original version and Mk4P
