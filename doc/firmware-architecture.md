# Firmware architecture

All current variants of the DIY RC Light Controller use Microchip PIC 16 micro-controllers.

The firmware is written in Assembler, using the open source [gputils](http://gputils.sourceforge.net/).


## Customizing the light controllers functionality

Often body shells require different light configurations: some have parking lights and main beam, others main beam and high beam only. Some cars have fog lamps. On some cars the brake and tail lamps are combined, on others they are separate LEDs.

To facilitate these difference, vehicle-specific algorithms are stored in a separate file that starts with the prefix ``lights-``.

There is however also a ``lights-generic.asm`` that may fit a wide variety of vehicles. The functionality of the generic configuration is described in [../doc/light-controller-instructions.pdf](../doc/light-controller-instructions.pdf).
This configuration is currently only available for the TLC5940 based hardware. The firmware directory contains pre-assembled HEX files for your covenience:

- [generic-two-position-master.hex](../firmware/generic-two-position-master.hex)

    For transmitters where the AUX/CH3 channel is a two position switch; e.g. HobbyKing HK310, FlySky GT3B

- [generic-momentary-master.hex](../firmware/generic-momentary-master.hex)

    For transmitters where the AUX/CH3 channel is a momentary push button; e.g. Futaba 4PL

For hardware testing there is ``lights-test-tlc5940-16f1825.asm`` and ``lights-test-ws2812-12f1840.asm``, which blink the connected LEDs without the need of any receiver input. Very useful for debugging PCBs.

Ready made HEX files are available for your convenience:

- [test-tlc5940-16f1825.hex](../firmware/test-tlc5940-16f1825.hex)

    For the TLC5940 and PIC16F1825 based hardware

- [test-ws2812-12f1840.hex](../firmware/test-ws2812-12f1840.hex)

    For the PIC12F1840 based hardware driving WS2812B or PL9823 LEDs


## Common components

Independent of the hardware and vehicle configuration, all light controller
variants build upon a set of common files. Usually you will not need to modify
any of these files.

- ``master.asm``

    This file contains the main business logic of the light controller. It also
    contains the startup function and hardware initialization.

- ``utils.asm``

    Utility functions for mathematics, etc.

- ``servo-output.asm``

    Functionality to drive a servo, e.g. for a 2-speed gearbox, a steering wheel or a figures head.


## Hardware specific components

To gain flexibility in the used hardware, certain functionality has been split
into include files. The correct include file matching the light controller
hardware must be chosen.

- ````hw_tlc5940_16f1825.inc````

    For hardware based on the TLC5940 and the PIC16F1825.

- ````hw_ws2812_12f1840.inc````, ````ws2812.asm````

    For driving WS2812B or PL9823 LEDs using a PIC12F1840 micro-controller.

- ````hw_original_servo_reader.inc````, ````hw_original_uart_reader.inc````

    For the first iteration of the light controller based on PIC16F628A and
    TLC5916. Not recommended for new builds.


## Input specific components

The DIY RC Light Controller can get access to the Steering, Throttle and AUX
channels either by reading the servo pulses directly from the receiver, or in a
single combined UART (serial) signal from a pre-processor. Please refer to
[hardware-overview.md](hardware-overview.md) for information on the pre-
processor.

- ``servo-reader.asm``

    This file must be used when reading servo pulses directly from a receiver.
    Note that all three signals must be present, otherwise the light controller
    will not function!

- ``uart-reader.asm``

    This file is used when a pre-processor is employed.

- ``dummy-reader.asm``

    Used during development only. It can be modified to perform certain operations
    without having to use an actual RC system.


## Build process

Building the various light controller firmware variants is done with a makefile. The supplied makefile is GNU Make compatible.

Running ``make`` builds all configured variants and vehicles. To build a
specific variant one needs to execute ``make vehicle_name``.

Please refer to the makefile itself for more information; it contains detailed
comments about how the build process works.


## Creating a custom light configuration

As stated initially, RC body shells differ in terms of available lights. Therefore we usually create a new custom light logic for each new body shell we
acquire. We don't do this from scratch but mostly copy from an existing vehicle
that has a hardware and light configuration close to the new vehicle.

The light logic is stored in files with the prefix ``lights-"``. Each light
logic is also tied to a specific hardware, which is the hardware we've chosen
for that vehicle.

Good choices for starting new vehicles are:

For the TLC5940 based light controller, use the simple ``lights-ferrari-la-ferrari.asm`` for a car that has only 2 LEDs in the front and 2 in the back.

A more complex TLC5940 based example is ``lights-subaru-impreza-2008.asm``, which drives 14 light outputs in total.

A very elaborate light logic with winch control, 2-speed gearbox control and
running lights is ``lights-wraith.asm``; also TLC5940 based like most of the
light logic files supplied.

For the WS2812 based light controller there is ``lights-fiat-131.asm`` as simple
example, mixing both WS2812B and PL9823 LEDs. ``lights-lancia-fulvia.asm`` is
using PL9823 LEDs exclusively but has a nice gimmick of fading indicators,
simulating incandescent bulbs, and a simulation of a weak earth connection by
dimming the left tail/brake light whenever the indicator lights up.

Some old American vehicles have a single light in the back for combined tail,
brake and indicator functionality. You can find a reference implementation of
this scheme in ``lights-sawback.asm`` (TLC5940 based) and ``lights-xr311.asm``
(outdated TLC5916 design with two stacked TLC5916s, not recommended).


## Light configuration implementation details

Every ``lights-"`` vehicle specific file needs to export two functions that
are called by the rest of the firmware:

- ``Init_lights``

    This function is called during startup of the DIY light controller. Here we
    initialize the LED driver hardware, and set the LEDs to a unique pattern
    that indicates that the light controller is waiting for a receiver signal.
    This helps us identifying connection problems, as the ``Output_lights``
    function documented below is only called once we have detected valid pulses
    from the receiver. Therefore we know that if the LED pattern set by the
    ``Init_lights`` function is visible, we did not receive valid signals from the receiver yet.

- ``Output_lights``

    This function is called every *mainloop*. It monitors the state of certain
    global variables (see below) and turns on and off the LEDs corresponding to
    the state found in those variables. The mainloop runs once every time all
    three servo signals (Steering, Throttle and AUX) have been read. Ideally
    this happens at the repeat rate of the servo pulses (e.g. 16ms for the
    HobbyKing HK310, ~8ms for the Futaba 4PL), but may take 3 times as long if
    the sequence of the pulses generated by the receiver is not the sequence
    expected by ``servo-reader.asm``.

The light controller firmware outputs the state of certain light and drive
functions in global variables. The ``lights-"`` vehicle specific file monitors
these variables and outputs LEDs that corresponds to the state of those
variables.

The next sections describes some of the variables available.

### startup_mode

This variable is non-zero during the time the DIY RC light controller is waiting for the servo signals to stabilize and register the neutral point for Steering and Throttle. The user should not touch Steering and Throttle during this time as it would interfer with recording the neutral point, therefore we generate a uniquly identifyable light pattern that the driver can recognize that initialization is in progress.
Currently ``startup_mode`` only has a single bit ``STARTUP_MODE_NEUTRAL``, but other bits are reserved for future use.

### setup_mode

This variable is non-zero whenever a setup function, such as servo reversing or setting neutral and end-points for the steering wheel servo, are active.

The bits ``SETUP_MODE_CENTRE``, ``SETUP_MODE_LEFT`` and ``SETUP_MODE_RIGHT`` are used for setting up the steering wheel or gear shift servo (8 clicks on the AUX/CH3 channel). Usually we turn off all LEDs and turn the Indicator lights on permamently to signal those states (``SETUP_MODE_LEFT`` = left indicator; ``SETUP_MODE_RIGHT`` = right indicator; ``SETUP_MODE_CENTRE`` = both indicators).

``SETUP_MODE_STEERING_REVERSE`` and ``SETUP_MODE_THROTTLE_REVERSE`` prompt the user to turn the steering left, and the throttle to forward respectively so that the DIY RC Light Controller can learn the direction of the steering an throttle channels. This function is invoked with 7 clicks on the AUX/CH3 channel. Usually we turn on both main beams (but no tail lights) for ``SETUP_MODE_THROTTLE_REVERSE``, and both left indicators for ``SETUP_MODE_STEERING_REVERSE``.

### light_mode

This variable sets one more bit every time a single click on AUX/CH3 is performed.

It is used to drive the main lights on the vehicle. For example if a vehicle has parking lamps, main beam, and high beam, we would use bit 0 in ``light_mode`` to light up the parking lamps and tail lights; bit 1 to light up the main beam, and bit 2 to light up the high beam.

Higher bits are only set when the lower bits are 1, so we generate realistic light patterns, i.e. the main beam can only be on when parking lamps and tail lights are on.

### drive_mode

There are two bits in this variable that are of concern to us:

``DRIVE_MODE_BRAKE`` is set when the brake lights should turn on.

``DRIVE_MODE_REVERSE`` is set when the reversing lights should be on.

Other bits are available too; please refer to the source code for details.

### blink_mode

The ``blink_mode`` variable contains bits that relate to the indicator and hazard light functions.

``BLINK_MODE_BLINKFLAG`` toggles with 1.5 Hz and provides the actual blinking frequency. Usually you want to have the indicator LEDs off when this bit is off.

``BLINK_MODE_HAZARD`` is set when the hazard light funciton has been turned on (4 clicks on AUX/CH3)

``BLINK_MODE_INDICATOR_LEFT``, ``BLINK_MODE_INDICATOR_RIGHT`` are set when the left / right indicator function is active.


## Changing features at compile time

Certain features of the light controller can be configured when compiling a vehicle.
These features can be found in the ``CFLAGS`` directive of a vehicle in the makefile.

- ``LIGHT_MODE_MASK = 0x0f``

    This bitmask defines how many lights there are that should be switched on
    with a single click of the AUX/CH3 channel. Each time AUX is activated, more lights are turned on. If you have 2 lights (main beam and high beam for example), then you would specify ``0x03`` (0011 binary), if you have parking, main beam, high beam and fog lights you would specify ``0x0f`` (1111 binary), and so on. The mask defines basically the maximum value the ``light_mode`` variable (see above) will receive.

- ``CH3_MOMENTARY``

    Define this name if your transmitters AUX channel returns to the initial position when the AUX channel button is released (Futaba 4PL for example).

    If this name is not defined then the software assumes that every AUX channel activation toggles the AUX servo position (HobbyKing HK310, FlySky GT3B).

- ``ESC_FORWARD_REVERSE``

    Define this name when your ESC is in "crawler mode", i.e. it does not have a brake function but goes from forward to reverse immediately.

- ``DISABLE_BRAKE_DISARM_TIMEOUT``

    Define this name if your ESC goes into reverse only after applying the brake function once (Example: Tamiya TEU-105BK).

    Do not define this name for ESCs that automatically allow going into reverse when the throttle has been in neutral for a certain amount of time.

- ``ENABLE_STEERING_WHEEL_SERVO``

    Define this name if you want the light controller to drive a steering wheel servo. You must also define ``ENABLE_SERVO_OUTPUT`` and include ``servo-output`` in the list of files in the makefile.

- ``ENABLE_SERVO_OUTPUT``

    Define this name if you want the light controller to drive a servo. You must also include ``servo-output`` in the list of files in the makefile.

- ``ENABLE_UART``

    Define this name when using a UART related function such as uart-reader.

- ``NUMBER_OF_LEDS=`` 8

    Specifies the number of WS2812B or PL9823 LEDs in use. Only applies to the WS2812 variant of the light controller. The maximum number of LEDs currently supported is 21. This is a limitation of the RAM banks in the PIC16F architecture.

- ``ENABLE_GEARBOX``

    Define this name to enable control of a 2-speed gearbox using a servo. You must also define ``ENABLE_SERVO_OUTPUT`` and include ``servo-output`` in
    the list of files in the makefile.

- ``SERVO_OUTPUT_ON_THROTTLE``

    Define this name to output the servo signal on the throttle channel instead of the dedicated steering wheel servo output of the light controller. Only has an effect when ``ENABLE_SERVO_OUTPUT`` is enabled.

- ``ENABLE_WINCH``

    Enables control of a RC winch through using the [DIY RC Winch Controller](http://laneboysrc.blogspot.com/2013/09/make-your-own-rc-winch-controller.html).

- ``RECEIVER_OUTPUT_RATE = 20``

    Allows to specify the number of milliseconds between repeated servo pulses given by the receiver. If not specified the default of 16 ms is used. All this value really does is adjust the timing of initialization.

- ``SEQUENTIAL_CHANNEL_READING``

    Define this name when you are sure that the steering, throttle and AUX channel pulses are output by the receiver in sequence, right after each other. This is the case for the HobbyKing HKR3000 receiver. If unsure, do not define this value.

- ``CHANNEL_SEQUENCE_TH_ST_CH3``

    Define this along with ``SEQUENTIAL_CHANNEL_READING`` when the receiver pulse sequence throttle, steering, AUX; like in the GT3B.

- ``DUAL_TLC5916``

    Enables the use of two TLC5916 (for dim and full brightness). Not recommended, use TLC5940 hardware instead.

