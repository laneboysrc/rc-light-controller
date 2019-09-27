# MK4

X US-style combined tail/brake/indicator

X Deprecate CPPM reader (to simplify things, no-one ever used it)

X Asymetric indicator blinking for US cars

X Shelf Queen mode

X Add AUX value reading to light programs

# Configurator

X Check with IE10

X Add `stand-alone` mode

X Make `pre-processor` a configuration in the drop-down box

X Add RUN_WHEN_SHELF_QUEEN_MODE
X Add AUX, AUX2. AUX3

X Add support for addressing LEDs without having to use an
    led x = led[y] statement. This is useful for light patterns where the
    LED sequence is important. This could be as easy as translating names like
    'indicator' to the appropriate led[0..31] values.

X Set Local ch3_is_local_switch always when UART input active


# General improvements

* DOC: When a priority program runs once, and another state takes precedence,
  the program has no effect and after the other state disappears, the lights
  are still wrong. Solution is to output constantly in a loop,
  including fade commands!.
  Needs documenting.
