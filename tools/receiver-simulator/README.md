# Receiver simulator

This test code uses a Microchip (Atmel) SAMD21 micro-controller to simulate a 2-channel receiver.

It was designed to run on an SAMR21 Xplained Pro board (because this is what I had lying around), but can be easily adapted to other SAMD21 boards.

The current I/O definition can be found in hal.h.

- The UART prints diagnostics output.
- Two pins should be connected to potentimeters to control ST and TH channels.
- Two other pins output 1000-2000 us servo pulses depending on the potentiometer position.


The receiver has one special feature that the ST and TH output run asynchronously and at different fequencies. TH outputs every 32 ms, ST ever 16ms.
This was done because it seems to mimic the behaviour of the Spektrum 4210 receiver. This behaviour is quite unusual and caused an issue with flickering brake lights.

