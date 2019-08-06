# Request to support multiple AUX channels

Modern RC car transmitter support 4 or 5 channels. A few light controller users issued the request to support these additional AUX channels for light switching function, as to make operation easier than the current clicking of CH3/AUX.


## Discussion

The easiest way to implement this request is via a new version of the preprocessor.

In order to keep software-compatibility with the light controller, we can only use pins that are currently unused:

PIO0_5  (3, RESET)
PIO0_8  (11, XTAL-IN)
PIO0_10 (8, open drain)
PIO0_11 (7, open drain)

For safety reasons we will not use the reset pin.

PIO0_8 can be used as normal IO and is therefore ideal.
PIO0_10 and PIO0__11 can be used as inputs, but a pull-up or pull-down resistor is required as they are true open drain pins.

In theory we can therefore support a maximum of 4 AUX channels.

1) Adding 3 more AUX channels, each on its own standard 3-pin servo header, would add about 7.6mm to the preprocessor (29 mm instead of 21 mm)
2) Adding 2 AUX channels on a standard 3-pin servo header would add 5.1 mm (26 mm instead of 21 mm)
3) Adding 1 AUX channel. This would add 2.54 mm (24mm instead of 21 mm)
4) Adding 3 AUX channels on one 3-pin header (signal only) would add 2.54 mm (24mm instead of 21 mm)


Option 4 provides the most inputs at the least additional space, but has the disadvantage that the new AUX pin header has a different pin-out. If the user accidentally plugs in a receiver channel into the AUX pin header the MCU may get damaged (plus pole on middle pin!) The user also must use 1 pole Dupont pins, which don't hold well mechanically and may come off too easily.

Option 1 makes the preprocessor very large, and may get into mechanical issues with the servo plugs, which extend slightly on the top.

Therefore it may be best to go for option 2: 2 additional AUX channels on a standard 3-pin servo header => 5-channel preprocessor

AUX2: PIO0_8 (11, XTAL-IN)
AUX3: PIO0_11 (7, open drain)

## Features

* Fully backwards compatible with the light controller software
* Configurable functions, using UI method from Betaflight
    * Toggle function as per today's CH3/AUX
    * Gearbox function 1/2/3 (depends on gear configuration)
    * Winch off/in/out
    * Manual indicators left/off/right
    * Hazard lights on/off
    * Light switch (servo range mapped to n positions)
* Supported actuators:
    * Two-position switch
    * Two-position switch with up/down buttons
    * Momentary push button
    * Three-position switch
    * Analog or multi-position switch
* Light programs can read AUX position


Two-position switch, Two-position switch with up/down buttons and Momentary push button are technically all of type two-position actuator.

```
       | Toggle   Gear   Winch   Ind   Hazard   Light
       | --------------------------------------------
2-pos  | toggle   gear1  winch1  N/A   on/off   N/A
3-pos  | N/A      gear2  winch2  ind   on/off   light2
n-pos  | N/A      gear2  winch2  ind   on/off   light2
```

gear1: if 2 gears, direct selection; if 3 gears: 1-click=up, 2-click=down
gear2: direct selection with hysteresis
winch1: as per current operation
winch2: direct off/in/out operation
light2: the servo range 1000..2000us is mapped to n light switch sections. Note that for 3-pos switch this could mean that not all are adjustable.

Note that the combination of 2-pos and Light is not used (N/A) because we rather apply the full toggle operation in that case.

All switch actions are only executed upon switch change. This allows e.g. controlling the hazard lights from both the usual 2-pos toggle function, but also from a manual switch at the same time. Or there can be multiple switches assigned to 'toggle'.
After initialization the manual functions are applied immediately.

When the preprocessor input is active, the light controller always supports a hardware push-button the CH3/AUX input. It always serves as 'toggle' button for all functions.

When indicator control is assigned to an AUX channel then automatic indicators are disabled.


## Configurator user interface

For each AUX channel, the user can specify the actuator type and which of the available functions it should control.

We need to add preprocessor and multi-aux preprocessor to the configuration list, so we can show the appropriate number of AUX channels.


## TODO

* Can we extend the preprocessor protocol easily with 2 (3) more channels?
    * Yes, by setting the unused bit 3 in the 4th byte we can signal that more data follows. Old light controllers ignore that bit (incl. ancient MK2) and the trailing data.
    * We send 3 additional bytes for AUX, AUX2 and AUX3. Data is always -100..100  corresponding to 1000..2000 us pulse width (= no auto EPA)
    * Need to update the preprocessor-reader and preprocessor-simulator tools
* Can 34800 BAUD support the additional data?
    * Yes, 7 Bytes take only 200 us @38400 BAUD
* One additional AUX channel could be handled with the 4th capture input. For further inputs we need to multiplex the respective input
* How to setup the light controller if there is no CH3/AUX toggle function assigned?
    - We always do local button on CH3 if master with uart-reader
* Arbitration of functions

