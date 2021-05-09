# 3-ch Pre-Processor

This folder contains KiCAD files for the 3 channel Pre-Processor.

The original design used the TSSOP-16 package of the NXP LPC812 microcontroller.
Folder [LPC812-TSSOP16-version/](LPC812-TSSOP16-version/)

Since the TSSOP-20 version was easier available at distributors, we also designed a PCB for this mircocontroller: [LPC812-TSSOP20-version/](LPC812-TSSOP20-version/)
We never built this PCB yet ourselves but it is likely to work since it is just a different package.

# 2021 electronics shortage

As of early 2021, many electronics components are out of stock due to global semiconductor shortage. This affects also the LPC812 microcontroller in both TSSOP-16 and TSSOP-20 packages.

However the XSON-16 package was still available, so we redesigned the PCB for this package: [LPC812-XSON16-version/](LPC812-XSON16-version/)
XSON-16 has a pin-pitch of only 0.4 mm and is extremely difficult to solder in a hobbyist-environment using cheap solder paste and a modified toaster oven. A high number of boards have to be reworked, making it very labor intense to build the hardware.

As of April 2021 a microcontroller similar to the LPC812 from NXP, the LPC832M101FDH20, was still in stock at some distributors (but stocks declined towards mid of April rapidly). We prepared a new circuit board for the LPC832, and also ported the firmware.

As of 2021-04-20 the same firmware can be used for the LPC812 and LPC832 variant.

Note that the LPC81x_ISP programming tool does not support LPC832 yet, but the WebUSB programmer does.

