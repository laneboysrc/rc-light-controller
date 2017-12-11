

Compared to LPC812 we have different restrictions:

LPC812
- Servo reader: ST/Rx, TH/Tx, CH3, Out/Tx
    - Diagnostics: via Out/Tx if not used as Servo or UART out
    - Button: via CH3
- UART reader: Rx, TH/Tx, Out
    - Diagnostics: Via TH/Tx if not used as UART out
    - Button: via CH3

SAMD21
- PCB: Rx, Out/Tx, Button, LED
    - Diagnostics: always via USB; via Out/Tx if not used as Servo or UART out
    - Button: via button
- Arduino based: ST, TH, CH3, Rx, Tx, Out, Button, LED
    - Diagnostics: always via USB; via Tx if not used as UART out
    - Button: via button

Arduino uses SERCOM5, which is not available on E15 chip.
We don't have to worry about Arduino SPI fixed pins as long as they are available on GPIOs, because we have no intention to use the Arduino library.


How to detect whether we have an Arduino or a PCB version?
- GPIO pattern <===
    - Short two unused GPIO together
- Detect SAMD21E15, E18?
    - Will not work if people "upgrade" the chip on the PCB
- Via 48 pin package?
    - Will work for most boards that use 48 pins, except for the "Tau" that uses an E17



ST/TH/CH3:  should be on same timer WO[x] pins          TCC1: PA6, PA7, PA8, PA9, PA10, PA11
Out/Tx:     SERCOM/PAD[0,2], Timer CC WO[x] pin         Arduino: PB22 SERCOM5, TCC7
Rx:         SERCOM/PAD[0..3]                            Arduino: PB23 SERCOM5
SPI:        MOSI SERCOM/PAD[0],                         Arduino: PA16 SERCOM1
            SCLK SERCOM/PAD[1],                         Arduino: PA17 SERCOM1
IO ports:   GSCLK, XLAT, BLANK, BUTTON, LED15


UART Rx PAD0..3     PA00=S1P0, PA01=S1P1, PA04=S0P0, PA05=S0P1, PA06=S0P2, PA07=S0P3, PA08=S0P0, PA08=S2P0, PA09=S0P1, PA09=S2P1, PA10=S0P2, PA10=S2P2, PA11=S0P3, PA11=S2P3, PA14=S2P2, PA14=S4P2, PA15=S2P3, PA15=S4P3, PA16=S1P0, PA16=S3P0, PA17=S1P1, PA17=S3P1, PA18=S1P2, PA18=S3P2, PA19=S1P3, PA19=S3P3, PA22=S3P0, PA22=S5P0, PA23=S3P1, PA23=S5P1, PB02=S5P0, PB03=S5P1

UART Tx PAD0,2      PA00=S1P0, PA04=S0P0, PA06=S0P2, PA08=S0P0, PA08=S2P0, PA10=S0P2, PA10=S2P2, PA14=S2P2, PA14=S4P2, PA16=S1P0, PA16=S3P0, PA18=S1P2, PA18=S3P2, PA22=S3P0, PA22=S5P0, PB02=S5P0

SPI SCK PAD1        PA01=S1P1, PA05=S0P1, PA09=S0P1, PA09=S2P1, PA17=S1P1, PA17=S3P1, PA23=S3P1, PA23=S5P1, PB03=S5P1
SPI MOSI PAD0       PA00=S1P0, PA04=S0P0, PA08=S0P0, PA08=S2P0, PA16=S1P0, PA16=S3P0, PA22=S3P0, PA22=S5P0, PB02=S5P0,
TCC0:               PA04=0, PA05=1, PA08=0, PA09=1, PA14=4, PA15=5, PA16=6, PA17=7, PA18=2, PA19=3, PA22=4, PA23=5
TCC1:               PA06=0, PA07=1, PA10=0, PA11=1
TCC2:               PA00=0, PA01=1, PA12=0, PA13=1, PA16=0, PA17=1


So we ideally want TCC0 for ST/TH/CH3 as the other timers only have two pads.

SPI: PA04/PA05 SERCOM0, rest of TLC5940 signals on P00..P07
UART: PA16/PA23 SERCOM3, OUT/TX = PA16/TCC2




    Arduino:
    ST/TH/CH3 on pads 17/18/19, all TCC0
    Rx=23, Tx=22 SERCOM3
    OUT=16 TCC2
    SPI PA04/05 SERCOM0


# SAM R21 Xplained Pro

[USART_TX]      PA04 SERCOM0 PAD[0]  / UART TX EDBG
[USART_RX]      PA05 SERCOM0 PAD[1]  / UART RX EDBG
[ADC(+)]        PA06 AIN[6]
[SPI_SS_B/GPIO] PA08 GPIO            / EDBG                       SERCOM2/PAD[0]
[SPI_SS_A]      PA14 GPIO            / EDBG                       SERCOM2/PAD[2]
[GPIO1]         PA15 GPIO                                         SERCOM2/PAD[3]
[TWI_SDA]       PA16 SERCOM1 PAD[0] I2C SDA     EXT1 and EDBG     SERCOM1/PAD[0]
[TWI_SCL]       PA17 SERCOM1 PAD[1] I2C SCL     EXT1 and EDBG     SERCOM1/PAD[1]
[PWM(+)]        PA18 TCC0 / WO[2]                                 SERCOM1/PAD[2]
[PWM(-)]        PA19 Yellow LED / TCC0 / WO[3]
[IRQ/GPIO]      PA22 EXTINT[6]                                    SERCOM3/PAD[0]
[SPI_SS_B/GPIO] PA23 GPIO1                                        SERCOM3/PAD[1]
PA24            USB D-
PA25            USB D+
[GPIO2]         PA28 GPIO

In use:
PA00            XIN32
PA01            XOUT32
PA07            VBUS Detection, [ADC(-)]
PA09            RFCTRL1, negative antenna switch control signal
PA27            GPIO SPI SS (Slave select) (SAM R21 is Master)
PA28            SW0
PA27            GPIO, chip select on the EDBG DGI SPI bus
PA30            SWCLK
PA31            SWDIO

Not on SAMD21E:
[GPIO1]         PA13 GPIO1
PA12            RFCTRL2, positive antenna switch control signal
[SPI_MOSI]      PB22 SERCOM5 PAD[2] SPI MOSI    EXT1 and EDBG
[SPI_SCK]       PB23 SERCOM5 PAD[3] SPI SCK     EXT3 and EDBG
[SPI_MISO]      PB02 SERCOM5 PAD[0] SPI MISO    EXT3 and EDBG
[SPI_SS_A]      PB03 SERCOM5 PAD[1] SPI SS



