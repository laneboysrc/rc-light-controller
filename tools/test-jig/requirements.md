# Test jig for light controllers and pre-processors

## Goals
- Be able to test light controllers and pre-processors before shipping them
- Be able to test light controllers after soldering
- Be able to test pre-processors after soldering
- Needs to work for both 3-ch and 5-ch pre-processors

**Excluded functions**
- Programming of light controllers and pre-processors

## Execution ideas
- Socket for mounting the 8ch receiver
- 5x 3-pin pin headers for mounting external receivers
- Power:
    - Micro-USB
    - 3-pin pin header
    - on/off Switch
    - Power LED
- 6-pin pin header for the light controller
- 17 Pogo-pins and mechanical support for the light controller
- Socket for the 3-ch pre-processor
- Socket for the 5-ch pre-processor
- 6-pin pin header for the USB-to-serial, wired to read the pre-processor output
- 6-pin pin header for the USB-to-serial, wired to use the pre-processor simulator on the light controller
- Everything is wired in parallel, no switches or jumpers; just plug in the devices needed for a test
- 2-pin pin header test points for all signals
    - Supply +, -
    - ST, TH, AUX, AUX2, AUX3, OUT/ISP, Pre-processor light output
- Mounting holes on the PCB edge
- Generous and descriptive silk screen

## Open points
- How to test light controller as servo reader?