This is a programmer for the NXP LPC812 micro-controller, using the ISP (serial) protocol.

It connects to a PC or Android phone via USB and uses WebUSB (available in Chrome, Opera and Edge browsers) to communicate with a web app.

The great thing about this programmer is that no driver or software needs to be installed. Just connect the WebUSB programmer to the PC, load the Light Controller Configurator in the web browser, and program your light controller.


The current revision is REV4. This revision can work with the CH551G or CH552G microcontroller from WCH. REV3 was working fine, but due to the *chip crisis* of 2021 it was not possible to get hold of the ST power switch anywhere. So we re-designed the electronics to use a very cheap motor driver to switch the power to the light controller. The firmware was also modified to use UART0, because only UART0 is available on CH551G.

REV3 was the first version using the inexpensive CH552G microcontroller from WCH that has an 8051 core and USB device capability. The BOM has been reduced to the minimum so that long-term we can provide the WebUSB programmers instead of USB-to-Serial adapters. The LEDs are now side mounted so they can be viewed from both sides of the PCB.

In REV1 and REV2 a Microchip (Atmel) ATSAMD21 MCU was used, as we had stock of these. However, this is quite an expensive MCU (at least ten times the price of the CH552!), and needs additional peripheral electronics like an LDO. And it uses a 0.5mm QFN package which requires a stencil, solderpaste and reflow equipment.

REV1 used a surface mount Micro-USB connector (because we had it in stock). However, several people broke the connector as it is mechanically not very robust. Furthermore, the white LED with its soft diffuser is also fragile and can get damaged when handling or shipping.

In REV2 we changed the Micro-USB connector to one with through-hole mounting tabs, and use "reverse mount" LEDs (actually side view LEDs abused in a reverse mount fashion). REV2 worked fine except that the footprint for the Micro-USB connector is not optimal and the hole for the LEDs is slightly too small.

