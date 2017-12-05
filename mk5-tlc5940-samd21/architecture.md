
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


### IGNORE THIS, it is for SAMD11:


    QFN24   SOIC20  I/O        Type  EIC        ADC     AC      PTC        DAC   SERCOM  ALT     TC/TCC      TCC         COM
    1       18      PA02             EXTINT[2]  AIN[0]          Y[0]       Vout
    2       19      PA03             EXTINT[3]  AIN[1]          Y[1]
    3       20      PA04             EXTINT[4]  AIN[2]  AIN[0]  Y[2]             S0P2    S0P0    TC1/WO[0]   TCC0/WO[0]
    4       1       PA05             EXTINT[5]  AIN[3]  AIN[1]  Y[3]             S0P3    S0P1    TC1/WO[0]   TCC0/WO[0]
    5       2       PA06             EXTINT[6]  AIN[4]  AIN[2]  Y[4]             S0P0    S0P2    TC2/WO[0]   TCC0/WO[2]
    6       3       PA07             EXTINT[7]  AIN[5]  AIN[3]  Y[5]             S0P1    S0P3    TC2/WO[1]   TCC0/WO[3]
    7       4       PA08             EXTINT[6]                                   S1P2    S0P2    TCC0/WO[2]  TCC0/WO[4]
    8       5       PA09             EXTINT[7]                                   S1P3    S0P3    TCC0/WO[3]  TCC0/WO[5]
    11      6       PA14       I2C   NMI        AIN[6]  CMP[0]  X[0]/Y[6]        S0P0    S2P0    TC1/WO[0]   TCC0/WO[0]
    12      7       PA15       I2C   EXTINT[1]  AIN[7]  CMP[1]  X[1]/Y[7]        S0P1    S2P1    TC1/WO[1]   TCC0/WO[1]
    13      8       PA16             EXTINT[0]                  X[4]/Y[10]       S1P2    S2P2    TC1/WO[0]   TCC0/WO[6]
    15      9       PA22       I2C   EXTINT[6]                  X[6]/Y[12]       S1P0    S2P0    TC1/WO[0]   TCC0/WO[4]
    16      10      PA23       I2C   EXTINT[7]                  X[7]/Y[13]       S1P1    S2P1    TC1/WO[1]   TCC0/WO[5]
    21      14      PA24             EXTINT[4]                  X[8]/Y[14]       S1P2    S2P2    TCC0/WO[2]  TCC0/WO[4]  USB/DM
    22      15      PA25             EXTINT[5]                  X[9]/Y[15]       S1P3    S2P3    TCC0/WO[3]              USB/DP
    18      11      PA28/RST
    19      12      PA30             EXTINT[2]                                   S1P0    S1P2    TC2/WO[0]   TCC0/WO[2]  SWCLK
    20      13      PA31             EXTINT[3]                                   S1P1    S1P3    TC2/WO[1]   TCC0/WO[3]  SWDIO


UART Tx can be on:      PAD0, PAD2
UART Rx can be on:      PAD0, PAD1, PAD2, PAD3


USB             PA24, PA25
SWDIO           PA30, PA31

SIN             PA04 (S0P0)                             P0    S0: PA04, PA06, PA14    S1: PA22           S2: PA14, PA22
SCK             PA05 (S0P1)                             P1    S0: PA05, PA07, PA15    S1: PA23           S2: PA15, PA23
XLAT            PA07   FIXME: Should be S0P2!
BLANK           PA08
GSCLK           PA09

ST/Rx           PA15 (EXTINT[1]¸ S2P1, TCC0/WO[1])      P1    S0: PA05, PA07, PA15    S1: PA23           S2: PA15, PA23
TH/Tx           PA22 (EXTINT[6], S2P0, TCC0/WO[4])      P0    S0: PA04, PA06, PA14    S1: PA22           S2: PA14, PA22
CH3             PA23 (EXTINT[7],       TCC0/WO[5])
OUT             PA14 (           S2P0. TC1/WO[0])       P0    S0: PA04, PA06, PA14    S1: PA22           S2: PA14, PA22

OUT15/LED       PA03
Button          PA02





