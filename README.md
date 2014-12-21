# Light controller for Radio Controlled cars

The objective is to achieve realistic lighting of RC model cars using a standard 3-channel radio control system.

The following functions have been implemented:

- Parking, Low-beam, High-beam etc can be switched on/off manually using the AUX (CH3) channel

- Brake and Reversing lights are automatically controlled by monitoring the throttle channel. Brake lights automatically turn on for a short, random
  time when the throttle goes to neutral.

- It is possible to have combined tail and brake light function using a single LED through controlling the brightness of the LED.

- Indicators only come on when you want to. You have to stay in neutral for one seconds, then hold the steering left/right for one second before they engage. This way normal driving does not trigger the indicators.

- Flashing hazard lights can be switched on/off using AUX/CH3

- Programmable servo output designed to drive a steering wheel or a figures head, or a 2-speed, 3-speed gearbox

- Automatic centre and end-point adjustment for all channels

- 16 LEDs can be driven by one light controller. Two controllers can be daisy-chained for a maximum of 32 LEDs (master-slave system).


A detailed introduction and videos can be found at our blog:

- [http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html](http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html)

- [http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html](http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html)

You may also want to read the [users guide](doc/light-controller-instructions.pdf) for a "generic" firmware that suits many vehicles.


## Hardware

Firmware and schematics in Eagle format for the different variants are included in this project, please consult the sub-folders of the respective variant.

Through the time the light controller went through several iterations.
**A detailed pro- and con- description of each variant can be found in the [hardware overview](doc/hardware-overview.md) document.**

Below is a brief overview of the current light controller variants:

### MK4 TLC5940 LPC812

![MK4 light controller in various state of assembly](doc/light-controller-mk4-tlc5940-lpc812.jpg)

[The MK4 variant](mk4-tlc5940-lpc812/) can drive 16 LEDs with a constant current. It utilises an inexpensive NXP ARM Cortex-M0 32-bit microcontroller.
The main advantage of this new design is that it can be fully configured through a web-based user interface. Firmware and configuration can be downloaded with a standard USB-to-serial converter, no proprietary tools required.


### MK3 WS812 PIC12F1840

[![The MK3 light controller on the left](https://farm6.staticflickr.com/5567/14791314828_801efd91f6_z.jpg)]("https://www.flickr.com/photos/78037110@N03/14791314828")
(MK3 on the left, MK2 on the right)

[This variant](mk3-ws2812b-pic12f1840/) drives the popular shift-register programmable LEDs [WS2812](https://www.adafruit.com/products/1655) are used, requiring only an inexpensive PIC12F1840 microcontroller to drive.
Very quick to build the hardware; supports colored lights; but mounting and wiring the LEDs may be tricky.


### MK2 TLC5940 PIC16F1825

[![Light controller on prototyping board and PCB](http://farm6.staticflickr.com/5321/9769284031_7576b9dbe0.jpg)](http://www.flickr.com/photos/78037110@N03/9769284031/)

The [PIC16F1825 variant](mk2-tlc5940-pic16f1825/) uses the TI TLC5940 LED driver and a a PIC16F1825 microcontroller. This system has been deployed in more than 20 vehicles and can be considered very robust.
It has also been built by several fellow RC enthusiasts all over the world on simple prototyping boards.


### MK1 TLC5916 PIC628a

[![The first light controller we ever built](https://farm8.staticflickr.com/7123/7675556184_958a45b5c5_z.jpg)](https://www.flickr.com/photos/78037110@N03/7675556184)

The project also includes information on the original design using a PIC16F628A and TLC5916. I do not recommend this design for new projects as the components are outdated, functionality is limited and the parts are more expensive than the other variants.


## Related articles

- [http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html](http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html)

- [http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html](http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html)

- [http://laneboysrc.blogspot.com/2012/12/pre-processor-for-diy-rc-light.html](http://laneboysrc.blogspot.com/2012/12/pre-processor-for-diy-rc-light.html)

- [http://laneboysrc.blogspot.com/2013/01/pre-processor-miniaturization.html](http://laneboysrc.blogspot.com/2013/01/pre-processor-miniaturization.html)

- [http://laneboysrc.blogspot.com/2014/07/ws2812-and-pl9823-led-power-consumption.html](http://laneboysrc.blogspot.com/2014/07/ws2812-and-pl9823-led-power-consumption.html)

- [http://laneboysrc.blogspot.com/2014/08/diy-rc-light-controller-with-ws2812b.html](http://laneboysrc.blogspot.com/2014/08/diy-rc-light-controller-with-ws2812b.html)
