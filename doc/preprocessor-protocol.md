# Preprocessor UART protocol

The preprocessor runs at either 38400 or 115200.

Usually 115200 should be used, but 38400 is still available to use nRF24LE1 which can only output at 38400. Also use of the LANE Boys RC winch controller requires 38400.


## Protocol details

The first byte is always 0x87, which indicates that it is a start byte. No other byte can have this value.
Note: values 0x80..0x87 do not appear in the other bytes by design, so those can be used as first byte to indicate start of a packet.

The second byte is a signed char of the steering channel, from -100 to 0 (Neutral) to +100, corresponding to the percentage of steering left/right.

The third byte is a signed char of the throttle channel, from -100 to 0 (Neutral) to +100.

The fourth byte holds CH3 position information in bit 0, and bit 4 indicates whether the preprocessor is initializing.
Note that bits 5..7 must be zero for backwards compatibility with the MK2 lightcontroller.
Bits 1..3 are unused and are set to 0 by the preprocessor.

Example:

    [0x87] [ST] [TH] [0b000_I_000_C]

Where *ST* is the steering value -100..0..+100%, *TH* is the throttle value -100..0..+100%, *I* is 1 when the preprocessor is initializing, otherwise 0; and *C* indicates the position of the CH3 two-position switch.


## Extension for 5-channel radios

Given the description of the legacy protocol, we can use any of the unused bits 1..3 in the 4th byte of the transfer to indicate whether additional channel follow.

We choose bit 3: when set to 1 the light controller knows to expect data to follow for additional AUX channels.
Furthermore, we also add normalized data for CH3, so that CH3/AUX can be used with any of the new functions introduced with the multi-aux-channel support (e.g. 3-position switch ...).

The values appended for the AUX channel are signed char with the range of -100..0..+100. The channels are normalized to a corresponding servo signal of 1000..1500..2000 us.
Note that this normalization is different than ST and TH, where the normalization is dynamic, as to compensate for end-point adjustments in the transmitter.

Example:

    [0x87] [ST] [TH] [0b000_I_100_C] [CH3] [AUX2] [AUX3]

The first 3 bytes are identical as described above.
The 4th byte has bit 3 set to 1 to indicate that more channels follow.
CH3 is the normalized value for the CH3/AUX servo signal in the range of -100..0..+100%.
Likewise, AUX2 and AUX3 represent the normalized values for those inputs, range -100..0..+100%.

Light controllers who don't understand the extended format will continue to function properly and simply ignore the CH3, AUX2 and AUX3 values.


Note that 7 Bytes at 38400 BAUD take about 200 us to transfer, so no issue timing-wise even at high servo refresh rates.


## HK310 expansion protocol

Note: this section is obsolete, it was an experiment to transmit addition data via the CH3 information without having to change the transmitter firmware. Please ignore.

The protocol was extended with a 5th byte, holding the normalized 6-bit value of CH3 as used in the HK310 expansion protocol. The light controller simply ignored this data.