EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "DIY RC Light Controller Mk4 S-mini5"
Date "2021-01-04"
Rev "1"
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
Text Notes 6500 8550 0    56   ~ 0
Special pins:\nPIO0_0  (19)   ISP UART RX\nPIO0_4  ( 5)   ISP UART TX\nPIO0_5  ( 4)   RESET\nPIO0_10 ( 9)   Open drain\nPIO0_11 ( 8)   Open drain\nPIO0_2  ( 7)   SWDIO\nPIO0_3  ( 6)   SWCLK
Text Notes 1200 3800 0    85   ~ 0
Input connectors
Text Notes 3250 1450 0    85   ~ 0
Voltage regulator
Text Notes 7650 5150 0    85   ~ 0
Microcontroller
Text Notes 2000 2700 0    59   ~ 0
X7R or X5R
Text Notes 1300 2350 0    59   ~ 0
Input voltage range:\n4 ... 10V
Text Notes 4950 2300 0    59   ~ 0
LDO: \nMCP1702, MCP1703,\nME6209A33M3G
Text Notes 6500 5900 0    59   ~ 0
NXP LPC812\nARM Cortex-M0\n16K Flash, 4K RAM\nTSSOP-20
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
P 1850 8450
F 0 "#PWR0101" H 1850 8200 50  0001 C CNN
F 1 "GND" H 1855 8277 50  0000 C CNN
F 2 "" H 1850 8450 50  0001 C CNN
F 3 "" H 1850 8450 50  0001 C CNN
	1    1850 8450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CCAC0F7
P 8050 7800
F 0 "#PWR0103" H 8050 7550 50  0001 C CNN
F 1 "GND" H 8055 7627 50  0000 C CNN
F 2 "" H 8050 7800 50  0001 C CNN
F 3 "" H 8050 7800 50  0001 C CNN
	1    8050 7800
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
Wire Wire Line
	8050 7700 8050 7800
Text Notes 11300 2700 0    85   ~ 0
LED driver and outputs
Text Label 6750 6600 0    50   ~ 0
ST-RX
Text Label 6750 7000 0    50   ~ 0
TH-TX
Wire Wire Line
	9400 6900 8750 6900
Text Label 9400 6900 2    50   ~ 0
OUT-ISP
Wire Wire Line
	9400 7000 8750 7000
$Comp
L MCU_NXP_LPC:LPC812M101JDH20 U1
U 1 1 5EE5F15A
P 8050 7000
F 0 "U1" H 7550 7650 50  0000 C CNN
F 1 "LPC812M101JDH20" H 8450 7650 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 9050 7700 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC81XM.pdf" H 8050 6500 50  0001 C CNN
	1    8050 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 5500 8050 5700
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
NoConn ~ 8750 6700
Text Label 9400 7400 2    50   ~ 0
OUT0
Text Label 6750 6900 0    50   ~ 0
OUT1
Text Label 6750 6800 0    50   ~ 0
OUT2
NoConn ~ 7350 7100
Text Label 3800 4700 2    50   ~ 0
LED+
Text Notes 5450 4400 0    59   ~ 0
VIN1 is physically close to LED+.\nThis allows two modes of operation:\n1) when VIN1 is conntected to LED+\nvia a jumper, then the LEDs are \npowered from the receiver.\n2) A separate power supply can be \nconnected to LED+ (and the nearby GND),\ne.g. for higher voltages
Text Label 2850 4300 2    50   ~ 0
ST-Rx-in
Text Label 2850 4400 2    50   ~ 0
VIN
Text Label 13300 2850 0    50   ~ 0
LED+
Text Label 9400 7000 2    50   ~ 0
CH3
Wire Wire Line
	6750 6800 7350 6800
Wire Wire Line
	12100 3350 12400 3350
Wire Wire Line
	11450 3150 11800 3150
Wire Wire Line
	9400 7400 8750 7400
Wire Wire Line
	6750 6900 7350 6900
NoConn ~ 7350 7200
$Comp
L Device:C C3
U 1 1 5F2418F7
P 8350 5850
F 0 "C3" H 8465 5896 50  0000 L CNN
F 1 "100n" H 8465 5805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8388 5700 50  0001 C CNN
F 3 "~" H 8350 5850 50  0001 C CNN
	1    8350 5850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5F2421F0
P 8350 6050
F 0 "#PWR03" H 8350 5800 50  0001 C CNN
F 1 "GND" H 8355 5877 50  0000 C CNN
F 2 "" H 8350 6050 50  0001 C CNN
F 3 "" H 8350 6050 50  0001 C CNN
	1    8350 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 6000 8350 6050
Wire Wire Line
	8350 5700 8050 5700
Connection ~ 8050 5700
Wire Wire Line
	8050 5700 8050 6300
Wire Wire Line
	4050 2100 4650 2100
$Comp
L power:+3V3 #+3V01
U 1 1 5F1CA21E
P 8050 5500
F 0 "#+3V01" H 8050 5500 50  0001 C CNN
F 1 "+3V3" H 7900 5650 59  0000 L BNN
F 2 "" H 8050 5500 50  0001 C CNN
F 3 "" H 8050 5500 50  0001 C CNN
	1    8050 5500
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
Text Notes 10000 7350 0    50   ~ 0
PIO0_14 serves as detection whether \nthe hardware has the TLC5940 or is the switching version. \nFor TLC5940 it must be left floating (pull-up).\nFor switching version it must be pulled to GND.
$Comp
L power:GND #PWR0102
U 1 1 5F1DE1FF
P 9900 7300
F 0 "#PWR0102" H 9900 7050 50  0001 C CNN
F 1 "GND" H 9905 7127 50  0000 C CNN
F 2 "" H 9900 7300 50  0001 C CNN
F 3 "" H 9900 7300 50  0001 C CNN
	1    9900 7300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8750 7100 9900 7100
Wire Wire Line
	9900 7100 9900 7300
$Comp
L Connector_Generic:Conn_01x09 J3
U 1 1 5FF85660
P 13650 4050
F 0 "J3" H 13568 3525 50  0000 C CNN
F 1 "Pinheader straight 1x06" H 13650 4500 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x09_P2.54mm_Vertical" H 13650 4050 50  0001 C CNN
F 3 "~" H 13650 4050 50  0001 C CNN
	1    13650 4050
	1    0    0    -1  
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
NoConn ~ 7350 6700
NoConn ~ 8750 7200
NoConn ~ 8750 7300
NoConn ~ 7350 7300
NoConn ~ 8750 6600
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
Wire Wire Line
	1100 10400 1100 10500
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
$Comp
L Device:R_Pack04 RN1
U 1 1 5FF24B18
P 5750 6600
F 0 "RN1" V 5333 6600 50  0000 C CNN
F 1 "1k" V 5424 6600 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 6025 6600 50  0001 C CNN
F 3 "~" H 5750 6600 50  0001 C CNN
	1    5750 6600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6200 6600 7350 6600
Wire Wire Line
	5950 6800 6600 6800
Wire Wire Line
	6600 7000 7350 7000
Text Label 4300 7350 2    50   ~ 0
OUT-ISP
Text Label 4300 7050 2    50   ~ 0
CH3
Text Label 4850 6500 0    50   ~ 0
ST-Rx-in
Wire Wire Line
	4850 6500 5550 6500
Wire Wire Line
	4850 6800 5550 6800
Text Label 4850 6800 0    50   ~ 0
TH-Tx-in
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5FF3C085
P 1550 4400
F 0 "J1" H 1468 4075 50  0000 C CNN
F 1 "Conn_01x03" H 1468 4166 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 4400 50  0001 C CNN
F 3 "~" H 1550 4400 50  0001 C CNN
	1    1550 4400
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J2
U 1 1 5FF3EA5B
P 1550 4950
F 0 "J2" H 1468 4625 50  0000 C CNN
F 1 "Conn_01x03" H 1468 4716 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 4950 50  0001 C CNN
F 3 "~" H 1550 4950 50  0001 C CNN
	1    1550 4950
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 5FF3EF14
P 1550 5500
F 0 "J4" H 1468 5175 50  0000 C CNN
F 1 "Conn_01x03" H 1468 5266 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 5500 50  0001 C CNN
F 3 "~" H 1550 5500 50  0001 C CNN
	1    1550 5500
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J5
U 1 1 5FF3F40C
P 1550 6050
F 0 "J5" H 1468 5725 50  0000 C CNN
F 1 "Conn_01x03" H 1468 5816 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 6050 50  0001 C CNN
F 3 "~" H 1550 6050 50  0001 C CNN
	1    1550 6050
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 5FF48E1C
P 1550 6600
F 0 "J6" H 1468 6275 50  0000 C CNN
F 1 "Conn_01x03" H 1468 6366 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 6600 50  0001 C CNN
F 3 "~" H 1550 6600 50  0001 C CNN
	1    1550 6600
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J7
U 1 1 5FF48FCE
P 1550 7150
F 0 "J7" H 1468 6825 50  0000 C CNN
F 1 "Conn_01x03" H 1468 6916 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 7150 50  0001 C CNN
F 3 "~" H 1550 7150 50  0001 C CNN
	1    1550 7150
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J8
U 1 1 5FF48FD8
P 1550 7700
F 0 "J8" H 1468 7375 50  0000 C CNN
F 1 "Conn_01x03" H 1468 7466 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 7700 50  0001 C CNN
F 3 "~" H 1550 7700 50  0001 C CNN
	1    1550 7700
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J9
U 1 1 5FF48FE2
P 1550 8250
F 0 "J9" H 1468 7925 50  0000 C CNN
F 1 "Conn_01x03" H 1468 8016 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1550 8250 50  0001 C CNN
F 3 "~" H 1550 8250 50  0001 C CNN
	1    1550 8250
	-1   0    0    1   
$EndComp
Wire Wire Line
	1750 4500 1850 4500
Wire Wire Line
	1850 4500 1850 5050
Connection ~ 1850 8350
Wire Wire Line
	1850 8350 1850 8450
Wire Wire Line
	1750 7800 1850 7800
Connection ~ 1850 7800
Wire Wire Line
	1850 7800 1850 8350
Wire Wire Line
	1750 7250 1850 7250
Connection ~ 1850 7250
Wire Wire Line
	1850 7250 1850 7800
Wire Wire Line
	1750 6700 1850 6700
Connection ~ 1850 6700
Wire Wire Line
	1850 6700 1850 7250
Wire Wire Line
	1750 6150 1850 6150
Connection ~ 1850 6150
Wire Wire Line
	1850 6150 1850 6700
Wire Wire Line
	1750 5600 1850 5600
Connection ~ 1850 5600
Wire Wire Line
	1850 5600 1850 6150
Wire Wire Line
	1750 5050 1850 5050
Connection ~ 1850 5050
Wire Wire Line
	1850 5050 1850 5600
Wire Wire Line
	1750 4400 1950 4400
Wire Wire Line
	1750 8350 1850 8350
Wire Wire Line
	1750 8250 1950 8250
Wire Wire Line
	1950 8250 1950 7700
Connection ~ 1950 4400
Wire Wire Line
	1950 4400 2850 4400
Wire Wire Line
	1750 4950 1950 4950
Connection ~ 1950 4950
Wire Wire Line
	1950 4950 1950 4400
Wire Wire Line
	1750 5500 1950 5500
Connection ~ 1950 5500
Wire Wire Line
	1950 5500 1950 4950
Wire Wire Line
	1750 6050 1950 6050
Connection ~ 1950 6050
Wire Wire Line
	1950 6050 1950 5500
Wire Wire Line
	1750 6600 1950 6600
Connection ~ 1950 6600
Wire Wire Line
	1950 6600 1950 6050
Wire Wire Line
	1750 7150 1950 7150
Connection ~ 1950 7150
Wire Wire Line
	1950 7150 1950 6600
Wire Wire Line
	1750 7700 1950 7700
Connection ~ 1950 7700
Wire Wire Line
	1950 7700 1950 7150
Wire Wire Line
	2850 4300 2050 4300
Wire Wire Line
	2850 5400 2050 5400
Wire Wire Line
	1750 4850 2050 4850
Wire Wire Line
	2050 4850 2050 4300
Connection ~ 2050 4300
Wire Wire Line
	2050 4300 1750 4300
Wire Wire Line
	1750 5950 2050 5950
Wire Wire Line
	2050 5950 2050 5400
Connection ~ 2050 5400
Wire Wire Line
	2050 5400 1750 5400
Text Label 2850 5400 2    50   ~ 0
TH-Tx-in
Text Label 2850 6500 2    50   ~ 0
CH3-in
Text Label 2850 7050 2    50   ~ 0
AUX2-in
Text Label 2850 7600 2    50   ~ 0
AUX3-in
Text Label 2850 8150 2    50   ~ 0
OUT-ISP-in
$Comp
L Device:R_Pack04 RN2
U 1 1 5FFCB31F
P 3600 7150
F 0 "RN2" V 3183 7150 50  0000 C CNN
F 1 "1k" V 3274 7150 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 3875 7150 50  0001 C CNN
F 3 "~" H 3600 7150 50  0001 C CNN
	1    3600 7150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1750 7600 3150 7600
Wire Wire Line
	3800 7050 4300 7050
Wire Wire Line
	4300 7150 3800 7150
Text Label 4300 7150 2    50   ~ 0
AUX2
Text Label 4300 7250 2    50   ~ 0
AUX3
Wire Wire Line
	7350 7400 6750 7400
Text Label 6750 7400 0    50   ~ 0
AUX2
Text Label 9400 6800 2    50   ~ 0
AUX3
$Comp
L Device:R R1
U 1 1 5FFE7AD9
P 8950 7650
F 0 "R1" H 9020 7696 50  0000 L CNN
F 1 "100k" H 9020 7605 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8880 7650 50  0001 C CNN
F 3 "~" H 8950 7650 50  0001 C CNN
	1    8950 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 7500 8950 6800
Wire Wire Line
	8750 6800 8950 6800
Connection ~ 8950 6800
Wire Wire Line
	8950 6800 9400 6800
$Comp
L power:GND #PWR02
U 1 1 5FFEE6F8
P 8950 7900
F 0 "#PWR02" H 8950 7650 50  0001 C CNN
F 1 "GND" H 8955 7727 50  0000 C CNN
F 2 "" H 8950 7900 50  0001 C CNN
F 3 "" H 8950 7900 50  0001 C CNN
	1    8950 7900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 7800 8950 7900
Wire Wire Line
	3400 8150 3400 7350
Wire Wire Line
	1750 8150 3400 8150
Wire Wire Line
	1750 6500 3250 6500
Wire Wire Line
	4300 7250 3800 7250
Wire Wire Line
	4300 7350 3800 7350
NoConn ~ 5950 6700
NoConn ~ 5550 6700
Wire Wire Line
	1750 7050 2900 7050
Wire Wire Line
	2900 7050 2900 7150
Wire Wire Line
	2900 7150 3400 7150
Wire Wire Line
	3150 7600 3150 7250
Wire Wire Line
	3150 7250 3400 7250
Wire Wire Line
	3250 7050 3400 7050
Wire Wire Line
	3250 6500 3250 7050
NoConn ~ 5550 6600
Wire Wire Line
	6200 6600 6200 6500
Wire Wire Line
	6200 6500 5950 6500
NoConn ~ 5950 6600
Wire Wire Line
	6600 6800 6600 7000
$Comp
L power:GND #PWR?
U 1 1 5FF80AFD
P 13150 4700
F 0 "#PWR?" H 13150 4450 50  0001 C CNN
F 1 "GND" H 13155 4527 50  0000 C CNN
F 2 "" H 13150 4700 50  0001 C CNN
F 3 "" H 13150 4700 50  0001 C CNN
	1    13150 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	13450 4250 13150 4250
Wire Wire Line
	13150 4250 13150 4700
Wire Wire Line
	13300 4050 13300 4350
Wire Wire Line
	13300 4350 13450 4350
Connection ~ 13300 4050
Wire Wire Line
	13450 4450 12750 4450
Text Label 12750 4450 0    50   ~ 0
VIN
$EndSCHEMATC
