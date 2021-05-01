# Configurator

* Add 'Whats new' section to info

* Make no-signal, initializing, etc configurable via a table
    DONE Tab based: `car functions` and `diagnostics`
    DONE Checkbox per diagnostics function
    DONE 1 brightness field for all LEDs/diagnostics functions
    DONE Calculate diagnostics_mask when saving a configuration (never loaded, just saved)
    DONE Add help text
    DONE Add tooltips
    DONE Clear all leds should clear diagnostics

* Light programs: combine `start` and `programs` for fully dynamic flash memory use
    DONE The light_programs code can handle it already
    - Change global.h structure and the C output of light-program-assembler need to change
    DONE FIX CONFIGURATOR DISASSEMBLER ISSUE WITH DYNAMIC TABLE!

* Remove old/dead code from configurator
    - ui.js init_led_features()

* Update WebUSB programmer image

DONE Remove WebUSB programmer console log


# Mk4P and Mk4S

* Make mounting hole smaller
