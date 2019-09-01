# MK4

X US-style combined tail/brake/indicator as per the gentleman from Manila

X Deprecate CPPM reader (to simplify things, no-one ever used it)

X Asymetric indicator blinking for US cars

X Shelf Queen mode

X Add AUX value reading to light programs

# Configurator

* Fix table for ground fault not lining up

* Add `stand-alone` mode

* Make `pre-processor` a configuration in the drop-down box

* Configurator to have a shortcut for boilerplate for new light programs
    E.g. all LEDs pre-defined

* Add support for addressing LEDs without having to use an
    led x = led[y] statement. This is useful for light patterns where the
    LED sequence is important. This could be as easy as translating names like
    'indicator' to the appropriate led[0..31] values.

* Add RUN_WHEN_SHELF_QUEEN_MODE
* Add AUX, AUX2. AUX3
* Remove `local_switch_is_momentary`
* Set Local ch3_is_local_switch always when UART input active

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