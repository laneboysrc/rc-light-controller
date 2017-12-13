# TO DO LIST for the RC Light Controller

* FW: Test revised dignostics selection on LPC812

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

* TOOL: Configurator: Fix issue with firmware version number in config file

* TOOLS: Add tool to extract light program source out of light controller config

* TOOLS: Add watch folder to ISP tool


## Ideas for mk5

* Trigger bootloader from CDC like Arduino (1200 BAUD, DTR low)
    * Can we also detect BOSSAC?

* Button input via HAL (must be configurable)

* LED out separate IO than OUT15S?


# MK5 PCB

* TLC5940 footprint based on TI datasheet
* Design for stencil 5mil/0.12mm thick
* GPIO short-circuit for hardware detection!