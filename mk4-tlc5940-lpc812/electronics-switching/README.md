# Mk4S - light controller with pin headers and 9 switching outputs

This is a new variant of the DIY RC Light Controller Mk4.
It has 9 (Mosfet) switched outputs on standard 2.54 mm pin headers, 8 of them with two connections and one of them with 1 connection.

We are using the TSSOP-20 package of the LPC812 as it is easier to source (as of 2020) than the TSSOP-16 version, and we have plenty of space.

# 2021 electronics shortage

As of early 2021, many electronics components are out of stock due to global semiconductor shortage. This affects also the LPC812 microcontroller.

As of April 2021 a similar microcontroller from NXP, the LPC832M101FDH20 was still in stock at some distributors (but stocks declined towards mid of April rapidly). We prepared a new circuit board for the LPC832, and also ported the firmware.

**IMPORTANT: As of July 2021 the firmware has not been adapted to the LPC832 (for the switching version). The hardware has not been tested**