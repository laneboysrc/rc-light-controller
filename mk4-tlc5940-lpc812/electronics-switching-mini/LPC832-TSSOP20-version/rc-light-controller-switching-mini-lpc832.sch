EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "DIY RC Light Controller Mk4 S-mini LPC832"
Date "2021-07-27"
Rev "2"
Comp "LANE Boys RC"
Comment1 "laneboysrc@gmail.com"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	4650 2100 4650 1900
Wire Wire Line
	2650 2900 2650 2700
Wire Wire Line
	3750 2400 3750 2900
Wire Wire Line
	1700 5550 1700 5450
Text Label 1300 2100 0    70   ~ 0
VIN
$Comp
L power:+3V3 #+3V03
U 1 1 8A0CCF2D
P 4650 1900
F 0 "#+3V03" H 4650 1900 50  0001 C CNN
F 1 "+3V3" H 4500 2050 59  0000 L BNN
F 2 "" H 4650 1900 50  0001 C CNN
F 3 "" H 4650 1900 50  0001 C CNN
	1    4650 1900
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:MCP1703A-3302_SOT23 U2
U 1 1 09B5DC2D
P 3750 2100
F 0 "U2" H 3700 2250 69  0000 L BNN
F 1 "MCP1703A-3302_SOT23" H 3100 2350 69  0000 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23W" H 3750 2100 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/20005122B.pdf" H 3750 2100 50  0001 C CNN
	1    3750 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T1
U 1 1 8692C711
P 12000 3150
F 0 "T1" H 11850 3300 59  0000 L BNN
F 1 "PMV30UN" H 12100 3150 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12000 3150 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12000 3150 50  0001 C CNN
	1    12000 3150
	1    0    0    -1  
$EndComp
Text Notes 5750 9300 0    56   ~ 0
Special pins:\nPIO0_0  (19)   ISP UART RX\nPIO0_4  ( 6)   ISP UART TX\nPIO0_5  ( 5)   RESET\nPIO0_10 ( 10)   Open drain\nPIO0_11 ( 9)   Open drain\nPIO0_2  ( 8)   SWDIO\nPIO0_3  ( 7)   SWCLK
Text Notes 1350 4400 0    85   ~ 0
Input and power connector
Text Notes 3250 1450 0    85   ~ 0
Voltage regulator
Text Notes 7600 5850 0    85   ~ 0
Microcontroller
Text Notes 2000 2700 0    59   ~ 0
X7R or X5R
Text Notes 1300 2350 0    59   ~ 0
Input voltage range:\n4 ... 10V
Text Notes 4950 2300 0    59   ~ 0
LDO: \nMCP1702, MCP1703,\nME6209A33M3G
Text Notes 11600 5500 0    56   ~ 0
N-Channel MOSFET\nSOT23 package\ne.g. PMV16UN, PMV30UN, \nSI2302. AO3400, DMN2075U-7
Wire Wire Line
	4650 2100 4650 2500
Connection ~ 4650 2100
Wire Wire Line
	4650 2800 4650 2900
$Comp
L Device:CP C2
U 1 1 5C870864
P 4650 2650
F 0 "C2" H 4768 2696 50  0000 L CNN
F 1 "47u/6V3" H 4768 2605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4688 2500 50  0001 C CNN
F 3 "https://www.vishay.com/doc?40189" H 4650 2650 50  0001 C CNN
	1    4650 2650
	1    0    0    -1  
$EndComp
Text Notes 4750 2900 0    50   ~ 0
Polymer
$Comp
L power:GND #PWR0101
U 1 1 5CCABD51
P 1700 5550
F 0 "#PWR0101" H 1700 5300 50  0001 C CNN
F 1 "GND" H 1705 5377 50  0000 C CNN
F 2 "" H 1700 5550 50  0001 C CNN
F 3 "" H 1700 5550 50  0001 C CNN
	1    1700 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CCAC0F7
P 8000 8750
F 0 "#PWR0103" H 8000 8500 50  0001 C CNN
F 1 "GND" H 8005 8577 50  0000 C CNN
F 2 "" H 8000 8750 50  0001 C CNN
F 3 "" H 8000 8750 50  0001 C CNN
	1    8000 8750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5CCAC156
P 2650 2900
F 0 "#PWR0104" H 2650 2650 50  0001 C CNN
F 1 "GND" H 2655 2727 50  0000 C CNN
F 2 "" H 2650 2900 50  0001 C CNN
F 3 "" H 2650 2900 50  0001 C CNN
	1    2650 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5CCAC492
P 3750 2900
F 0 "#PWR0105" H 3750 2650 50  0001 C CNN
F 1 "GND" H 3755 2727 50  0000 C CNN
F 2 "" H 3750 2900 50  0001 C CNN
F 3 "" H 3750 2900 50  0001 C CNN
	1    3750 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5CCAC4F1
P 4650 2900
F 0 "#PWR0106" H 4650 2650 50  0001 C CNN
F 1 "GND" H 4655 2727 50  0000 C CNN
F 2 "" H 4650 2900 50  0001 C CNN
F 3 "" H 4650 2900 50  0001 C CNN
	1    4650 2900
	1    0    0    -1  
$EndComp
Text Notes 11300 2700 0    85   ~ 0
LED driver and outputs
Text Label 6500 7400 0    50   ~ 0
ST-RX
Text Label 6500 7800 0    50   ~ 0
TH-TX
Wire Wire Line
	9350 7600 8700 7600
Text Label 9350 7600 2    50   ~ 0
OUT-ISP
Wire Wire Line
	9350 7700 8700 7700
Wire Wire Line
	8000 6200 8000 6400
$Comp
L Device:Q_NMOS_GSD T2
U 1 1 5EEDFD3A
P 12000 3750
F 0 "T2" H 11850 3900 59  0000 L BNN
F 1 "PMV30UN" H 12100 3750 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12000 3750 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12000 3750 50  0001 C CNN
	1    12000 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T3
U 1 1 5EEE0AF7
P 12000 4350
F 0 "T3" H 11850 4500 59  0000 L BNN
F 1 "PMV30UN" H 12100 4350 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12000 4350 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12000 4350 50  0001 C CNN
	1    12000 4350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5EEF5842
P 12400 4700
F 0 "#PWR01" H 12400 4450 50  0001 C CNN
F 1 "GND" H 12405 4527 50  0000 C CNN
F 2 "" H 12400 4700 50  0001 C CNN
F 3 "" H 12400 4700 50  0001 C CNN
	1    12400 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	12100 4550 12400 4550
Wire Wire Line
	12100 3950 12400 3950
Wire Wire Line
	12400 3950 12400 4550
Wire Wire Line
	12400 3350 12400 3950
Connection ~ 12400 3950
Text Label 11450 4350 0    50   ~ 0
OUT0
Wire Wire Line
	11800 3750 11450 3750
Text Label 11450 3750 0    50   ~ 0
OUT1
Wire Wire Line
	11450 4350 11800 4350
Text Label 11450 3150 0    50   ~ 0
OUT2
Text Label 9350 8000 2    50   ~ 0
OUT0
Text Label 6500 7700 0    50   ~ 0
OUT1
Text Label 6500 7600 0    50   ~ 0
OUT2
Text Label 3050 5050 2    50   ~ 0
LED+
Text Notes 1000 6700 0    59   ~ 0
VIN1 is physically close to LED+.\nThis allows two modes of operation:\n1) when VIN1 is conntected to LED+\nvia a jumper, then the LEDs are \npowered from the receiver.\n2) A separate power supply can be \nconnected to LED+ (and the nearby GND),\ne.g. for higher voltages
Text Label 3050 4700 2    50   ~ 0
ST-Rx-in
Wire Wire Line
	2200 4700 2200 5250
Wire Wire Line
	2200 5250 1500 5250
Wire Wire Line
	2200 4700 3050 4700
Wire Wire Line
	1500 5350 1600 5350
Text Label 3050 5350 2    50   ~ 0
VIN
Text Label 13300 2850 0    50   ~ 0
LED+
Text Label 9350 7700 2    50   ~ 0
CH3
Wire Wire Line
	6500 7600 7100 7600
Wire Wire Line
	12100 3350 12400 3350
Wire Wire Line
	11450 3150 11800 3150
Wire Wire Line
	9350 8000 8700 8000
Wire Wire Line
	6500 7700 7100 7700
$Comp
L Device:C C3
U 1 1 5F2418F7
P 8300 6550
F 0 "C3" H 8415 6596 50  0000 L CNN
F 1 "100n" H 8415 6505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8338 6400 50  0001 C CNN
F 3 "~" H 8300 6550 50  0001 C CNN
	1    8300 6550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5F2421F0
P 8300 6750
F 0 "#PWR03" H 8300 6500 50  0001 C CNN
F 1 "GND" H 8305 6577 50  0000 C CNN
F 2 "" H 8300 6750 50  0001 C CNN
F 3 "" H 8300 6750 50  0001 C CNN
	1    8300 6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 6700 8300 6750
Wire Wire Line
	8300 6400 8000 6400
Connection ~ 8000 6400
Wire Wire Line
	8000 6400 8000 6900
Wire Wire Line
	4050 2100 4650 2100
$Comp
L power:+3V3 #+3V01
U 1 1 5F1CA21E
P 8000 6200
F 0 "#+3V01" H 8000 6200 50  0001 C CNN
F 1 "+3V3" H 7850 6350 59  0000 L BNN
F 2 "" H 8000 6200 50  0001 C CNN
F 3 "" H 8000 6200 50  0001 C CNN
	1    8000 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 2100 3450 2100
Wire Wire Line
	1300 2100 2650 2100
Connection ~ 2650 2100
Wire Wire Line
	2650 2400 2650 2100
$Comp
L Device:C C1
U 1 1 30D010B6
P 2650 2550
F 0 "C1" H 2750 2650 59  0000 L BNN
F 1 "1u/16V" H 2750 2400 59  0000 L BNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2650 2550 50  0001 C CNN
F 3 "" H 2650 2550 50  0001 C CNN
	1    2650 2550
	1    0    0    -1  
$EndComp
Text Notes 9750 7750 0    50   ~ 0
PIO0_14 serves as detection whether \nthe hardware has the TLC5940 or is the switching version. \nFor TLC5940 it must be left floating (pull-up).\nFor switching version it must be pulled to GND.
$Comp
L power:GND #PWR0102
U 1 1 5F1DE1FF
P 9850 8000
F 0 "#PWR0102" H 9850 7750 50  0001 C CNN
F 1 "GND" H 9855 7827 50  0000 C CNN
F 2 "" H 9850 8000 50  0001 C CNN
F 3 "" H 9850 8000 50  0001 C CNN
	1    9850 8000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 7800 9850 7800
Wire Wire Line
	9850 7800 9850 8000
Wire Wire Line
	1500 4950 1600 4950
Wire Wire Line
	1600 4950 1600 5350
Wire Wire Line
	1600 5350 3050 5350
Wire Wire Line
	1500 5150 1700 5150
Wire Wire Line
	1700 5150 1700 5450
Connection ~ 1700 5450
Wire Wire Line
	1500 5050 3050 5050
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 5FF85660
P 13650 3950
F 0 "J3" H 13568 3425 50  0000 C CNN
F 1 "Pinheader straight 1x06" H 13650 4400 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 13650 3950 50  0001 C CNN
F 3 "~" H 13650 3950 50  0001 C CNN
	1    13650 3950
	1    0    0    1   
$EndComp
Wire Wire Line
	12400 4550 12400 4700
Connection ~ 12400 4550
Wire Wire Line
	12100 4150 13450 4150
Wire Wire Line
	12100 3550 12950 3550
Wire Wire Line
	12950 3550 12950 3950
Wire Wire Line
	12950 3950 13450 3950
Wire Wire Line
	12100 2950 13050 2950
Wire Wire Line
	13050 3750 13450 3750
Wire Wire Line
	13050 2950 13050 3750
Wire Wire Line
	13300 2850 13300 3650
Wire Wire Line
	13300 3650 13450 3650
Wire Wire Line
	13300 3650 13300 3850
Wire Wire Line
	13300 3850 13450 3850
Connection ~ 13300 3650
Wire Wire Line
	13300 3850 13300 4050
Wire Wire Line
	13300 4050 13450 4050
Connection ~ 13300 3850
Wire Wire Line
	1700 5450 1500 5450
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5F1D0E80
P 1300 5250
F 0 "J2" H 1218 4725 50  0000 C CNN
F 1 "Pinheader straight 1x06" H 1218 4816 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 1300 5250 50  0001 C CNN
F 3 "~" H 1300 5250 50  0001 C CNN
	1    1300 5250
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5FFCF829
P 1500 8600
F 0 "J1" H 1418 8275 50  0000 C CNN
F 1 "Conn_01x03" H 1418 8366 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1500 8600 50  0001 C CNN
F 3 "~" H 1500 8600 50  0001 C CNN
	1    1500 8600
	-1   0    0    1   
$EndComp
Wire Wire Line
	1700 8700 2550 8700
Wire Wire Line
	1700 8600 2550 8600
Wire Wire Line
	1700 8500 2550 8500
Text Label 2550 8600 2    50   ~ 0
CH3-in
Text Label 2550 8700 2    50   ~ 0
OUT-ISP-in
Text Label 2550 8500 2    50   ~ 0
TH-TX-in
Text Notes 1250 8050 0    85   ~ 0
Programming connector
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5FFE4BED
P 1850 10450
F 0 "#FLG0102" H 1850 10525 50  0001 C CNN
F 1 "PWR_FLAG" H 1850 10624 50  0000 C CNN
F 2 "" H 1850 10450 50  0001 C CNN
F 3 "~" H 1850 10450 50  0001 C CNN
	1    1850 10450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 10450 1850 10650
Wire Wire Line
	1850 10650 2050 10650
Text Label 2050 10650 2    50   ~ 0
VIN
$Comp
L Device:R_Pack04 RN1
U 1 1 5FF24B18
P 5500 7500
F 0 "RN1" V 5083 7500 50  0000 C CNN
F 1 "1k" V 5174 7500 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 5775 7500 50  0001 C CNN
F 3 "~" H 5500 7500 50  0001 C CNN
	1    5500 7500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5700 7400 7100 7400
Wire Wire Line
	5700 7500 6350 7500
Wire Wire Line
	6350 7500 6350 7800
Wire Wire Line
	6350 7800 7100 7800
Wire Wire Line
	5700 7600 6200 7600
Wire Wire Line
	5700 7700 6200 7700
Text Label 6200 7700 2    50   ~ 0
OUT-ISP
Text Label 6200 7600 2    50   ~ 0
CH3
Text Label 4600 7400 0    50   ~ 0
ST-Rx-in
Wire Wire Line
	4600 7400 5300 7400
Wire Wire Line
	4600 7500 5300 7500
Wire Wire Line
	4600 7600 5300 7600
Wire Wire Line
	4600 7700 5300 7700
Text Label 4600 7500 0    50   ~ 0
TH-TX-in
Text Label 4600 7600 0    50   ~ 0
CH3-in
Text Label 4600 7700 0    50   ~ 0
OUT-ISP-in
Connection ~ 1600 5350
$Comp
L MCU_NXP_LPC:LPC832M101FDH20 U1
U 1 1 6100576F
P 7900 7800
F 0 "U1" H 8400 8450 50  0000 C CNN
F 1 "LPC832M101FDH20" H 7050 8450 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 8950 8550 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC83X.pdf" H 8050 8450 50  0001 L CNN
	1    7900 7800
	1    0    0    -1  
$EndComp
NoConn ~ 7100 7500
Text Notes 8550 9000 0    59   ~ 0
NXP LPC832\nARM Cortex-M0\n16K Flash, 4K RAM\nTSSOP-20
Wire Wire Line
	8000 8600 8000 8650
Wire Wire Line
	7800 8600 7800 8650
Wire Wire Line
	7800 8650 8000 8650
Connection ~ 8000 8650
Wire Wire Line
	8000 8650 8000 8750
Wire Wire Line
	7800 7000 7800 6900
Wire Wire Line
	7800 6900 8000 6900
Connection ~ 8000 6900
Wire Wire Line
	8000 6900 8000 7000
NoConn ~ 8700 7400
NoConn ~ 8700 7500
NoConn ~ 8700 7900
NoConn ~ 8700 8100
NoConn ~ 7100 7900
NoConn ~ 7100 8000
NoConn ~ 7100 8100
Wire Wire Line
	1100 10400 1100 10500
$Comp
L power:GND #PWR0111
U 1 1 5CD1123C
P 1100 10500
F 0 "#PWR0111" H 1100 10250 50  0001 C CNN
F 1 "GND" H 1105 10327 50  0000 C CNN
F 2 "" H 1100 10500 50  0001 C CNN
F 3 "" H 1100 10500 50  0001 C CNN
	1    1100 10500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C85CE29
P 1100 10400
F 0 "#FLG0101" H 1100 10475 50  0001 C CNN
F 1 "PWR_FLAG" H 1100 10574 50  0000 C CNN
F 2 "" H 1100 10400 50  0001 C CNN
F 3 "~" H 1100 10400 50  0001 C CNN
	1    1100 10400
	1    0    0    -1  
$EndComp
$EndSCHEMATC
