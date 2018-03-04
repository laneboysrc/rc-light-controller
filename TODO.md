# MK5


# MK5 PCB

* TLC5940 footprint based on TI datasheet
* Design for stencil 5mil/0.12mm thick
* Verify Micro-USB footprint fits

# MK4 + MK5

* It would be great if CH3 as switch would trigger `new_channel_data` independently of the other signals, i.e. set it if no other `new_channel_data` was seen in a certain amount of systicks

* FW: Allow light programs to read input pin states

* FW: Add Xenon lamp simulation

* Deprecate CPPM reader (to simplify things, no-one ever used it)

* Shelf Queen mode

* More sophisticated servo reader initialization:
  * Require constant stream of data, not just one pulse
  * Plausibility check?


# Configurator

* Configurator to have a shortcut for boilerplate for new light programs
    E.g. all LEDs pre-defined

* Add support for addressing LEDs without having to use an
    led x = led[y] statement. This is useful for light patterns where the
    LED sequence is important. This could be as easy as translating names like
    'indicator' to the appropriate led[0..31] values.

* Add `local_switch_is_momentary` for MK5

* Make pre-processor configuration in drop-down box

* Add 'save light program as text' button


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