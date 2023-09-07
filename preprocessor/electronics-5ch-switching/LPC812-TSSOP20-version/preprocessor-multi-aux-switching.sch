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
F 0 "U2" H 3800 1750 69  0000 L BNN
F 1 "ME6209A33M3G" H 3350 2200 69  0000 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23W" H 3750 2100 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/20005122B.pdf" H 3750 2100 50  0001 C CNN
	1    3750 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T1
U 1 1 8692C711
P 12300 2300
F 0 "T1" H 12150 2450 59  0000 L BNN
F 1 "PMV30UN" H 12400 2300 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12300 2300 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12300 2300 50  0001 C CNN
	1    12300 2300
	1    0    0    -1  
$EndComp
Text Notes 7400 8500 0    56   ~ 0
Special pins:\nPIO0_0  (19)   ISP UART RX\nPIO0_4  ( 5)   ISP UART TX\nPIO0_5  ( 4)   RESET\nPIO0_10 ( 9)   Open drain\nPIO0_11 ( 8)   Open drain\nPIO0_2  ( 7)   SWDIO\nPIO0_3  ( 6)   SWCLK
Text Notes 1100 4250 0    85   ~ 0
Input connectors
Text Notes 3250 1450 0    85   ~ 0
Voltage regulator
Text Notes 8050 4450 0    85   ~ 0
Microcontroller
Text Notes 2000 2700 0    59   ~ 0
X7R or X5R
Text Notes 1300 2350 0    59   ~ 0
Input voltage range:\n4 ... 10V
Text Notes 4950 2300 0    59   ~ 0
LDO: \nMCP1702, MCP1703,\nME6209A33M3G
Text Notes 6900 5200 0    59   ~ 0
NXP LPC812\nARM Cortex-M0\n16K Flash, 4K RAM\nTSSOP-20
Text Notes 11900 4650 0    56   ~ 0
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
P 1750 8900
F 0 "#PWR0101" H 1750 8650 50  0001 C CNN
F 1 "GND" H 1755 8727 50  0000 C CNN
F 2 "" H 1750 8900 50  0001 C CNN
F 3 "" H 1750 8900 50  0001 C CNN
	1    1750 8900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CCAC0F7
P 8450 7100
F 0 "#PWR0103" H 8450 6850 50  0001 C CNN
F 1 "GND" H 8455 6927 50  0000 C CNN
F 2 "" H 8450 7100 50  0001 C CNN
F 3 "" H 8450 7100 50  0001 C CNN
	1    8450 7100
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
	8450 7000 8450 7100
Text Notes 11600 1150 0    85   ~ 0
LED driver and outputs
Text Label 7150 5900 0    50   ~ 0
ST-RX
Text Label 7150 6300 0    50   ~ 0
TH-TX
Wire Wire Line
	9800 6200 9150 6200
Text Label 9800 6200 2    50   ~ 0
OUT-ISP
Wire Wire Line
	9800 6300 9150 6300
$Comp
L MCU_NXP_LPC:LPC812M101JDH20 U1
U 1 1 5EE5F15A
P 8450 6300
F 0 "U1" H 7950 6950 50  0000 C CNN
F 1 "LPC812M101JDH20" H 8850 6950 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 9450 7000 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC81XM.pdf" H 8450 5800 50  0001 C CNN
	1    8450 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 4800 8450 5000
$Comp
L Device:Q_NMOS_GSD T2
U 1 1 5EEDFD3A
P 12300 2900
F 0 "T2" H 12150 3050 59  0000 L BNN
F 1 "PMV30UN" H 12400 2900 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12300 2900 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12300 2900 50  0001 C CNN
	1    12300 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T3
U 1 1 5EEE0AF7
P 12300 3500
F 0 "T3" H 12150 3650 59  0000 L BNN
F 1 "PMV30UN" H 12400 3500 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12300 3500 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12300 3500 50  0001 C CNN
	1    12300 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5EEF5842
P 12700 3850
F 0 "#PWR01" H 12700 3600 50  0001 C CNN
F 1 "GND" H 12705 3677 50  0000 C CNN
F 2 "" H 12700 3850 50  0001 C CNN
F 3 "" H 12700 3850 50  0001 C CNN
	1    12700 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	12400 3700 12700 3700
Wire Wire Line
	12400 3100 12700 3100
Wire Wire Line
	12700 3100 12700 3700
Wire Wire Line
	12700 2500 12700 3100
Connection ~ 12700 3100
Text Label 11750 3500 0    50   ~ 0
OUT0
Wire Wire Line
	12100 2900 11750 2900
Text Label 11750 2900 0    50   ~ 0
OUT1
Wire Wire Line
	11750 3500 12100 3500
Text Label 11750 2300 0    50   ~ 0
OUT2
NoConn ~ 9150 6000
Text Label 9800 6700 2    50   ~ 0
OUT0
Text Label 7150 6200 0    50   ~ 0
OUT1
Text Label 7150 6100 0    50   ~ 0
OUT2
NoConn ~ 7750 6400
Text Label 2750 4750 2    50   ~ 0
ST-Rx-in
Text Label 2750 4850 2    50   ~ 0
VIN
Text Label 9800 6300 2    50   ~ 0
AUX
Wire Wire Line
	7150 6100 7750 6100
Wire Wire Line
	12400 2500 12700 2500
Wire Wire Line
	11750 2300 12100 2300
Wire Wire Line
	9800 6700 9150 6700
Wire Wire Line
	7150 6200 7750 6200
NoConn ~ 7750 6700
$Comp
L Device:C C3
U 1 1 5F2418F7
P 8750 5150
F 0 "C3" H 8865 5196 50  0000 L CNN
F 1 "100n" H 8865 5105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8788 5000 50  0001 C CNN
F 3 "~" H 8750 5150 50  0001 C CNN
	1    8750 5150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5F2421F0
P 8750 5350
F 0 "#PWR03" H 8750 5100 50  0001 C CNN
F 1 "GND" H 8755 5177 50  0000 C CNN
F 2 "" H 8750 5350 50  0001 C CNN
F 3 "" H 8750 5350 50  0001 C CNN
	1    8750 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8750 5300 8750 5350
Wire Wire Line
	8750 5000 8450 5000
Connection ~ 8450 5000
Wire Wire Line
	8450 5000 8450 5600
Wire Wire Line
	4050 2100 4650 2100
$Comp
L power:+3V3 #+3V01
U 1 1 5F1CA21E
P 8450 4800
F 0 "#+3V01" H 8450 4800 50  0001 C CNN
F 1 "+3V3" H 8300 4950 59  0000 L BNN
F 2 "" H 8450 4800 50  0001 C CNN
F 3 "" H 8450 4800 50  0001 C CNN
	1    8450 4800
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
Text Notes 10400 6650 0    50   ~ 0
PIO0_14 serves as detection whether \nthe hardware has the TLC5940 or is the switching version. \nFor TLC5940 it must be left floating (pull-up).\nFor switching version it must be pulled to GND.
$Comp
L power:GND #PWR0102
U 1 1 5F1DE1FF
P 10300 6600
F 0 "#PWR0102" H 10300 6350 50  0001 C CNN
F 1 "GND" H 10305 6427 50  0000 C CNN
F 2 "" H 10300 6600 50  0001 C CNN
F 3 "" H 10300 6600 50  0001 C CNN
	1    10300 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 6400 10300 6400
Wire Wire Line
	10300 6400 10300 6600
$Comp
L Connector_Generic:Conn_01x08 J3
U 1 1 5FF85660
P 13950 3000
F 0 "J3" H 13868 2475 50  0000 C CNN
F 1 "Pinheader straight 1x08" H 14250 2350 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x08_P2.54mm_Vertical_nosilk" H 13950 3000 50  0001 C CNN
F 3 "~" H 13950 3000 50  0001 C CNN
	1    13950 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	12700 3700 12700 3850
Connection ~ 12700 3700
Wire Wire Line
	12400 3300 13750 3300
Wire Wire Line
	12400 2700 13250 2700
Wire Wire Line
	13250 2700 13250 3100
Wire Wire Line
	13250 3100 13750 3100
Wire Wire Line
	12400 2100 13350 2100
Wire Wire Line
	13350 2900 13750 2900
Wire Wire Line
	13350 2100 13350 2900
Wire Wire Line
	13600 2000 13600 2800
Wire Wire Line
	13600 2800 13750 2800
Wire Wire Line
	13600 2800 13600 3000
Wire Wire Line
	13600 3000 13750 3000
Connection ~ 13600 2800
Wire Wire Line
	13600 3000 13600 3200
Wire Wire Line
	13600 3200 13750 3200
Connection ~ 13600 3000
NoConn ~ 7750 6000
NoConn ~ 9150 6500
NoConn ~ 7750 6600
NoConn ~ 9150 5900
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
P 6150 5900
F 0 "RN1" V 5733 5900 50  0000 C CNN
F 1 "1k" V 5824 5900 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 6425 5900 50  0001 C CNN
F 3 "~" H 6150 5900 50  0001 C CNN
	1    6150 5900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6900 5900 7750 5900
Wire Wire Line
	6350 5900 6750 5900
Wire Wire Line
	6750 6300 7750 6300
Text Label 4200 7600 2    50   ~ 0
OUT-ISP
Text Label 4200 7500 2    50   ~ 0
AUX
Text Label 5250 5800 0    50   ~ 0
ST-Rx-in
Wire Wire Line
	5250 5800 5950 5800
Wire Wire Line
	5250 5900 5950 5900
Text Label 5250 5900 0    50   ~ 0
TH-Tx-in
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5FF3C085
P 1450 4850
F 0 "J1" H 1368 4525 50  0000 C CNN
F 1 "Conn_01x03" H 1368 4616 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 4850 50  0001 C CNN
F 3 "~" H 1450 4850 50  0001 C CNN
	1    1450 4850
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J2
U 1 1 5FF3EA5B
P 1450 5400
F 0 "J2" H 1368 5075 50  0000 C CNN
F 1 "Conn_01x03" H 1368 5166 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 5400 50  0001 C CNN
F 3 "~" H 1450 5400 50  0001 C CNN
	1    1450 5400
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 5FF3EF14
P 1450 5950
F 0 "J4" H 1368 5625 50  0000 C CNN
F 1 "Conn_01x03" H 1368 5716 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 5950 50  0001 C CNN
F 3 "~" H 1450 5950 50  0001 C CNN
	1    1450 5950
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J5
U 1 1 5FF3F40C
P 1450 6500
F 0 "J5" H 1368 6175 50  0000 C CNN
F 1 "Conn_01x03" H 1368 6266 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 6500 50  0001 C CNN
F 3 "~" H 1450 6500 50  0001 C CNN
	1    1450 6500
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 5FF48E1C
P 1450 7050
F 0 "J6" H 1368 6725 50  0000 C CNN
F 1 "Conn_01x03" H 1368 6816 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 7050 50  0001 C CNN
F 3 "~" H 1450 7050 50  0001 C CNN
	1    1450 7050
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J7
U 1 1 5FF48FCE
P 1450 7600
F 0 "J7" H 1368 7275 50  0000 C CNN
F 1 "Conn_01x03" H 1368 7366 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 7600 50  0001 C CNN
F 3 "~" H 1450 7600 50  0001 C CNN
	1    1450 7600
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J8
U 1 1 5FF48FD8
P 1450 8150
F 0 "J8" H 1368 7825 50  0000 C CNN
F 1 "Conn_01x03" H 1368 7916 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 8150 50  0001 C CNN
F 3 "~" H 1450 8150 50  0001 C CNN
	1    1450 8150
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J9
U 1 1 5FF48FE2
P 1450 8700
F 0 "J9" H 1368 8375 50  0000 C CNN
F 1 "Conn_01x03" H 1368 8466 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x03_P2.54mm_Vertical_nosilk" H 1450 8700 50  0001 C CNN
F 3 "~" H 1450 8700 50  0001 C CNN
	1    1450 8700
	-1   0    0    1   
$EndComp
Wire Wire Line
	1650 4950 1750 4950
Wire Wire Line
	1750 4950 1750 5500
Connection ~ 1750 8800
Wire Wire Line
	1750 8800 1750 8900
Wire Wire Line
	1650 8250 1750 8250
Connection ~ 1750 8250
Wire Wire Line
	1750 8250 1750 8800
Wire Wire Line
	1650 7700 1750 7700
Connection ~ 1750 7700
Wire Wire Line
	1750 7700 1750 8250
Wire Wire Line
	1650 7150 1750 7150
Connection ~ 1750 7150
Wire Wire Line
	1750 7150 1750 7700
Wire Wire Line
	1650 6600 1750 6600
Connection ~ 1750 6600
Wire Wire Line
	1750 6600 1750 7150
Wire Wire Line
	1650 6050 1750 6050
Connection ~ 1750 6050
Wire Wire Line
	1750 6050 1750 6600
Wire Wire Line
	1650 5500 1750 5500
Connection ~ 1750 5500
Wire Wire Line
	1750 5500 1750 6050
Wire Wire Line
	1650 4850 1850 4850
Wire Wire Line
	1650 8800 1750 8800
Wire Wire Line
	1650 8700 1850 8700
Wire Wire Line
	1850 8700 1850 8150
Connection ~ 1850 4850
Wire Wire Line
	1850 4850 2750 4850
Wire Wire Line
	1650 5400 1850 5400
Connection ~ 1850 5400
Wire Wire Line
	1850 5400 1850 4850
Wire Wire Line
	1650 5950 1850 5950
Connection ~ 1850 5950
Wire Wire Line
	1850 5950 1850 5400
Wire Wire Line
	1650 6500 1850 6500
Connection ~ 1850 6500
Wire Wire Line
	1850 6500 1850 5950
Wire Wire Line
	1650 7050 1850 7050
Connection ~ 1850 7050
Wire Wire Line
	1850 7050 1850 6500
Wire Wire Line
	1650 7600 1850 7600
Connection ~ 1850 7600
Wire Wire Line
	1850 7600 1850 7050
Wire Wire Line
	1650 8150 1850 8150
Connection ~ 1850 8150
Wire Wire Line
	1850 8150 1850 7600
Wire Wire Line
	2750 4750 1950 4750
Wire Wire Line
	2750 5850 1950 5850
Wire Wire Line
	1650 5300 1950 5300
Wire Wire Line
	1950 5300 1950 4750
Connection ~ 1950 4750
Wire Wire Line
	1950 4750 1650 4750
Wire Wire Line
	1650 6400 1950 6400
Wire Wire Line
	1950 6400 1950 5850
Connection ~ 1950 5850
Wire Wire Line
	1950 5850 1650 5850
Text Label 2750 5850 2    50   ~ 0
TH-Tx-in
Text Label 2750 6950 2    50   ~ 0
AUX-in
Text Label 2750 7500 2    50   ~ 0
AUX2-in
Text Label 2750 8050 2    50   ~ 0
AUX3-in
Text Label 2750 8600 2    50   ~ 0
OUT-ISP-in
$Comp
L Device:R_Pack04 RN2
U 1 1 5FFCB31F
P 3500 7600
F 0 "RN2" V 3083 7600 50  0000 C CNN
F 1 "1k" V 3174 7600 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 3775 7600 50  0001 C CNN
F 3 "~" H 3500 7600 50  0001 C CNN
	1    3500 7600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1650 8050 3050 8050
Wire Wire Line
	3700 7500 4200 7500
Wire Wire Line
	4200 7600 3700 7600
Text Label 4200 7700 2    50   ~ 0
AUX2
Text Label 4200 7800 2    50   ~ 0
AUX3
Wire Wire Line
	7750 6500 7150 6500
Text Label 7150 6500 0    50   ~ 0
AUX2
Text Label 9800 6100 2    50   ~ 0
AUX3
$Comp
L Device:R R1
U 1 1 5FFE7AD9
P 9350 6950
F 0 "R1" H 9420 6996 50  0000 L CNN
F 1 "100k" H 9420 6905 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9280 6950 50  0001 C CNN
F 3 "~" H 9350 6950 50  0001 C CNN
	1    9350 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9350 6800 9350 6100
Wire Wire Line
	9150 6100 9350 6100
Connection ~ 9350 6100
Wire Wire Line
	9350 6100 9800 6100
$Comp
L power:GND #PWR02
U 1 1 5FFEE6F8
P 9350 7200
F 0 "#PWR02" H 9350 6950 50  0001 C CNN
F 1 "GND" H 9355 7027 50  0000 C CNN
F 2 "" H 9350 7200 50  0001 C CNN
F 3 "" H 9350 7200 50  0001 C CNN
	1    9350 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9350 7100 9350 7200
Wire Wire Line
	1650 8600 3150 8600
Wire Wire Line
	1650 6950 3150 6950
Wire Wire Line
	4200 7700 3700 7700
Wire Wire Line
	4200 7800 3700 7800
NoConn ~ 6350 6000
NoConn ~ 5950 6000
Wire Wire Line
	1650 7500 2800 7500
Wire Wire Line
	2800 7500 2800 7700
Wire Wire Line
	2800 7700 3300 7700
Wire Wire Line
	3050 7800 3300 7800
Wire Wire Line
	3150 7500 3300 7500
Wire Wire Line
	3150 6950 3150 7500
NoConn ~ 5950 6100
Wire Wire Line
	6900 5900 6900 5800
Wire Wire Line
	6900 5800 6350 5800
NoConn ~ 6350 6100
Wire Wire Line
	6750 5900 6750 6300
Text Label 13600 2000 0    50   ~ 0
VIN
Wire Wire Line
	3050 8050 3050 7800
Wire Wire Line
	3150 8600 3150 7600
Wire Wire Line
	3150 7600 3300 7600
Text Notes 6000 7300 0    50   ~ 0
We are using PIO0_6 instead of \nPIO0_8 for AUX2 so that in the \nfuture we could make a \n9-output Mk4S with 5-channel input.\n\nWhen switching hardware is detected \n(PIO0_14 is GND) then the software will\nuse PIO0_6 for AUX2, otherwise PIO0_8
Text Notes 3550 8300 0    50   ~ 0
Note: Resistor assignment according\nto what is easiest for the layout
$Comp
L Device:Q_NMOS_GSD T4
U 1 1 60061D05
P 12300 1650
F 0 "T4" H 12150 1800 59  0000 L BNN
F 1 "PMV30UN" H 12400 1650 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12300 1650 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12300 1650 50  0001 C CNN
	1    12300 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	11750 1650 12100 1650
Text Label 11750 1650 0    50   ~ 0
OUT3
Wire Wire Line
	12400 1850 12700 1850
Wire Wire Line
	12700 1850 12700 2500
Connection ~ 12700 2500
Wire Wire Line
	13600 3200 13600 3400
Wire Wire Line
	13600 3400 13750 3400
Connection ~ 13600 3200
Wire Wire Line
	13750 2700 13450 2700
Wire Wire Line
	13450 2700 13450 1450
Wire Wire Line
	13450 1450 12400 1450
Wire Wire Line
	9800 6600 9150 6600
Text Label 9800 6600 2    50   ~ 0
OUT3
$EndSCHEMATC
