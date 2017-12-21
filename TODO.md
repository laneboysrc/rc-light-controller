# MK4

* FW: Test revised dignostics selection on LPC812


# MK5

* Use DFU, using existing WebUSB DFU implementation

* Look at GPIO HAL from T2, e.g. `pin_low`, `pin_out`, ...

* Low interrupt priority for Systick

* Use DMA for UART TX?

* Make convenient script to flash app with bootloader, and debug as well

* Use Watchdog

* Does LED on PA01 interfere with Arduino hardware?

* Button input via HAL (must be configurable)

* How to deal with the USB shielding?

* Implement CPPM reader?

* FIX dfu-util reporting 'dfu-util: error detaching' when switching from run time mode to dfu mode
* FIX dfu-util reporting 'dfu-util: unable to read DFU status after completion' after flashing


# MK5 PCB

* TLC5940 footprint based on TI datasheet
* Design for stencil 5mil/0.12mm thick


# General improvements

* DOC: When a priority program runs once, and another state takes precedence,
  the program has no effect and after the other state disappears, the lights
  are still wrong. Solution is to output constantly in a loop,
  including fade commands!.
  Needs documenting.

* HW: issue with analog servo interfering with ISP
  Clearly an issue with some servos only. Need to disable ISP and rather provide
  a software way for those

* FW: Allow light programs to read input pin states

* FW: Add Xenon lamp simulation

* TOOL: Configurator to have a shortcut for boilerplate for new light programs
    E.g. all LEDs pre-defined

* TOOL: Configurator: Add support for addressing LEDs without having to use an
    led x = led[y] statement. This is useful for light patterns where the
    LED sequence is important.

* TOOL: Configurator: Make pre-processor configuration in drop-down box

* TOOLS: Add tool to extract light program source out of light controller config

* TOOLS: Add watch folder to ISP tool