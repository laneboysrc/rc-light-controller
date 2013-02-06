DIY light controller for Radio controlled cars
==============================================

This project is for a light controller for Radio Controlled cars.

The objective is to achieve realistic lighting of RC model cars using a 
standard 3-channel radio control system. Some functions like brake lights,
reversing lights and indicators are automatic, other functions like
switching on the main lights and hazard lights are manually controlled 
through channel 3.

The light controller also can drive an independent servo connected to the
steering wheel in the interior of the car for added realism.

A detailed introduction can be found at our blog:

    * http://laneboysrc.blogspot.sg/2012/07/diy-car-light-controller-for-3-channel.html


It makes uses of a PIC16F1825 microcontroller and TLC5940 LED driver. An older 
variant using the PIC16F628A and TLC5916 is available too.

Firmware and schematics in Eagle format are included.

To compile the PIC firmware you need:

    * GNU Make
    
    * gputils (Version 1.0.0; http://gputils.sourceforge.net/)


Related articles:

    * http://laneboysrc.blogspot.sg/2012/12/pre-processor-for-diy-rc-light.html
    
    * http://laneboysrc.blogspot.sg/2013/01/pre-processor-miniaturization.html

