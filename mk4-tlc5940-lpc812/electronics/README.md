# Mk4 - light controller with solder pads

This is the original version of the Mk4 light controller.
Multiple PCB versions are available for different microcontrollers.

The LPC812-TSSOP16 version is the original design.

pcb-size-27x20 uses the LPC812-TSSOP16 and was created to get around the minimum assembly width requirement for 3rd party manufacture. Not recommended.

The LPC812-XSON16 version uses a tiny case for the microcontroller, which is extremely difficult to work with unless you have professional equipment.

The LPC812-TSSOP20 was never built and tested.

The LPC832-TSSOP20 version is not tested yet, but most likely will work as we successfully ported the Mk4P (basically same schematics) already to the LPC832.


Background info: we ported to the different microcontroller variants to get around the global chip shortage in 2020/2021.