# Firmware architecture

All current variants of the DIY RC Light Controller use Microchip PIC 16 
micro-controllers.

The firmware is written in Assembler, using the open source [gputils](http://gputils.sourceforge.net/).

One of the main features is that the light controller is very customizable.
Often body shells require different light configurations: some have
parking lights and main beam, others main beam and high beam only. Some cars have
fog lamps. On some cars the brake and tail lamps are combined, on others
they are different LEDs.

As such the vehicle-specific functionality is stored in a separate file
that starts with the prefix **lights-**. 

There is however also a **lights-generic.asm** that may fit a wide variety 
of vehicles. The functionality of the generic configuration is described in
[../doc/light-controller-instructions.pdf](../doc/light-controller-instructions.pdf)

For hardware testing there is **lights-test-tlc5940-16f1825.asm** and
**lights-test-ws2812-12f1840.asm**, which blink the connected LEDs without
the need of any receiver input. Very useful for debugging PCBs.


## Common components

Independent of the hardware and vehicle configuration all light controller 
variants build upon a set of common files. Usually you will not need to 
modify any of these files.

- **master.asm**

    This file contains the main business logic of the light controller. It
    also contains the startup function and hardware initialization.

- **utils.asm**

    Utility functions for mathematics, etc.

- **servo-output.asm**

    Functionality to drive a servo, e.g. for a 2-speed gearbox, a steering wheel
    or a figures head.
  

## Hardware specific components

To gain flexibility in the used hardware, certain functionality has been 
split into include and source files. The correct include file matching the
light controller hardware must be chosen.

- **hw_tlc5940_16f1825.inc**

    For hardware based on the TLC5940 and the PIC16F1825.

- **hw_ws2812_12f1840.inc**, **ws2812.asm**

    For driving WS2812B or PL9823 LEDs using a PIC12F1840 micro-controller.

- **hw_original_servo_reader.inc**, **hw_original_uart_reader.inc**

    For the first iteration of the light controller based on PIC16F628A and
    TLC5916. Not recommended for new builds.


## Input specific components

The DIY RC Light Controller can get access to the Steering, Throttle and AUX
channels either by reading the servo outputs directly from the receiver, or
in a single combined UART (serial) signal from a pre-processor.

- **servo-reader.asm**

    This file must be used when reading servo channels directly from a receiver.
    Note that all three signals must be present, otherwise the light controller
    will not function!

- **uart-reader.asm**

    This file is used when a pre-processor is employed. 
  
- **dummy-reader.asm**

    Used during development only. It can be modified to perform certain operations
    without having to use an actual RC system.


## Build process

Building the various light controller firmware variants is done with a makefile.
The supplied makefile is GNU Make compatible.

Running **make** builds all configured variants and vehicles. To build a specific
variant one needs to execute **make vehicle_name**. 

Please refer to the makefile itself for more information; it contains detailed
comments about how the build process works.


## Creating a custom light configuration

As stated initially, many RC body shells differ in terms of available lights.
Therefore we usually create a new custom light logic for each new body shell
we acquire. We don't do this from scratch but mostly copy from an existing
vehicle that has a hardware and light configuration close to the new vehicle.

The light logic is stored in files with the prefix **lights-"**. Each light logic
is also tied to a specific hardware, which is the hardware we've chosen
for that vehicle. 

Good choices for starting new vehicles are:
For the TLC5940 based light controller, use **lights-ferrari-la-ferrari.asm**
for a car that has only 2 LEDs in the front and 2 in the back. 

A more complex TLC5940 based example is **lights-subaru-impreza-2008.asm**, which 
drives 14 light outputs in total. 

A very elaborate light logic with winch control, 2-speed gearbox control and 
running lights is **lights-wraith.asm**. also TLC5940 based like most of the 
light logic files supplied.

For the WS2812 based light controller there is **lights-fiat-131.asm** as 
simple example, mixing both WS2812B and PL9823 LEDs. **lights-lancia-fulvia.asm**
is using PL9823 exclusively but has a nice gimmick of fading indicators, 
simulating incandescent bulbs, and a simulation of a weak earth connection
by dimming the left tail/brake light whenever the indicator lights up.

Some old American vehicles have a single light in the back for combined tail, 
brake and indicator functionality. You can find a reference implementation of 
this scheme in **lights-sawback.asm** (TLC5940 based) and **lights-xr311.asm** 
(outdated TLC5916 design with two stacked TLC5916s, not recommended).


## Changing features at compile time

Certain features of the light controller can be configured when 
compiling a vehicle.
These features can be found in the **CFLAGS** directive of a vehicle in the
makefile.

- **LIGHT_MODE_MASK=**0x0f

    This bitmask defines how many lights there are that should be switched 
    on with a single click of the AUX (CH3) channel. Each time AUX is activated,
    more lights are turned on. If you have 2 lights (main beam and high beam 
    for example), then you would specify **0x03** (0011 binary), if you have
    parking, main beam, high beam and fog lights you would specify **0x0f** 
    (1111 binary), and so on.

- **CH3_MOMENTARY** 

    Define this name if your transmitter's AUX channel returns to the initial 
    position when the AUX channel button is released (Futaba 4PL for example)
    If this name is not defined then the software assumes that every AUX channel
    activation toggles the AUX servo position (HobbyKing HK310, FlySky GT3B).

- **ESC_FORWARD_REVERSE**

    Define this name when your ESC is in "crawler mode", i.e. it does not have
    a brake function but goes from forward to reverse immediately.

- **DISABLE_BRAKE_DISARM_TIMEOUT**

    Define this name if your ESC goes into reverse only after applying the
    brake function once (Example: Tamiya TEU-105BK). 

    Do not define this name for ESCs that automatically allow going into reverse
    when the throttle has been in neutral for a certain amount of time.

- **ENABLE_STEERING_WHEEL_SERVO** 
    
    Define this name if you want the light controller to drive a steering wheel
    servo. You must also define **ENABLE_SERVO_OUTPUT** and include **servo-output** in 
    the list of files in the makefile.

- **ENABLE_SERVO_OUTPUT**

    Define this name if you want the light controller to drive a servo. You must
    also include **servo-output** in the list of files in the makefile.
  
- **ENABLE_UART** 

    Define this name when using a UART related function such as uart-reader.

- **NUMBER_OF_LEDS=**8

    Specifies the number of WS2812B or PL9823 LEDs in use. Only applies to the 
    WS2812 variant of the light controller.
  
- **ENABLE_GEARBOX**

    Define this name to enable control of a 2-speed gearbox using a servo.
    You must also define **ENABLE_SERVO_OUTPUT** and include **servo-output** in 
    the list of files in the makefile.
  
- **SERVO_OUTPUT_ON_THROTTLE**

    Define this name to output the servo signal on the throttle channel instead
    of the dedicated steering wheel servo output of the light controller.
    Only has an effect when **ENABLE_SERVO_OUTPUT** is enabled.

- **ENABLE_WINCH**

    Enables control of a RC winch through using the 
    [DIY RC Winch Controller](http://laneboysrc.blogspot.com/2013/09/make-your-own-rc-winch-controller.html).
  
- **RECEIVER_OUTPUT_RATE=**20

    Allows to specify the number of milliseconds between repeated servo pulses
    given by the receiver. If not specified the default of 16 ms is used.
    All this value really does is adjust the timing of initialization.
  
- **SEQUENTIAL_CHANNEL_READING** 

    Define this name when you are sure that the steering, throttle and AUX
    channel pulses are output by the receiver in sequence, right after each other.
    This is the case for the HobbyKing HKR3000 receiver.
    If unsure, do not define this value.


- **CHANNEL_SEQUENCE_TH_ST_CH3**

    Define this along with **SEQUENTIAL_CHANNEL_READING** when the receiver pulse 
    sequence throttle, steering, AUX; like in the GT3B.
  
- **DUAL_TLC5916**

    Enables the use of two TLC5916 (for dim and full brightness). Not recommended,
    use TLC5940 hardware instead.
  

  








  


