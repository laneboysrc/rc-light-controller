WORK IN PROGRESS -- DO NOT USE!

This is a programmer for the NXP LPC812 micro-controller, using the ISP (serial) protocol.
A Microchip (Atmel) ATSAMD21 MCU is used.

It connects to a PC or Android phone via USB and uses WebUSB (available in Chrom, Opera and Edge browsers) to communicate with a web app.


# Design goals

- Reset a connected light controller through switching its power supply
- OUT/ISP pin can be pulled-low for programming and released for running
- Custom WebUSB protocol using simple IN and OUT endpoints
- Be able to extend the protocol for use as Pre-Processor simulator
- Multiple LED indicating state


# Architecture considerations

- We don't want to put too much protocol details in the MCU
    - Which means we should separate the serial interface to the MCU from the control interface that controls power switching, OUT/ISP switching etc.

- UART interface
    - OUT endpoints sends data to Light Controller (Tx on the programmer)
    - IN endpoint sends data received from Light Controller (Rx on the programmer)

- Controler interface
    - We will be using USB control transfers
        - Type will be set to VENDOR
        - Request will be set to 72
        - wValue will contain the command
        - Commands can return data if necessary (using controlTransferIn)
            - Note that a command must be either out or in (SET or GET), it is not possible to use the same command with different directions
    - Commands:
        - OUT/ISP state: low, high, tri-state
        - DUT power: on, off
        - Baudrate
        - LEDs on, off

- LEDs
    - System power
    - Light controller power
    - Ok (green)
    - Busy (amber)
    - Error (red)


# Hardware

- SAMD21E15A
- Micro-USB connector
- LDO 3V3, 1uF on input, 47uF + some 100n on output
- STMPS2141 high-side power switch, active low
- Light controller is powered from 5V
- 5 LEDs
    - USB power: green
    - Light controller power: green
    - OK: green
    - Busy: amber
    - Error: red
- 6-pin header for light controller
    - Pin-out like Light controller or like USB-to-Serial?
- All pins series resistors for protection
    - How about OUT/ISP?