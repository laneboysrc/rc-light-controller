
    I/O        Type  EIC        ADC     AC      PTC        DAC   SERCOM   ALT      TC/TCC      TCC         COM
    PA02             EXTINT[2]  AIN[0]          Y[0]       Vout
    PA03             EXTINT[3]  AIN[1]          Y[1]
    PA04             EXTINT[4]  AIN[2]  AIN[0]  Y[2]             S0/P[2]  S0/P[0]  TC1/WO[0]   TCC0/WO[0]
    PA05             EXTINT[5]  AIN[3]  AIN[1]  Y[3]             S0/P[3]  S0/P[1]  TC1/WO[0]   TCC0/WO[0]
    PA06             EXTINT[6]  AIN[4]  AIN[2]  Y[4]             S0/P[0]  S0/P[2]  TC2/WO[0]   TCC0/WO[2]
    PA07             EXTINT[7]  AIN[5]  AIN[3]  Y[5]             S0/P[1]  S0/P[3]  TC2/WO[1]   TCC0/WO[3]
    PA08             EXTINT[6]                                   S1/P[2]  S0/P[2]  TCC0/WO[2]  TCC0/WO[4]
    PA09             EXTINT[7]                                   S1/P[3]  S0/P[3]  TCC0/WO[3]  TCC0/WO[5]
    PA14       I2C   NMI        AIN[6]  CMP[0]  X[0]/Y[6]        S0/P[0]  S2/P[0]  TC1/WO[0]   TCC0/WO[0]
    PA15       I2C   EXTINT[1]  AIN[7]  CMP[1]  X[1]/Y[7]        S0/P[1]  S2/P[1]  TC1/WO[1]   TCC0/WO[1]
    PA16             EXTINT[0]                  X[4]/Y[10]       S1/P[2]  S2/P[2]  TC1/WO[0]   TCC0/WO[6]
    PA22       I2C   EXTINT[6]                  X[6]/Y[12]       S1/P[0]  S2/P[0]  TC1/WO[0]   TCC0/WO[4]
    PA23       I2C   EXTINT[7]                  X[7]/Y[13]       S1/P[1]  S2/P[1]  TC1/WO[1]   TCC0/WO[5]
    PA24             EXTINT[4]                  X[8]/Y[14]       S1/P[2]  S2/P[2]  TCC0/WO[2]  TCC0/WO[4]  USB/DM
    PA25             EXTINT[5]                  X[9]/Y[15]       S1/P[3]  S2/P[3]  TCC0/WO[3]              USB/DP
    PA28/RST
    PA30             EXTINT[2]                                   S1/P[0]  S1/P[2]  TC2/WO[0]   TCC0/WO[2]  SWCLK
    PA31             EXTINT[3]                                   S1/P[1]  S1/P[3]  TC2/WO[1]   TCC0/WO[3]  SWDIO


USB             PA24, PA25
SWDIO           PA30, PA31

BLANK           PA07
GSCLK           PA05
SCK             PA23 (S1/P1)
DATA            PA22 (S1/P0)
XLAT            PA09 (S1/P3)

ST/Rx           PA15 (EXTINT[1]¸ S0/P1, TCC0/WO[1])
TH/Tx           PA04 (EXTINT[4], S0/P0, TCC0/WO[0])
CH3             PA08 (EXTINT[6], S0/P2, TCC0/WO[2])
OUT             PA14 (S0/P0. TC1/WO[0])

OUT15/LED       PA02
Button          PA03