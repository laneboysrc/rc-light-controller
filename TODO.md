# MK4

* Enable entering ISP mode via UART while firmware is running?

# MK5

* Investigate outputs sometimes not working on Android board

* Remove WebUSB URL

* DFU read firmware


# MK5 PCB

* TLC5940 footprint based on TI datasheet
* Output stencil 5mil/0.12mm thick
* Verify Micro-USB footprint fits
* QR code to configurator for phone/webusb


# MK4 + MK5

* FW: Add Xenon lamp simulation

* Deprecate CPPM reader (to simplify things, no-one ever used it)

* Shelf Queen mode

* More sophisticated servo reader initialization:
  * Require constant stream of data, not just one pulse
  * Plausibility check?


# Configurator

* nw.js based stand-alone program, and on-line WebUSB
  * Use Babel ot make work on IE11
  * Babel-minification?
  * Use latest version on Github by default in nw.js, local if no internet

* Refactor firmware loading
  - Load from JS(hex)/Disk(hex, bin)/ISP(bin)
    - Hex: convert to bin, then use bin loading function
  - Auto-detect Mk4/Mk5 when loading firmware

* Disable preprocessor-simulator while flashing

* Implemment firmware reading for MK5

* Add Arduino/BOSSAC support


* Fix table for ground fault not lining up

* Add `local_switch_is_momentary` for MK5

* Add `stand-alone` mode

* Make `pre-processor` a configuration in the drop-down box

* Configurator to have a shortcut for boilerplate for new light programs
    E.g. all LEDs pre-defined

* Add support for addressing LEDs without having to use an
    led x = led[y] statement. This is useful for light patterns where the
    LED sequence is important. This could be as easy as translating names like
    'indicator' to the appropriate led[0..31] values.


# General improvements

* DOC: When a priority program runs once, and another state takes precedence,
  the program has no effect and after the other state disappears, the lights
  are still wrong. Solution is to output constantly in a loop,
  including fade commands!.
  Needs documenting.

* DOC: Add more connection diagrams
    - Overall RC system with preprocessor
    - Overall RC system with servo reader
    - Overall RC system with 2 light controllers
    - LED connection
    - Light bar connection directly powered from LiPo

* DOC: make a nicer diagram for the programming cable

* DOC: make a diagram how to program the preprocessor

* DOC: add more documentation to preprocessor and distribution boards