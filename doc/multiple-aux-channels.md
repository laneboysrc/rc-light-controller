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
PIO0_10 and PIO0__11 can be used as inputs, but a pull-up or pull-down resistor is required as the are true open drain pins.

In theory we can therefore support a maximum of 4 AUX channels.

1) Adding 3 more AUX channels, each on its own standard 3-pin servo header, would add about 7.6mm to the preprocessor (29 mm instead of 21 mm)
2) Adding 2 AUX channels on a standard 3-pin servo header would add 5.1 mm (26 mm instead of 21 mm)
3) Adding 1 AUX channel. This would add 2.54 mm (24mm instead of 21 mm)
4) Adding 3 AUX channels on one 3-pin header (signal only) would add 2.54 mm (24mm instead of 21 mm)


Option 4 provides the most inputs at the least additional space, but has the disadvantage that the new AUX pin header has a different pin-out. If the user accidentally plugs in a receiver channel into the AUX pin header the MCU may get damaged (plus pole on middle pin!) The user also must use 1 pole Dupont pins, which don't hold well mechanically and may come off too easily.

Option 1 makes the preprocessor very large, and may get into mechanical issues with the servo plugs, which extend slightly on the top.

Therefore it may be best to go for option 2: 2 additional AUX channels on a standard 3-pin servo header => 5-channel preprocessor

## Features

* Fully backwards compatible with the light controller software
* Configurable functions, using UI method from Betaflight
    * Toggle function as per today's CH3/AUX
    * Gearbox function on 2/3 pos switch
    * Winch on 3 pos switch (less interesting; better to use winch separate!)
    * Manual indicators on 3 pos switch
    * Hazard lights on 2 pos switch
    * Light switch (n positions?)

## System impact

* Can we extend the preprocessor protocol easily with 2 more channels?
* One additional AUX channel could be handled with the 4th capture input. For further inputs we need to multiplex the respective input


