# MK4

* FW: Test revised dignostics selection on LPC812


# MK5

* Use DFU, using existing WebUSB DFU implementation

* Use DMA for UART TX?

* Use Watchdog

* Does LED on PA01 interfere with Arduino hardware?

* Button input via HAL (must be configurable)

* PLL coarse and fine; USB are taken from USER instead of FACTORY CAL area!

# MK5 PCB

* TLC5940 footprint based on TI datasheet
* Design for stencil 5mil/0.12mm thick
* How to deal with the USB shielding?

* Move TH to PA18. Because PA17 is LED/D13 on Arduino Zero
* Can we use other pins than PA22/PA23? They are not convenient on the Protoneer

#MK4 + MK5

* It would be great if CH3 as switch would trigger `new_channel_data` independently of the other signals, i.e. set it if no other `new_channel_data` was seen in a certain amount of systicks

* FW: Allow light programs to read input pin states

* FW: Add Xenon lamp simulation

* Deprecate CPPM reader (to simplify things, no-one ever used it)

* Shelf Queen mode


# General improvements

* DOC: When a priority program runs once, and another state takes precedence,
  the program has no effect and after the other state disappears, the lights
  are still wrong. Solution is to output constantly in a loop,
  including fade commands!.
  Needs documenting.

* HW: issue with analog servo interfering with ISP
  Clearly an issue with some servos only. Need to disable ISP and rather provide
  a software way for those

* TOOL: Configurator to have a shortcut for boilerplate for new light programs
    E.g. all LEDs pre-defined

* TOOL: Configurator: Add support for addressing LEDs without having to use an
    led x = led[y] statement. This is useful for light patterns where the
    LED sequence is important.

* TOOL: Configurator: Make pre-processor configuration in drop-down box

* TOOLS: Add tool to extract light program source out of light controller config

* TOOLS: Add watch folder to ISP tool