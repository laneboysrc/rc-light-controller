# Mk4P - light controller with pin headers

This is a new variant of the DIY RC Light Controller Mk4.
All outputs are available on a standard 2.54 mm pin header.

We are using the TSSOP-20 package of the LPC812 as it is easier to source (as of 2020) than the TSSOP-16 version, and we have plenty of space.

# 2021 electronics shortage

As of early 2021, many electronics components are out of stock due to global semiconductor shortage. This affects also the LPC812 microcontroller.

As of April 2021 a similar microcontroller from NXP, the LPC832M101FDH20 was still in stock at some distributors (but stocks declined towards mid of April rapidly). We prepared a new circuit board for the LPC832, and also ported the firmware.

As of 2021-04-20 the same firmware can be used for the LPC812 and LPC832 variant.

Note that the LPC81x_ISP programming tool does not support LPC832 yet, but the WebUSB programmer does.

