

        I/O        Type  EIC        ADC     AC      PTC        DAC   SERCOM   ALT      TC/TCC      TCC         COM
     1  PA05             EXTINT[5]  AIN[3]  AIN[1]  Y[3]             S0/P[3]  S0/P[1]  TC1/WO[0]   TCC0/WO[0]
     2  PA08             EXTINT[6]                                   S1/P[2]  S0/P[2]  TCC0/WO[2]  TCC0/WO[4]
     3  PA09             EXTINT[7]                                   S1/P[3]  S0/P[3]  TCC0/WO[3]  TCC0/WO[5]
     4  PA14       I2C   NMI        AIN[6]  CMP[0]  X[0]/Y[6]        S0/P[0]  S2/P[0]  TC1/WO[0]   TCC0/WO[0]
     5  PA15       I2C   EXTINT[1]  AIN[7]  CMP[1]  X[1]/Y[7]        S0/P[1]  S2/P[1]  TC1/WO[1]   TCC0/WO[1]
     6  PA28/RST
     7  PA30             EXTINT[2]                                   S1/P[0]  S1/P[2]  TC2/WO[0]   TCC0/WO[2]  SWCLK
     8  PA31             EXTINT[3]                                   S1/P[1]  S1/P[3]  TC2/WO[1]   TCC0/WO[3]  SWDIO
     9  PA24             EXTINT[4]                  X[8]/Y[14]       S1/P[2]  S2/P[2]  TCC0/WO[2]  TCC0/WO[4]  USB/DM
    10  PA25             EXTINT[5]                  X[9]/Y[15]       S1/P[3]  S2/P[3]  TCC0/WO[3]              USB/DP
    11  GND
    12  Vdd
    13  PA02             EXTINT[2]  AIN[0]          Y[0]       Vout
    14  PA04             EXTINT[4]  AIN[2]  AIN[0]                   S0/P[2]  S0/P[0]  TC1/WO[0]   TCC0/WO[0]



USB:            PA24, PA25
BLANK           PA28/RST
GSCLK/Button    PA05
SCK             PA31 (S1/P1, SWDIO)
DATA            PA30 (S1/P0, SWCLK)
XLAT            PA09 (S1/P3)
ST/Rx           PA15 (EXTINT[1]¸ S0/P1, TCC0/WO[1])
TH/Tx           PA04 (EXTINT[4], S0/P0, TCC0/WO[0])
CH3             PA08 (EXTINT[6], S0/P2, TCC0/WO[2])
OUT             PA14 (S0/P0. TC1/WO[0])
LED15           PA02
