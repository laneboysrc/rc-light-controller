# i-Bus

FlySky now offers surface receivers that use the new AFHDS3 protocol and support i-Bus output.

i-Bus support in the Light Controller allows users to use a single servo cable between receive and Light Controller, without the need for a Pre-Processor.

## i-Bus specification

Unfortunately there does not appear to be a formal specification of i-Bus.

To make things worse, in February 2020 FlySky modified the data format of how i-Bus transmits servo information to cram an additional 4 channels into the same packet. This change breaks many existing i-Bus library, e.g. on Arduino.

## i-Bus, as LANE Boys RC understands it

Anyway, the Light Controller supports i-Bus protocol with the following understanding of the data structure:

* The baudrate is 115200
* Packets containing servo information are sent at regular intervals of about 7-8ms
* When the connection with the Transmitter is broken, the receiver stops outputing servo packets
* A servo packet is up to 32 bytes long and comprises the following information
    * 1 byte packet length
    * 1 byte packet ID
    * Up to 28 bytes of payload
    * 2 bytes checksum (little-endian uint16_t)
* The packet ID for servo packets is 0x40
* The checksum is the sum of all bytes (!) XOR 0xffff
* The servo payload is as follows:
    * Byte 3 (first payload byte): CH1[0..7]
    * Byte 4 CH15[0..3], CH1[8..11]
    * Byte 5 CH2[0..7]
    * Byte 6 CH15[4..7], CH2[8..11]
    * Byte 7 CH3[0..7]
    * Byte 8 CH15[8..11], CH3[8..11]
    * Byte 9 CH4[0..7]
    * Byte 10 CH16[0..3], CH4[8..11]
    * Byte 11 CH5[0..7]
    * Byte 12 CH16[4..7], CH5[8..11]
    * Byte 13 CH6[0..7]
    * Byte 14 CH16[8..11], CH4[8..11]
    * Byte 15 CH7[0..7]
    * Byte 16 CH17[0..3], CH7[8..11]
    * Byte 17 CH8[0..7]
    * Byte 18 CH17[4..7], CH8[8..11]
    * Byte 19 CH9[0..7]
    * Byte 20 CH17[8..11], CH9[8..11]
    * Byte 21 CH10[0..7]
    * Byte 22 CH18[0..3], CH10[8..11]
    * Byte 23 CH11[0..7]
    * Byte 24 CH18[4..7], CH11[8..11]
    * Byte 25 CH12[0..7]
    * Byte 26 CH18[8..11], CH12[8..11]
    * Byte 27 CH13[0..7]
    * Byte 28 CH13[8..11]
    * Byte 29 CH14[0..7]
    * Byte 30 CH14[8..11]
    Channel values describe the servo pulse duration in microseconds (e.g. 1500 = 0x5dc = center)


## Example packet

(Source: https://basejunction.wordpress.com/2015/08/23/en-flysky-i6-14-channels-part1):

```
0x20 0x40 0xdc 0x05 0xdb 0x05 0xef 0x03 0xdd 0x05 0xd0 0x07 0xd0 0x07 0xdc 0x05 0xdc 0x05 0xdc 0x05 0xdc 0x05 0xdc 0x05 0xdc 0x05 0xdc 0x05 0xdc 0x05 0x54 0xf3
```

We have received a capture from a Saleae logic analyzer from a kind light controller user.

*i-bus_capture_from_flysky_receiver_saleae_format.sal*
Is the captured file, can be opened in the Saleae "logic" tool.

*i-bus_capture_from_actual_receiver_in_salee_format.bin*
Contains the i-Bus serial communication exported by "logic" in "binary format". See https://support.saleae.com/faq/technical-faq/binary-export-format-logic-2 for information how to decode this format.

*i-bus_decoder_for_saleae_format.py* is a tool that decodes the binary format expecting a UART signal at 115200 BAUD n-8-1

*i-bus_decoded_packets.py_or_js* is the result of running *i-bus_capture_from_actual_receiver_in_salee_format.bin* through *i-bus_decoder_for_saleae_format.py*. It contains the i-Bus packets as captured from the FlySky receiver. It has movement data on the first 4 channels.


## Open points:

* Is the servo packet always 32 bytes long, or can there be less servo data if the transmitter supports less than 14 channels?
    * Our implementation supports variable length servo packets up to a maximum of 32 bytes.
    * The minimum servo packet length must be 6 (payload count, packet ID, checksum, 2 bytes for 1 channel)
    * The servo packet length must be even

* What is the valid range of channel values?
    * We follow the same values as we use for our "servo reader"
