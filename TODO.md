# Configurator

* AUX2/3 2-pos and 3-pos switch for direct gear control

* Make no-signal, initializing, etc configurable via a table
    - Tab based: `car functions` and `diagnostics`
    - Checkbox per diagnostics function
    - 1 brightness field for all LEDs/diagnostics functions,

* Light programs: combine `start` and `programs` for fully dynamic flash memory use
    - The light_programs code can handle it already
    - The configurator recent change already implements it
    - Now only the global.h structure and the C output of light-program-assembler need to change


# Mk4P and Mk4S

* Make mounting hole smaller
