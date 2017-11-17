
    I/O        Type  EIC        ADC     AC      PTC        DAC   SERCOM  ALT     TC/TCC      TCC         COM
    PA02             EXTINT[2]  AIN[0]          Y[0]       Vout
    PA03             EXTINT[3]  AIN[1]          Y[1]
    PA04             EXTINT[4]  AIN[2]  AIN[0]  Y[2]             S0P2    S0P0    TC1/WO[0]   TCC0/WO[0]
    PA05             EXTINT[5]  AIN[3]  AIN[1]  Y[3]             S0P3    S0P1    TC1/WO[0]   TCC0/WO[0]
    PA06             EXTINT[6]  AIN[4]  AIN[2]  Y[4]             S0P0    S0P2    TC2/WO[0]   TCC0/WO[2]
    PA07             EXTINT[7]  AIN[5]  AIN[3]  Y[5]             S0P1    S0P3    TC2/WO[1]   TCC0/WO[3]
    PA08             EXTINT[6]                                   S1P2    S0P2    TCC0/WO[2]  TCC0/WO[4]
    PA09             EXTINT[7]                                   S1P3    S0P3    TCC0/WO[3]  TCC0/WO[5]
    PA14       I2C   NMI        AIN[6]  CMP[0]  X[0]/Y[6]        S0P0    S2P0    TC1/WO[0]   TCC0/WO[0]
    PA15       I2C   EXTINT[1]  AIN[7]  CMP[1]  X[1]/Y[7]        S0P1    S2P1    TC1/WO[1]   TCC0/WO[1]
    PA16             EXTINT[0]                  X[4]/Y[10]       S1P2    S2P2    TC1/WO[0]   TCC0/WO[6]
    PA22       I2C   EXTINT[6]                  X[6]/Y[12]       S1P0    S2P0    TC1/WO[0]   TCC0/WO[4]
    PA23       I2C   EXTINT[7]                  X[7]/Y[13]       S1P1    S2P1    TC1/WO[1]   TCC0/WO[5]
    PA24             EXTINT[4]                  X[8]/Y[14]       S1P2    S2P2    TCC0/WO[2]  TCC0/WO[4]  USB/DM
    PA25             EXTINT[5]                  X[9]/Y[15]       S1P3    S2P3    TCC0/WO[3]              USB/DP
    PA28/RST
    PA30             EXTINT[2]                                   S1P0    S1P2    TC2/WO[0]   TCC0/WO[2]  SWCLK
    PA31             EXTINT[3]                                   S1P1    S1P3    TC2/WO[1]   TCC0/WO[3]  SWDIO


USB             PA24, PA25
SWDIO           PA30, PA31

BLANK           PA07
GSCLK           PA05
SCK             PA23 (S1P1)                             P1    S0: PA05, PA07, PA15    S1: PA23           S2: PA15, PA23
DATA            PA22 (S1P0)                             P0    S0: PA04, PA06, PA14    S1: PA22           S2: PA14, PA22
XLAT            PA09 (S1P3)                             P3    S0: PA05, PA07, PA09    S1: PA09, PA25     S2: PA25

ST/Rx           PA15 (EXTINT[1]¸ S0P1, TCC0/WO[1])      P1    S0: PA05, PA07, PA15    S1: PA23           S2: PA15, PA23
TH/Tx           PA04 (EXTINT[4], S0P0, TCC0/WO[0])      P0    S0: PA04, PA06, PA14    S1: PA22           S2: PA14, PA22
CH3             PA08 (EXTINT[6], S0P2, TCC0/WO[2])
OUT             PA14 (S0P0. TC1/WO[0])                  P0    S0: PA04, PA06, PA14    S1: PA22           S2: PA14, PA22

OUT15/LED       PA02
Button          PA03