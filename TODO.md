# TO DO LIST for the RC Light Controller

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

- Based on STM32F103C8T6
  - 64KB flash, 20KB RAM, crystal-less USB, ultra cheap
- Support own hardware as well as *blue pill* board
  - PWM output on GPIOs
- USB DFU for config/firmware update
- USB Live configuration
- Stay compatible with mk4 as much as possible
- Hardware with (optional) Tamiya LED compatible plugs
  - Also check on-line for cheap pre-wired LEDs with connector
- Heatsink for TLC5940?
- Power from USB and servo connector
- Preprocessor only?
- 3D printed case, so 1 PCB only

## Preprocessor board
- Based on LPC811/2 TSSOP16
- Use existing firmware
- 5x3 input, 1x3 output to light controller
- MCP1702 voltage reg, 2x 1uF vreg
- No components on bottom
