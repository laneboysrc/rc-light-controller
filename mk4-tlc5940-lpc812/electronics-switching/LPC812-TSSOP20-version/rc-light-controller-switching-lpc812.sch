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
	4650 2100 4650 1900
Wire Wire Line
	2650 2900 2650 2700
Wire Wire Line
	3750 2400 3750 2900
Wire Wire Line
	1850 6050 1850 5950
Wire Wire Line
	1850 5950 1700 5950
Wire Wire Line
	3600 5350 3100 5350
Text Label 3600 5350 2    50   ~ 0
TH-TX
Text Label 1300 2100 0    70   ~ 0
VIN
Wire Wire Line
	3600 5250 3100 5250
Text Label 3600 5250 2    50   ~ 0
ST-RX
Wire Wire Line
	3600 5550 3100 5550
Text Label 3600 5450 2    50   ~ 0
OUT-ISP
Wire Wire Line
	3100 5450 3600 5450
Text Label 3600 5550 2    50   ~ 0
CH3
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
P 12350 1850
F 0 "T1" H 12200 2000 59  0000 L BNN
F 1 "PMV30UN" H 12450 1850 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 1850 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 1850 50  0001 C CNN
	1    12350 1850
	1    0    0    -1  
$EndComp
Text Notes 5400 8050 0    56   ~ 0
Special pins:\nPIO0_0  (19)   ISP UART RX\nPIO0_4  ( 5)   ISP UART TX\nPIO0_5  ( 4)   RESET\nPIO0_10 ( 9)   Open drain\nPIO0_11 ( 8)   Open drain\nPIO0_2  ( 7)   SWDIO\nPIO0_3  ( 6)   SWCLK
Text Notes 1550 4900 0    85   ~ 0
Servo/Pre-processor in/out
Text Notes 3250 1450 0    85   ~ 0
Voltage regulator
Text Notes 6550 4650 0    85   ~ 0
Microcontroller
Text Notes 2000 2700 0    59   ~ 0
X7R or X5R
Text Notes 1300 2350 0    59   ~ 0
Input voltage range:\n4 ... 10V
Text Notes 4950 2300 0    59   ~ 0
LDO: \nMCP1702, MCP1703,\nME6209A33M3G
Text Notes 7650 7650 0    59   ~ 0
NXP LPC812\nARM Cortex-M0\n16K Flash, 4K RAM\nTSSOP-20
Text Notes 13200 7350 0    56   ~ 0
N-Channel MOSFET\nSOT23 package\ne.g. PMV16UN, PMV30UN, \nSI2302. AO3400, DMN2075U-7
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
P 1850 6050
F 0 "#PWR0101" H 1850 5800 50  0001 C CNN
F 1 "GND" H 1855 5877 50  0000 C CNN
F 2 "" H 1850 6050 50  0001 C CNN
F 3 "" H 1850 6050 50  0001 C CNN
	1    1850 6050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CCAC0F7
P 6950 7300
F 0 "#PWR0103" H 6950 7050 50  0001 C CNN
F 1 "GND" H 6955 7127 50  0000 C CNN
F 2 "" H 6950 7300 50  0001 C CNN
F 3 "" H 6950 7300 50  0001 C CNN
	1    6950 7300
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
	6950 7200 6950 7300
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
Text Notes 11650 1400 0    85   ~ 0
LED driver and outputs
Wire Wire Line
	6250 6100 5650 6100
Text Label 5650 6100 0    50   ~ 0
ST-RX
Wire Wire Line
	6250 6500 5650 6500
Text Label 5650 6500 0    50   ~ 0
TH-TX
Wire Wire Line
	8300 6400 7650 6400
Text Label 8300 6400 2    50   ~ 0
OUT-ISP
Wire Wire Line
	8300 6500 7650 6500
$Comp
L MCU_NXP_LPC:LPC812M101JDH20 U1
U 1 1 5EE5F15A
P 6950 6500
F 0 "U1" H 6450 7150 50  0000 C CNN
F 1 "LPC812M101JDH20" H 7350 7150 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 7950 7200 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC81XM.pdf" H 6950 6000 50  0001 C CNN
	1    6950 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 5000 6950 5200
$Comp
L Device:Q_NMOS_GSD T2
U 1 1 5EEDFD3A
P 12350 2450
F 0 "T2" H 12200 2600 59  0000 L BNN
F 1 "PMV30UN" H 12450 2450 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 2450 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 2450 50  0001 C CNN
	1    12350 2450
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T3
U 1 1 5EEE0AF7
P 12350 3050
F 0 "T3" H 12200 3200 59  0000 L BNN
F 1 "PMV30UN" H 12450 3050 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 3050 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 3050 50  0001 C CNN
	1    12350 3050
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T4
U 1 1 5EEE2D0F
P 12350 3650
F 0 "T4" H 12200 3800 59  0000 L BNN
F 1 "PMV30UN" H 12450 3650 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 3650 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 3650 50  0001 C CNN
	1    12350 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T5
U 1 1 5EEE34BB
P 12350 4250
F 0 "T5" H 12200 4400 59  0000 L BNN
F 1 "PMV30UN" H 12450 4250 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 4250 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 4250 50  0001 C CNN
	1    12350 4250
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T6
U 1 1 5EEE5899
P 12350 4850
F 0 "T6" H 12200 5000 59  0000 L BNN
F 1 "PMV30UN" H 12450 4850 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 4850 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 4850 50  0001 C CNN
	1    12350 4850
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T7
U 1 1 5EEE6005
P 12350 5450
F 0 "T7" H 12200 5600 59  0000 L BNN
F 1 "PMV30UN" H 12450 5450 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 5450 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 5450 50  0001 C CNN
	1    12350 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD T9
U 1 1 5EEE69E5
P 12350 6650
F 0 "T9" H 12200 6800 59  0000 L BNN
F 1 "PMV30UN" H 12450 6650 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 6650 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 6650 50  0001 C CNN
	1    12350 6650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5EEF5842
P 12750 7600
F 0 "#PWR01" H 12750 7350 50  0001 C CNN
F 1 "GND" H 12755 7427 50  0000 C CNN
F 2 "" H 12750 7600 50  0001 C CNN
F 3 "" H 12750 7600 50  0001 C CNN
	1    12750 7600
	1    0    0    -1  
$EndComp
Wire Wire Line
	12450 6850 12750 6850
Wire Wire Line
	12450 6250 12750 6250
Wire Wire Line
	12750 6250 12750 6850
Connection ~ 12750 6850
Wire Wire Line
	12450 5650 12750 5650
Wire Wire Line
	12750 5650 12750 6250
Connection ~ 12750 6250
Wire Wire Line
	12450 5050 12750 5050
Wire Wire Line
	12750 5050 12750 5650
Connection ~ 12750 5650
Wire Wire Line
	12450 4450 12750 4450
Wire Wire Line
	12750 4450 12750 5050
Connection ~ 12750 5050
Wire Wire Line
	12450 3850 12750 3850
Wire Wire Line
	12750 3850 12750 4450
Connection ~ 12750 4450
Wire Wire Line
	12450 3250 12750 3250
Wire Wire Line
	12750 3250 12750 3850
Connection ~ 12750 3850
Wire Wire Line
	12450 2650 12750 2650
Wire Wire Line
	12750 2650 12750 3250
Connection ~ 12750 3250
Wire Wire Line
	12750 2050 12750 2650
Connection ~ 12750 2650
Text Label 11800 1850 0    50   ~ 0
OUT0
Wire Wire Line
	12150 2450 11800 2450
Text Label 11800 2450 0    50   ~ 0
OUT1
Wire Wire Line
	11800 3050 12150 3050
Text Label 11800 3050 0    50   ~ 0
OUT2
Wire Wire Line
	11800 3650 12150 3650
Text Label 11800 3650 0    50   ~ 0
OUT3
Wire Wire Line
	11800 4250 12150 4250
Text Label 11800 4250 0    50   ~ 0
OUT4
Wire Wire Line
	11800 4850 12150 4850
Text Label 11800 4850 0    50   ~ 0
OUT5
Wire Wire Line
	12750 6850 12750 7600
Wire Wire Line
	11800 5450 12150 5450
Text Label 11800 5450 0    50   ~ 0
OUT6
Wire Wire Line
	11800 6050 12150 6050
Text Label 11800 6050 0    50   ~ 0
OUT7
Wire Wire Line
	11800 6650 12150 6650
Text Label 11800 6650 0    50   ~ 0
OUT8
Wire Wire Line
	7650 6100 8300 6100
Wire Wire Line
	5650 6200 6250 6200
Wire Wire Line
	8300 6700 7650 6700
Text Label 8300 6100 2    50   ~ 0
OUT6
NoConn ~ 7650 6200
NoConn ~ 7650 6300
Text Label 8300 6900 2    50   ~ 0
OUT0
Text Label 5650 6400 0    50   ~ 0
OUT1
Text Label 5650 6300 0    50   ~ 0
OUT2
Wire Wire Line
	5650 6900 6250 6900
Text Label 5650 6900 0    50   ~ 0
OUT5
Text Label 8300 6800 2    50   ~ 0
OUT3
Text Label 5650 6800 0    50   ~ 0
OUT4
$Comp
L power:GND #PWR02
U 1 1 5EFD836E
P 1800 7850
F 0 "#PWR02" H 1800 7600 50  0001 C CNN
F 1 "GND" H 1805 7677 50  0000 C CNN
F 2 "" H 1800 7850 50  0001 C CNN
F 3 "" H 1800 7850 50  0001 C CNN
	1    1800 7850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 7700 1800 7700
Wire Wire Line
	1800 7700 1800 7850
Text Label 5650 6200 0    50   ~ 0
OUT7
NoConn ~ 6250 6600
Text Label 2950 7200 2    50   ~ 0
VIN
Text Label 2950 7300 2    50   ~ 0
LED+
Wire Wire Line
	12450 3450 13500 3450
Wire Wire Line
	12450 2850 13500 2850
Text Notes 1450 8900 0    59   ~ 0
VIN1 is physically close to LED+.\nThis allows two modes of operation:\n1) when VIN1 is conntected to LED+\nvia a jumper, then the LEDs are \npowered from the receiver.\n2) A separate power supply can be \nconnected to LED+ (and the nearby GND),\ne.g. for higher voltages
Text Notes 1750 6800 0    85   ~ 0
Output connector
Text Label 2150 5250 0    50   ~ 0
ST-Rx-in
Text Label 2150 5350 0    50   ~ 0
TH-Tx-in
Text Label 2150 5550 0    50   ~ 0
CH3-in
Text Label 2150 5450 0    50   ~ 0
OUT-ISP-out
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5F1D0E80
P 1500 5750
F 0 "J2" H 1418 5225 50  0000 C CNN
F 1 "Pinheader straight 1x06" H 1418 5316 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 1500 5750 50  0001 C CNN
F 3 "~" H 1500 5750 50  0001 C CNN
	1    1500 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	1950 5350 1950 5650
Wire Wire Line
	1950 5650 1700 5650
Wire Wire Line
	1950 5350 2700 5350
Wire Wire Line
	1850 5250 1850 5750
Wire Wire Line
	1850 5750 1700 5750
Wire Wire Line
	1850 5250 2700 5250
Wire Wire Line
	1700 5850 3600 5850
Text Label 3600 5850 2    50   ~ 0
VIN
Wire Wire Line
	15050 2500 15050 1700
Text Label 15050 1700 0    50   ~ 0
LED+
Connection ~ 15050 2500
Connection ~ 15050 2600
Wire Wire Line
	15050 2600 15050 2500
Connection ~ 15050 2700
Wire Wire Line
	15050 2700 15050 2600
Connection ~ 15050 2800
Wire Wire Line
	15050 2800 15050 2700
Connection ~ 15050 2900
Wire Wire Line
	15050 2900 15050 2800
Connection ~ 15050 3000
Wire Wire Line
	15050 3000 15050 2900
Connection ~ 15050 3100
Wire Wire Line
	15050 3100 15050 3000
Wire Wire Line
	15050 3100 15050 3200
Connection ~ 15050 3200
Wire Wire Line
	15050 3200 15050 3300
Connection ~ 15050 3300
Wire Wire Line
	15050 3300 15050 3400
Connection ~ 15050 3400
Wire Wire Line
	15050 3400 15050 3500
Connection ~ 15050 3500
Wire Wire Line
	15050 3500 15050 3600
Connection ~ 15050 3600
Wire Wire Line
	15050 3600 15050 3700
Connection ~ 15050 3700
Wire Wire Line
	15050 3700 15050 3800
Connection ~ 15050 3800
Wire Wire Line
	15050 3800 15050 3900
Connection ~ 15050 3900
Wire Wire Line
	15050 3900 15050 4000
Connection ~ 15050 4000
Wire Wire Line
	15050 4000 15050 4100
Wire Wire Line
	14050 1650 14050 2500
Wire Wire Line
	14050 2500 14300 2500
Wire Wire Line
	14300 2600 14050 2600
Wire Wire Line
	14050 2600 14050 2500
Connection ~ 14050 2500
Wire Wire Line
	13850 2250 13850 2700
Wire Wire Line
	13850 2700 14300 2700
Wire Wire Line
	12450 2250 13850 2250
Wire Wire Line
	14300 2800 13850 2800
Wire Wire Line
	13850 2800 13850 2700
Connection ~ 13850 2700
Wire Wire Line
	13500 2850 13500 2900
Wire Wire Line
	13500 2900 14300 2900
Wire Wire Line
	14300 3000 13500 3000
Wire Wire Line
	13500 3000 13500 2900
Connection ~ 13500 2900
Wire Wire Line
	14300 3100 13500 3100
Wire Wire Line
	13500 3100 13500 3200
Wire Wire Line
	14300 3200 13500 3200
Connection ~ 13500 3200
Wire Wire Line
	13500 3200 13500 3450
Wire Wire Line
	14300 3300 13600 3300
Wire Wire Line
	13600 3300 13600 3400
Wire Wire Line
	12450 4050 13600 4050
Wire Wire Line
	14300 3400 13600 3400
Connection ~ 13600 3400
Wire Wire Line
	13600 3400 13600 4050
Wire Wire Line
	14300 3500 13650 3500
Wire Wire Line
	13650 3500 13650 3600
Wire Wire Line
	12450 4650 13650 4650
Wire Wire Line
	14300 3600 13650 3600
Connection ~ 13650 3600
Wire Wire Line
	13650 3600 13650 4650
Wire Wire Line
	14300 3700 13750 3700
Wire Wire Line
	13750 3700 13750 3800
Wire Wire Line
	12450 5250 13750 5250
Wire Wire Line
	14300 3800 13750 3800
Connection ~ 13750 3800
Wire Wire Line
	13750 3800 13750 5250
Wire Wire Line
	14300 3900 13900 3900
Wire Wire Line
	13900 3900 13900 4000
Wire Wire Line
	12450 5850 13900 5850
Wire Wire Line
	14300 4000 13900 4000
Connection ~ 13900 4000
Wire Wire Line
	13900 4000 13900 5850
Wire Wire Line
	14300 4100 14300 6450
Wire Wire Line
	12450 6450 14300 6450
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 5F244636
P 1500 7500
F 0 "J3" H 1418 6975 50  0000 C CNN
F 1 "Pinheader straight 1x06" H 1418 7066 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 1500 7500 50  0001 C CNN
F 3 "~" H 1500 7500 50  0001 C CNN
	1    1500 7500
	-1   0    0    1   
$EndComp
Wire Wire Line
	1700 7600 1900 7600
Wire Wire Line
	1700 7400 1800 7400
Wire Wire Line
	1800 7400 1800 7700
Connection ~ 1800 7700
Wire Wire Line
	1700 7200 1900 7200
Wire Wire Line
	15050 2800 14800 2800
Wire Wire Line
	14800 2700 15050 2700
Wire Wire Line
	14800 2600 15050 2600
Wire Wire Line
	14800 4100 15050 4100
Wire Wire Line
	14800 4000 15050 4000
Wire Wire Line
	14800 3900 15050 3900
Wire Wire Line
	14800 3800 15050 3800
Wire Wire Line
	14800 3700 15050 3700
Wire Wire Line
	14800 3600 15050 3600
Wire Wire Line
	14800 3500 15050 3500
Wire Wire Line
	14800 3400 15050 3400
Wire Wire Line
	14800 2500 15050 2500
Wire Wire Line
	14800 3300 15050 3300
Wire Wire Line
	14800 3200 15050 3200
Wire Wire Line
	14800 3100 15050 3100
Wire Wire Line
	14800 3000 15050 3000
Wire Wire Line
	14800 2900 15050 2900
$Comp
L Connector_Generic:Conn_02x17_Odd_Even J1
U 1 1 5F1ABEB8
P 14600 3300
F 0 "J1" H 14750 4350 50  0000 C CNN
F 1 "Pinheader right-angle 2x17" H 13700 4250 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x17_P2.54mm_Horizontal" H 14600 3300 50  0001 C CNN
F 3 "~" H 14600 3300 50  0001 C CNN
	1    14600 3300
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Pack04 RN1
U 1 1 5F192F7F
P 2900 5450
F 0 "RN1" V 2483 5450 50  0000 C CNN
F 1 "1k x4" V 2574 5450 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 3175 5450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/1810311812_UNI-ROYAL-Uniroyal-Elec-4D03WGJ0102T5E_C20197.pdf" H 2900 5450 50  0001 C CNN
	1    2900 5450
	0    1    1    0   
$EndComp
Text Label 8300 6700 2    50   ~ 0
OUT8
Text Label 8300 6500 2    50   ~ 0
CH3
Wire Wire Line
	5650 6300 6250 6300
Wire Wire Line
	5650 6800 6250 6800
$Comp
L Mechanical:MountingHole H1
U 1 1 5F2D18E2
P 1600 10400
F 0 "H1" H 1700 10446 50  0000 L CNN
F 1 "MountingHole" H 1700 10355 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_ISO14580" H 1600 10400 50  0001 C CNN
F 3 "~" H 1600 10400 50  0001 C CNN
	1    1600 10400
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5F2D2122
P 1600 10650
F 0 "H2" H 1700 10696 50  0000 L CNN
F 1 "MountingHole" H 1700 10605 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_ISO14580" H 1600 10650 50  0001 C CNN
F 3 "~" H 1600 10650 50  0001 C CNN
	1    1600 10650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 6800 8300 6800
Wire Wire Line
	12450 2050 12750 2050
Wire Wire Line
	12450 1650 14050 1650
Wire Wire Line
	11800 1850 12150 1850
Wire Wire Line
	8300 6900 7650 6900
Wire Wire Line
	5650 6400 6250 6400
NoConn ~ 6250 6700
$Comp
L Device:C C3
U 1 1 5F2418F7
P 7250 5350
F 0 "C3" H 7365 5396 50  0000 L CNN
F 1 "100n" H 7365 5305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7288 5200 50  0001 C CNN
F 3 "~" H 7250 5350 50  0001 C CNN
	1    7250 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5F2421F0
P 7250 5550
F 0 "#PWR03" H 7250 5300 50  0001 C CNN
F 1 "GND" H 7255 5377 50  0000 C CNN
F 2 "" H 7250 5550 50  0001 C CNN
F 3 "" H 7250 5550 50  0001 C CNN
	1    7250 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 5500 7250 5550
Wire Wire Line
	7250 5200 6950 5200
Connection ~ 6950 5200
Wire Wire Line
	6950 5200 6950 5800
Wire Wire Line
	4050 2100 4650 2100
$Comp
L power:+3V3 #+3V01
U 1 1 5F1CA21E
P 6950 5000
F 0 "#+3V01" H 6950 5000 50  0001 C CNN
F 1 "+3V3" H 6800 5150 59  0000 L BNN
F 2 "" H 6950 5000 50  0001 C CNN
F 3 "" H 6950 5000 50  0001 C CNN
	1    6950 5000
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
Wire Wire Line
	1700 7500 2950 7500
Text Label 2950 7500 2    50   ~ 0
OUT-ISP-out
Wire Wire Line
	1700 5550 2700 5550
Wire Wire Line
	1700 5450 2700 5450
$Comp
L Device:Q_NMOS_GSD T8
U 1 1 5EEE6489
P 12350 6050
F 0 "T8" H 12200 6200 59  0000 L BNN
F 1 "PMV30UN" H 12450 6050 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12350 6050 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12350 6050 50  0001 C CNN
	1    12350 6050
	1    0    0    -1  
$EndComp
Text Notes 8700 6550 0    50   ~ 0
PIO0_14 serves as detection whether \nthe hardware has the TLC5940 or is the switching version. \nFor TLC5940 it must be left floating (pull-up).\nFor switching version it must be pulled to GND.
$Comp
L power:GND #PWR0102
U 1 1 5F1DE1FF
P 8800 6800
F 0 "#PWR0102" H 8800 6550 50  0001 C CNN
F 1 "GND" H 8805 6627 50  0000 C CNN
F 2 "" H 8800 6800 50  0001 C CNN
F 3 "" H 8800 6800 50  0001 C CNN
	1    8800 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 6600 8800 6600
Wire Wire Line
	8800 6600 8800 6800
Wire Wire Line
	1700 7300 2950 7300
Wire Wire Line
	1900 7600 1900 7200
Connection ~ 1900 7200
Wire Wire Line
	1900 7200 2950 7200
$EndSCHEMATC
