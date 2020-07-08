EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "DIY RC Light Controller Mk4 S"
Date "2020-06-14"
Rev "1"
Comp "LANE Boys RC"
Comment1 "laneboysrc@gmail.com"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	6000 2400 6000 2200
Wire Wire Line
	6000 2400 5800 2400
Wire Wire Line
	4000 3200 4000 3000
Wire Wire Line
	4400 2600 4400 3200
Wire Wire Line
	2050 2900 2050 2800
Wire Wire Line
	2050 2800 1450 2800
Wire Wire Line
	3150 3600 2650 3600
Text Label 3150 3600 2    70   ~ 0
TH-TX
Wire Wire Line
	4400 2400 4000 2400
Wire Wire Line
	4000 2700 4000 2400
Connection ~ 4000 2400
Wire Wire Line
	8550 6050 8050 6050
Wire Wire Line
	8550 5750 8050 5750
Text Label 3150 2400 2    70   ~ 0
VIN
Wire Wire Line
	3150 3200 2650 3200
Text Label 3150 3200 2    70   ~ 0
ST-RX
Wire Wire Line
	2250 3200 1450 3200
Wire Wire Line
	2250 3600 1450 3600
Wire Wire Line
	3150 4400 2650 4400
Text Label 3150 4400 2    70   ~ 0
OUT-ISP
Wire Wire Line
	1450 4000 2250 4000
Wire Wire Line
	2650 4000 3150 4000
Text Label 3150 4000 2    70   ~ 0
CH3
Wire Wire Line
	1450 4400 1950 4400
$Comp
L rc-light-controller-switching-lpc812-rescue:+3V3-rc-light-controller-tlc5940-lpc812-eagle-import #+3V01
U 1 1 910CF6D9
P 9750 1700
F 0 "#+3V01" H 9750 1700 50  0001 C CNN
F 1 "+3V3" V 9650 1500 59  0000 L BNN
F 2 "" H 9750 1700 50  0001 C CNN
F 3 "" H 9750 1700 50  0001 C CNN
	1    9750 1700
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:+3V3-rc-light-controller-tlc5940-lpc812-eagle-import #+3V03
U 1 1 8A0CCF2D
P 6000 2100
F 0 "#+3V03" H 6000 2100 50  0001 C CNN
F 1 "+3V3" V 5900 1900 59  0000 L BNN
F 2 "" H 6000 2100 50  0001 C CNN
F 3 "" H 6000 2100 50  0001 C CNN
	1    6000 2100
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:MCP1703T-3302E_CB-rc-light-controller-tlc5940-lpc812-eagle-import U$1
U 1 1 09B5DC2D
P 5100 2400
F 0 "U$1" H 4946 2699 69  0000 L BNN
F 1 "3V3" H 4994 1812 69  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT95P300X145-3N" H 5100 2400 50  0001 C CNN
F 3 "" H 5100 2400 50  0001 C CNN
	1    5100 2400
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD1
U 1 1 1C475C6D
P 1350 2400
F 0 "PAD1" H 1305 2473 59  0000 L BNN
F 1 "+" H 1305 2270 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 2400 50  0001 C CNN
F 3 "" H 1350 2400 50  0001 C CNN
	1    1350 2400
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD2
U 1 1 A0CD53A2
P 1350 2800
F 0 "PAD2" H 1305 2873 59  0000 L BNN
F 1 "-" H 1305 2670 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 2800 50  0001 C CNN
F 3 "" H 1350 2800 50  0001 C CNN
	1    1350 2800
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD3
U 1 1 1CFA7EBC
P 1350 3200
F 0 "PAD3" H 1305 3273 59  0000 L BNN
F 1 "ST/Rx" H 1305 3070 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 3200 50  0001 C CNN
F 3 "" H 1350 3200 50  0001 C CNN
	1    1350 3200
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD4
U 1 1 9066C7CA
P 1350 3600
F 0 "PAD4" H 1305 3673 59  0000 L BNN
F 1 "TH/Tx" H 1305 3470 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 3600 50  0001 C CNN
F 3 "" H 1350 3600 50  0001 C CNN
	1    1350 3600
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD6
U 1 1 6D080C98
P 1350 4400
F 0 "PAD6" H 1305 4473 59  0000 L BNN
F 1 "OUT/ISP" H 1305 4270 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 4400 50  0001 C CNN
F 3 "" H 1350 4400 50  0001 C CNN
	1    1350 4400
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT0
U 1 1 3C53439C
P 14200 1700
F 0 "OUT0" H 14155 1773 59  0000 L BNN
F 1 "SMD50X100" H 14155 1570 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14200 1700 50  0001 C CNN
F 3 "" H 14200 1700 50  0001 C CNN
	1    14200 1700
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT1
U 1 1 6BADA7B7
P 14200 2300
F 0 "OUT1" H 14155 2373 59  0000 L BNN
F 1 "SMD50X100" H 14155 2170 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14200 2300 50  0001 C CNN
F 3 "" H 14200 2300 50  0001 C CNN
	1    14200 2300
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT2
U 1 1 861A1973
P 14150 2900
F 0 "OUT2" H 14105 2973 59  0000 L BNN
F 1 "SMD50X100" H 14105 2770 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14150 2900 50  0001 C CNN
F 3 "" H 14150 2900 50  0001 C CNN
	1    14150 2900
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT3
U 1 1 FBBF8CFD
P 14150 3500
F 0 "OUT3" H 14105 3573 59  0000 L BNN
F 1 "SMD50X100" H 14105 3370 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14150 3500 50  0001 C CNN
F 3 "" H 14150 3500 50  0001 C CNN
	1    14150 3500
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT4
U 1 1 F2672D97
P 14200 4100
F 0 "OUT4" H 14155 4173 59  0000 L BNN
F 1 "SMD50X100" H 14155 3970 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14200 4100 50  0001 C CNN
F 3 "" H 14200 4100 50  0001 C CNN
	1    14200 4100
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT5
U 1 1 A0BFA14C
P 14200 4700
F 0 "OUT5" H 14155 4773 59  0000 L BNN
F 1 "SMD50X100" H 14155 4570 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14200 4700 50  0001 C CNN
F 3 "" H 14200 4700 50  0001 C CNN
	1    14200 4700
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT6
U 1 1 F2728259
P 14200 5300
F 0 "OUT6" H 14155 5373 59  0000 L BNN
F 1 "SMD50X100" H 14155 5170 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14200 5300 50  0001 C CNN
F 3 "" H 14200 5300 50  0001 C CNN
	1    14200 5300
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT7
U 1 1 F2054039
P 14200 5900
F 0 "OUT7" H 14155 5973 59  0000 L BNN
F 1 "SMD50X100" H 14155 5770 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14200 5900 50  0001 C CNN
F 3 "" H 14200 5900 50  0001 C CNN
	1    14200 5900
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import OUT8
U 1 1 D45D03D4
P 14200 6500
F 0 "OUT8" H 14155 6573 59  0000 L BNN
F 1 "SMD50X100" H 14155 6370 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 14200 6500 50  0001 C CNN
F 3 "" H 14200 6500 50  0001 C CNN
	1    14200 6500
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+2
U 1 1 2268DE62
P 8650 6050
F 0 "LED+2" H 8605 6123 59  0000 L BNN
F 1 "SMD50X100" H 8605 5920 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 6050 50  0001 C CNN
F 3 "" H 8650 6050 50  0001 C CNN
	1    8650 6050
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+1
U 1 1 CFCDD87C
P 8650 5750
F 0 "LED+1" H 8605 5823 59  0000 L BNN
F 1 "SMD50X100" H 8605 5620 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 5750 50  0001 C CNN
F 3 "" H 8650 5750 50  0001 C CNN
	1    8650 5750
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:C-EUC0805-rc-light-controller-tlc5940-lpc812-eagle-import C1
U 1 1 30D010B6
P 4000 2800
F 0 "C1" H 4060 2815 59  0000 L BNN
F 1 "1u/16V" H 4060 2615 59  0000 L BNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4000 2800 50  0001 C CNN
F 3 "" H 4000 2800 50  0001 C CNN
	1    4000 2800
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:R-EU_R0603-rc-light-controller-tlc5940-lpc812-eagle-import R3
U 1 1 3A5EE2B3
P 2450 3200
F 0 "R3" H 2300 3259 59  0000 L BNN
F 1 "1k" H 2300 3070 59  0000 L BNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2450 3200 50  0001 C CNN
F 3 "" H 2450 3200 50  0001 C CNN
	1    2450 3200
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:R-EU_R0603-rc-light-controller-tlc5940-lpc812-eagle-import R4
U 1 1 EB3F1CA8
P 2450 3600
F 0 "R4" H 2300 3659 59  0000 L BNN
F 1 "1k" H 2300 3470 59  0000 L BNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2450 3600 50  0001 C CNN
F 3 "" H 2450 3600 50  0001 C CNN
	1    2450 3600
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD5
U 1 1 411DFD1A
P 1350 4000
F 0 "PAD5" H 1305 4073 59  0000 L BNN
F 1 "CH3" H 1305 3870 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 4000 50  0001 C CNN
F 3 "" H 1350 4000 50  0001 C CNN
	1    1350 4000
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:R-EU_R0603-rc-light-controller-tlc5940-lpc812-eagle-import R6
U 1 1 5049E5A2
P 2450 4000
F 0 "R6" H 2300 4059 59  0000 L BNN
F 1 "1k" H 2300 3870 59  0000 L BNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2450 4000 50  0001 C CNN
F 3 "" H 2450 4000 50  0001 C CNN
	1    2450 4000
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:R-EU_R0603-rc-light-controller-tlc5940-lpc812-eagle-import R2
U 1 1 29741822
P 2450 4400
F 0 "R2" H 2300 4459 59  0000 L BNN
F 1 "1k" H 2300 4270 59  0000 L BNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2450 4400 50  0001 C CNN
F 3 "" H 2450 4400 50  0001 C CNN
	1    2450 4400
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T1
U 1 1 8692C711
P 13000 1900
F 0 "T1" H 13100 2000 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 1900 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 1900 50  0001 C CNN
F 3 "" H 13000 1900 50  0001 C CNN
	1    13000 1900
	1    0    0    -1  
$EndComp
Text Notes 7750 4500 0    56   ~ 0
Special pins:\nPIO0_0  (19)   ISP UART RX\nPIO0_4  ( 5)   ISP UART TX\nPIO0_5  ( 4)   RESET\nPIO0_10 ( 9)   Open drain\nPIO0_11 ( 8)   Open drain\nPIO0_2  ( 7)   SWDIO\nPIO0_3  ( 6)   SWCLK
Text Notes 1300 2100 0    85   ~ 0
Servo in/out
Text Notes 4600 1750 0    85   ~ 0
Voltage regulator
Text Notes 9300 1500 0    85   ~ 0
Microcontroller
Text Notes 3650 3000 0    59   ~ 0
X7R
Text Notes 2300 4650 0    59   ~ 0
All resistors 0603
Text Notes 2650 2650 0    59   ~ 0
Input voltage range:\n4 ... 10V
Text Notes 6300 2600 0    59   ~ 0
LDO: Microchip \nMCP1702 or MCP1703
Text Notes 10450 3900 0    59   ~ 0
NXP LPC812\nARM Cortex-M0\n16K Flash, 4K RAM\nTSSOP-20
Text Notes 12250 7350 0    56   ~ 0
N-Channel MOSFET\nSOT23 package\ne.g. NXP PMV16UN
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C85CE29
P 1150 10450
F 0 "#FLG0101" H 1150 10525 50  0001 C CNN
F 1 "PWR_FLAG" H 1150 10624 50  0000 C CNN
F 2 "" H 1150 10450 50  0001 C CNN
F 3 "~" H 1150 10450 50  0001 C CNN
	1    1150 10450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 10450 1150 10550
Wire Wire Line
	6000 2400 6000 2800
Connection ~ 6000 2400
Wire Wire Line
	6000 3100 6000 3200
$Comp
L Device:CP C2
U 1 1 5C870864
P 6000 2950
F 0 "C2" H 6118 2996 50  0000 L CNN
F 1 "47u/6V3" H 6118 2905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6038 2800 50  0001 C CNN
F 3 "~" H 6000 2950 50  0001 C CNN
	1    6000 2950
	1    0    0    -1  
$EndComp
Text Notes 6100 3200 0    50   ~ 0
Polymer
$Comp
L power:GND #PWR0101
U 1 1 5CCABD51
P 2050 2900
F 0 "#PWR0101" H 2050 2650 50  0001 C CNN
F 1 "GND" H 2055 2727 50  0000 C CNN
F 2 "" H 2050 2900 50  0001 C CNN
F 3 "" H 2050 2900 50  0001 C CNN
	1    2050 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CCAC0F7
P 9750 3550
F 0 "#PWR0103" H 9750 3300 50  0001 C CNN
F 1 "GND" H 9755 3377 50  0000 C CNN
F 2 "" H 9750 3550 50  0001 C CNN
F 3 "" H 9750 3550 50  0001 C CNN
	1    9750 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5CCAC156
P 4000 3200
F 0 "#PWR0104" H 4000 2950 50  0001 C CNN
F 1 "GND" H 4005 3027 50  0000 C CNN
F 2 "" H 4000 3200 50  0001 C CNN
F 3 "" H 4000 3200 50  0001 C CNN
	1    4000 3200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5CCAC492
P 4400 3200
F 0 "#PWR0105" H 4400 2950 50  0001 C CNN
F 1 "GND" H 4405 3027 50  0000 C CNN
F 2 "" H 4400 3200 50  0001 C CNN
F 3 "" H 4400 3200 50  0001 C CNN
	1    4400 3200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5CCAC4F1
P 6000 3200
F 0 "#PWR0106" H 6000 2950 50  0001 C CNN
F 1 "GND" H 6005 3027 50  0000 C CNN
F 2 "" H 6000 3200 50  0001 C CNN
F 3 "" H 6000 3200 50  0001 C CNN
	1    6000 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 6050 8050 5750
Connection ~ 8050 5750
Wire Wire Line
	8050 5750 7300 5750
Wire Wire Line
	9750 3450 9750 3550
$Comp
L power:GND #PWR0111
U 1 1 5CD1123C
P 1150 10550
F 0 "#PWR0111" H 1150 10300 50  0001 C CNN
F 1 "GND" H 1155 10377 50  0000 C CNN
F 2 "" H 1150 10550 50  0001 C CNN
F 3 "" H 1150 10550 50  0001 C CNN
	1    1150 10550
	1    0    0    -1  
$EndComp
Text Notes 12200 1450 0    85   ~ 0
LED driver and outputs
Wire Wire Line
	9050 2350 8450 2350
Text Label 8450 2350 0    50   ~ 0
ST-RX
Wire Wire Line
	9050 2750 8450 2750
Text Label 8450 2750 0    50   ~ 0
TH-TX
Wire Wire Line
	11100 2650 10450 2650
Text Label 11100 2650 2    50   ~ 0
OUT-ISP
Wire Wire Line
	11100 2750 10450 2750
Text Label 11100 2750 2    50   ~ 0
CH3
$Comp
L MCU_NXP_LPC:LPC812M101JDH20 U1
U 1 1 5EE5F15A
P 9750 2750
F 0 "U1" H 9250 3400 50  0000 C CNN
F 1 "LPC812M101JDH20" H 10150 3400 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 10750 3450 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC81XM.pdf" H 9750 2250 50  0001 C CNN
	1    9750 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 1800 9750 2050
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T2
U 1 1 5EEDFD3A
P 13000 2500
F 0 "T2" H 13100 2600 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 2500 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 2500 50  0001 C CNN
F 3 "" H 13000 2500 50  0001 C CNN
	1    13000 2500
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T3
U 1 1 5EEE0AF7
P 13000 3100
F 0 "T3" H 13100 3200 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 3100 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 3100 50  0001 C CNN
F 3 "" H 13000 3100 50  0001 C CNN
	1    13000 3100
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T4
U 1 1 5EEE2D0F
P 13000 3700
F 0 "T4" H 13100 3800 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 3700 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 3700 50  0001 C CNN
F 3 "" H 13000 3700 50  0001 C CNN
	1    13000 3700
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T5
U 1 1 5EEE34BB
P 13000 4300
F 0 "T5" H 13100 4400 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 4300 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 4300 50  0001 C CNN
F 3 "" H 13000 4300 50  0001 C CNN
	1    13000 4300
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T6
U 1 1 5EEE5899
P 13000 4900
F 0 "T6" H 13100 5000 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 4900 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 4900 50  0001 C CNN
F 3 "" H 13000 4900 50  0001 C CNN
	1    13000 4900
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T7
U 1 1 5EEE6005
P 13000 5500
F 0 "T7" H 13100 5600 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 5500 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 5500 50  0001 C CNN
F 3 "" H 13000 5500 50  0001 C CNN
	1    13000 5500
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T8
U 1 1 5EEE6489
P 13000 6100
F 0 "T8" H 13100 6200 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 6100 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 6100 50  0001 C CNN
F 3 "" H 13000 6100 50  0001 C CNN
	1    13000 6100
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:NMOSSOT23-rc-light-controller-tlc5940-lpc812-eagle-import T9
U 1 1 5EEE69E5
P 13000 6700
F 0 "T9" H 13100 6800 59  0000 L BNN
F 1 "NMOSSOT23" H 13100 6700 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SOT-23" H 13000 6700 50  0001 C CNN
F 3 "" H 13000 6700 50  0001 C CNN
	1    13000 6700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5EEF5842
P 13300 7650
F 0 "#PWR01" H 13300 7400 50  0001 C CNN
F 1 "GND" H 13305 7477 50  0000 C CNN
F 2 "" H 13300 7650 50  0001 C CNN
F 3 "" H 13300 7650 50  0001 C CNN
	1    13300 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	13000 6900 13300 6900
Wire Wire Line
	13000 6300 13300 6300
Wire Wire Line
	13300 6300 13300 6900
Connection ~ 13300 6900
Wire Wire Line
	13000 5700 13300 5700
Wire Wire Line
	13300 5700 13300 6300
Connection ~ 13300 6300
Wire Wire Line
	13000 5100 13300 5100
Wire Wire Line
	13300 5100 13300 5700
Connection ~ 13300 5700
Wire Wire Line
	13000 4500 13300 4500
Wire Wire Line
	13300 4500 13300 5100
Connection ~ 13300 5100
Wire Wire Line
	13000 3900 13300 3900
Wire Wire Line
	13300 3900 13300 4500
Connection ~ 13300 4500
Wire Wire Line
	13000 3300 13300 3300
Wire Wire Line
	13300 3300 13300 3900
Connection ~ 13300 3900
Wire Wire Line
	13000 2700 13300 2700
Wire Wire Line
	13300 2700 13300 3300
Connection ~ 13300 3300
Wire Wire Line
	13000 2100 13300 2100
Wire Wire Line
	13300 2100 13300 2700
Connection ~ 13300 2700
Wire Wire Line
	13000 1700 14100 1700
Wire Wire Line
	12800 2000 12200 2000
Text Label 12200 2000 0    50   ~ 0
OUT0
Wire Wire Line
	12800 2600 12200 2600
Text Label 12200 2600 0    50   ~ 0
OUT1
Wire Wire Line
	12200 3200 12800 3200
Text Label 12200 3200 0    50   ~ 0
OUT2
Wire Wire Line
	12200 3800 12800 3800
Text Label 12200 3800 0    50   ~ 0
OUT3
Wire Wire Line
	12200 4400 12800 4400
Text Label 12200 4400 0    50   ~ 0
OUT4
Wire Wire Line
	12200 5000 12800 5000
Text Label 12200 5000 0    50   ~ 0
OUT5
Wire Wire Line
	13300 6900 13300 7650
Wire Wire Line
	12200 5600 12800 5600
Text Label 12200 5600 0    50   ~ 0
OUT6
Wire Wire Line
	12200 6200 12800 6200
Text Label 12200 6200 0    50   ~ 0
OUT7
Wire Wire Line
	12200 6800 12800 6800
Text Label 12200 6800 0    50   ~ 0
OUT8
Wire Wire Line
	10450 2350 11100 2350
Wire Wire Line
	8450 2450 9050 2450
Wire Wire Line
	11100 2950 10450 2950
Text Label 8450 2450 0    50   ~ 0
OUT6
NoConn ~ 10450 2450
NoConn ~ 10450 2550
Wire Wire Line
	11100 3050 10450 3050
Text Label 8450 3050 0    50   ~ 0
OUT0
Wire Wire Line
	8450 2550 9050 2550
Text Label 8450 2550 0    50   ~ 0
OUT1
Wire Wire Line
	8450 2650 9050 2650
Text Label 11100 3050 2    50   ~ 0
OUT2
Wire Wire Line
	8450 3150 9050 3150
Text Label 8450 2950 0    50   ~ 0
OUT5
Wire Wire Line
	8450 3050 9050 3050
Text Label 8450 2650 0    50   ~ 0
OUT3
Wire Wire Line
	8450 2950 9050 2950
Text Label 11100 2950 2    50   ~ 0
OUT4
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+3
U 1 1 5EFA4B99
P 8650 6350
F 0 "LED+3" H 8605 6423 59  0000 L BNN
F 1 "SMD50X100" H 8605 6220 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 6350 50  0001 C CNN
F 3 "" H 8650 6350 50  0001 C CNN
	1    8650 6350
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+4
U 1 1 5EFA4E98
P 8650 6650
F 0 "LED+4" H 8605 6723 59  0000 L BNN
F 1 "SMD50X100" H 8605 6520 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 6650 50  0001 C CNN
F 3 "" H 8650 6650 50  0001 C CNN
	1    8650 6650
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+5
U 1 1 5EFA50EE
P 8650 6950
F 0 "LED+5" H 8605 7023 59  0000 L BNN
F 1 "SMD50X100" H 8605 6820 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 6950 50  0001 C CNN
F 3 "" H 8650 6950 50  0001 C CNN
	1    8650 6950
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+6
U 1 1 5EFA58A6
P 8650 7250
F 0 "LED+6" H 8605 7323 59  0000 L BNN
F 1 "SMD50X100" H 8605 7120 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 7250 50  0001 C CNN
F 3 "" H 8650 7250 50  0001 C CNN
	1    8650 7250
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+7
U 1 1 5EFA5A3F
P 8650 7550
F 0 "LED+7" H 8605 7623 59  0000 L BNN
F 1 "SMD50X100" H 8605 7420 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 7550 50  0001 C CNN
F 3 "" H 8650 7550 50  0001 C CNN
	1    8650 7550
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+8
U 1 1 5EFA5BE9
P 8650 7850
F 0 "LED+8" H 8605 7923 59  0000 L BNN
F 1 "SMD50X100" H 8605 7720 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 7850 50  0001 C CNN
F 3 "" H 8650 7850 50  0001 C CNN
	1    8650 7850
	-1   0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+9
U 1 1 5EFA5EEB
P 8650 8100
F 0 "LED+9" H 8605 8173 59  0000 L BNN
F 1 "SMD50X100" H 8605 7970 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 8650 8100 50  0001 C CNN
F 3 "" H 8650 8100 50  0001 C CNN
	1    8650 8100
	-1   0    0    1   
$EndComp
Wire Wire Line
	8550 8100 8050 8100
Wire Wire Line
	8050 8100 8050 7850
Connection ~ 8050 6050
Wire Wire Line
	8550 7850 8050 7850
Connection ~ 8050 7850
Wire Wire Line
	8050 7850 8050 7550
Wire Wire Line
	8550 7550 8050 7550
Connection ~ 8050 7550
Wire Wire Line
	8050 7550 8050 7250
Wire Wire Line
	8550 7250 8050 7250
Connection ~ 8050 7250
Wire Wire Line
	8050 7250 8050 6950
Wire Wire Line
	8550 6950 8050 6950
Connection ~ 8050 6950
Wire Wire Line
	8050 6950 8050 6650
Wire Wire Line
	8550 6650 8050 6650
Connection ~ 8050 6650
Wire Wire Line
	8050 6650 8050 6350
Wire Wire Line
	8550 6350 8050 6350
Connection ~ 8050 6350
Wire Wire Line
	8050 6350 8050 6050
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD7
U 1 1 5EFD5293
P 1350 5850
F 0 "PAD7" H 1305 5923 59  0000 L BNN
F 1 "+" H 1305 5720 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 5850 50  0001 C CNN
F 3 "" H 1350 5850 50  0001 C CNN
	1    1350 5850
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD8
U 1 1 5EFD555E
P 1350 6150
F 0 "PAD8" H 1305 6223 59  0000 L BNN
F 1 "-" H 1305 6020 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 6150 50  0001 C CNN
F 3 "" H 1350 6150 50  0001 C CNN
	1    1350 6150
	1    0    0    1   
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SDM80X120-rc-light-controller-tlc5940-lpc812-eagle-import PAD9
U 1 1 5EFD596E
P 1350 6500
F 0 "PAD9" H 1305 6573 59  0000 L BNN
F 1 "OUT/ISP" H 1305 6370 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1350 6500 50  0001 C CNN
F 3 "" H 1350 6500 50  0001 C CNN
	1    1350 6500
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5EFD836E
P 2400 6300
F 0 "#PWR02" H 2400 6050 50  0001 C CNN
F 1 "GND" H 2405 6127 50  0000 C CNN
F 2 "" H 2400 6300 50  0001 C CNN
F 3 "" H 2400 6300 50  0001 C CNN
	1    2400 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 6150 2400 6150
Wire Wire Line
	2400 6150 2400 6300
Wire Wire Line
	1450 5850 2250 5850
Wire Wire Line
	1950 4400 1950 6500
Wire Wire Line
	1950 6500 1450 6500
Connection ~ 1950 4400
Wire Wire Line
	1950 4400 2250 4400
Text Label 11100 2350 2    50   ~ 0
OUT7
Text Label 8450 3150 0    50   ~ 0
OUT8
NoConn ~ 10450 2850
NoConn ~ 10450 3150
NoConn ~ 9050 2850
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED+_in1
U 1 1 5F04C1D0
P 7200 5750
F 0 "LED+_in1" H 7155 5823 59  0000 L BNN
F 1 "SMD50X100" H 7155 5620 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 7200 5750 50  0001 C CNN
F 3 "" H 7200 5750 50  0001 C CNN
	1    7200 5750
	1    0    0    -1  
$EndComp
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import Vin1
U 1 1 5F054E07
P 6900 5750
F 0 "Vin1" H 6855 5823 59  0000 L BNN
F 1 "SMD50X100" H 6855 5620 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 6900 5750 50  0001 C CNN
F 3 "" H 6900 5750 50  0001 C CNN
	1    6900 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	6800 5750 6200 5750
Text Label 6200 5750 0    70   ~ 0
VIN
Text Label 7650 5750 0    50   ~ 0
LED+
$Comp
L power:GND #PWR03
U 1 1 5F06E5EF
P 7500 6500
F 0 "#PWR03" H 7500 6250 50  0001 C CNN
F 1 "GND" H 7505 6327 50  0000 C CNN
F 2 "" H 7500 6500 50  0001 C CNN
F 3 "" H 7500 6500 50  0001 C CNN
	1    7500 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 6100 7500 6500
Wire Wire Line
	7300 6100 7500 6100
$Comp
L rc-light-controller-switching-lpc812-rescue:SMD50X100-rc-light-controller-tlc5940-lpc812-eagle-import LED-_in1
U 1 1 5F06BB15
P 7200 6100
F 0 "LED-_in1" H 7155 6173 59  0000 L BNN
F 1 "SMD50X100" H 7155 5970 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 7200 6100 50  0001 C CNN
F 3 "" H 7200 6100 50  0001 C CNN
	1    7200 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	13000 6500 14100 6500
Wire Wire Line
	13000 5900 14100 5900
Wire Wire Line
	13000 5300 14100 5300
Wire Wire Line
	13000 4700 14100 4700
Wire Wire Line
	13000 4100 14100 4100
Wire Wire Line
	13000 3500 14050 3500
Wire Wire Line
	13000 2900 14050 2900
Wire Wire Line
	13000 2300 14100 2300
Text Notes 5750 7600 0    59   ~ 0
VIN1 is physically close to LED+_in1.\nThis allows two modes of operation:\n1) when VIN1 is conntected to LED+_in1\nvia a solder bridge, then the LEDs are \npowered from the receiver.\n2) A separate power supply can be \nconnected to LED+_in1 (and LED-_in1),\ne.g. for higher voltages
Text Notes 1300 5550 0    85   ~ 0
Slave out
Wire Wire Line
	1450 2400 4000 2400
Text Label 2250 5850 2    50   ~ 0
VIN
Text Label 1700 3200 0    50   ~ 0
ST-Rx-in
Text Label 1700 3600 0    50   ~ 0
TH-Tx-in
Text Label 1700 4000 0    50   ~ 0
CH3-in
Text Label 1700 4400 0    50   ~ 0
OUT-ISP-out
$EndSCHEMATC
