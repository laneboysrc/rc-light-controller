# MK4

* Use watchdog


# MK5

* Use DFU, using existing WebUSB DFU implementation

* Use DMA for UART TX?

* Use Watchdog

* Does LED on PA01 interfere with Arduino hardware?
* Support LED on PA01 and PA17 (Arduino) simulatinously
    * Also in the bootloader

* What does the led do in the light controller?
    * Flash when no signal
    * Light up during initialization stage
    * Off during normal operation

* Button input via HAL (must be configurable)


# MK5 PCB

* TLC5940 footprint based on TI datasheet
* Design for stencil 5mil/0.12mm thick

* Can we have pins for ST, TH and CH3? So that we can make adapter boards that directly plug into the receiver

* Each MCU signal needs series resistor; then we also can get rid of test points


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

* DOC: Add more connection diagrams
    - Overall RC system with preprocessor
    - Overall RC system with servo reader
    - Overall RC system with 2 light controllers
    - LED connection
    - Light bar connection directly powered from LiPo

* DOC: make a nicer diagram for the programming cable

* DOC: make a diagram how to program the preprocessor

* DOC: add more documentation to preprocessor and distribution boards