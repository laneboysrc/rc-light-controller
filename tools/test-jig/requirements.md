# Test jig for light controllers and pre-processors

## Goals
- Be able to test light controllers and pre-processors before shipping them
- Be able to test light controllers after soldering
- Be able to test pre-processors after soldering
- Needs to work for both 3-ch and 5-ch pre-processors

**Excluded functions**
- Programming of light controllers and pre-processors

## Execution ideas
x Socket for mounting the 8ch receiver
x 5x 3-pin pin headers for mounting external receivers
x Power:
    x Micro-USB
    x 3-pin pin header
    x on/off Switch
    x Power LED
x 6-pin pin header for the light controller when testing with a pre-processor
x 6-pin pin header for the light controller when testing as servo reader
x 18 Pogo-pins (16 LED, 1 switched LED, 1 LED+) and mechanical support for the light controller
x Socket for the 3-ch pre-processor
x Socket for the 5-ch pre-processor
x 6-pin pin header for the USB-to-serial, wired to read the pre-processor output
    - Power not connected!
x 6-pin pin header for the USB-to-serial, wired to use the pre-processor simulator on the light controller
    - Power not connected!
x 2-pin pin header test points for all signals
    - Supply +, -
    - ST, TH, AUX, AUX2, AUX3, OUT/ISP, Pre-processor light output
x Push-button on CH3 for preprocessor test
x Mounting holes on the PCB edge
x Screw holes for the mechanical support for the light controller
- Everything is wired in parallel, no switches or jumpers; just plug in the devices needed for a test
- Generous and descriptive silk screen
