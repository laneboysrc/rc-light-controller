# DIY light controller for Radio controlled cars

This project is for a light controller for radio controlled cars.

The objective is to achieve realistic lighting of RC model cars using a 
standard 3-channel radio control system. 

The following functions have been implemented:

- Parking, Low-beam, High-beam etc can be switched on/off 
  manually using the AUX (CH3) channel
- Brake and Reverse lights are automatically controlled by monitoring the 
  throttle channel. Brake lights automatically turn on for a short, random 
  time when the throttle goes to neutral.
- It is possible to have combined tail and brake light function using 
  a single LED through controlling the brightness of the LED.
- Indicators only come on when you want to. You have to stay in neutral for 
  one seconds, then hold the steering left/right for one second before 
  they engage. This way normal driving does not trigger the indicators.
- Flashing hazard lights can be switched on/off using AUX/CH3
- Programmable servo output designed to drive a steering wheel or a 
  figures head
- Programmable servo output for a 2-speed gearbox
- Various customizable light patterns for a roof light bar
- Automatic centre and end-point adjustment for all channels


A detailed introduction and videos can be found at our blog:

- [http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html](http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html)
- [http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html](http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html)

You may also want to read the [users guide](doc/light-controller-instructions.pdf) 
for a "generic" firmware that suits many vehicles.


## Hardware

There are several iterations of the light controller electronics.

In the latest variant the popular shift-register programmable LEDs 
[WS2812](https://www.adafruit.com/products/1655) are used, requiring only
an inexpensive PIC12F1840 microcontroller to drive. 

An earlier but still useful design used the TLC5940 LED driver and a 
a PIC16F1825 microcontroller. This system has been deployed in more than
10 vehicles and can be considered very robust.

[![Light controller on prototyping board and PCB](http://farm6.staticflickr.com/5321/9769284031_7576b9dbe0.jpg)](http://www.flickr.com/photos/78037110@N03/9769284031/)

The project also includes information on the original design using a PIC16F628A 
and TLC5916. I do not recommend this design for new projects.

Detailed information about the variants can be found in the 
[hardware overview](doc/hardware-overview.md) document.

Firmware and schematics in Eagle format for the different variants are [included
in this project](electronics/).


## Firmware

The project is written in Assembler.
To compile the PIC firmware you need:

- GNU Make ([https://www.gnu.org/software/make/](https://www.gnu.org/software/make/); 
  Windows executable is available at [http://gnuwin32.sourceforge.net/packages/make.htm](http://gnuwin32.sourceforge.net/packages/make.htm))
    
- gputils (Version 1.0.0; [http://gputils.sourceforge.net/](http://gputils.sourceforge.net/))

   
An overview of the software can be found in the 
[firmware architecture](doc/firmware-architecture.md) document. This document
also explains how to get started with customizing the lights for a new RC car.

>**Attention Windows users:**
>
> Ensure that the PATH environment variable points to the *make.exe* and 
> *gpasm.exe* executables.
>
> Run *cmd.exe*; type **make -v**. You should see a message originating
> from GNU make. Ensure that the message is indeed from GNU make, not from
> another make utility that you may have installed.
> Type **gpasm -v** to check that *gputils* is correctly installed.


## Related articles

- [http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html](http://laneboysrc.blogspot.com/2012/07/diy-car-light-controller-for-3-channel.html)
- [http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html](http://laneboysrc.blogspot.com/2013/09/diy-rc-light-controller-update.html)
- [http://laneboysrc.blogspot.com/2012/12/pre-processor-for-diy-rc-light.html](http://laneboysrc.blogspot.com/2012/12/pre-processor-for-diy-rc-light.html)
- [http://laneboysrc.blogspot.com/2013/01/pre-processor-miniaturization.html](http://laneboysrc.blogspot.com/2013/01/pre-processor-miniaturization.html)
- [http://laneboysrc.blogspot.com/2014/07/ws2812-and-pl9823-led-power-consumption.html](http://laneboysrc.blogspot.com/2014/07/ws2812-and-pl9823-led-power-consumption.html)

