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
	1450 5000 1450 4900
Wire Wire Line
	1450 4900 1050 4900
Wire Wire Line
	3150 3600 2650 3600
Text Label 3150 3600 2    70   ~ 0
TH-TX
Wire Wire Line
	4400 2400 4000 2400
Wire Wire Line
	4000 2700 4000 2400
Connection ~ 4000 2400
Text Label 2400 2400 0    70   ~ 0
VIN
Wire Wire Line
	3150 3200 2650 3200
Text Label 3150 3200 2    70   ~ 0
ST-RX
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
P 1450 5000
F 0 "#PWR0101" H 1450 4750 50  0001 C CNN
F 1 "GND" H 1455 4827 50  0000 C CNN
F 2 "" H 1450 5000 50  0001 C CNN
F 3 "" H 1450 5000 50  0001 C CNN
	1    1450 5000
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
L power:GND #PWR02
U 1 1 5EFD836E
P 2000 6600
F 0 "#PWR02" H 2000 6350 50  0001 C CNN
F 1 "GND" H 2005 6427 50  0000 C CNN
F 2 "" H 2000 6600 50  0001 C CNN
F 3 "" H 2000 6600 50  0001 C CNN
	1    2000 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1050 6450 1550 6450
Wire Wire Line
	2000 6450 2000 6600
Wire Wire Line
	1950 4400 1950 6250
Wire Wire Line
	1950 6250 1050 6250
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
Text Label 1700 6050 2    50   ~ 0
VIN
Text Label 1700 5950 2    50   ~ 0
LED+
Wire Wire Line
	13000 3500 14050 3500
Wire Wire Line
	13000 2900 14050 2900
Text Notes 1300 7950 0    59   ~ 0
VIN1 is physically close to LED+.\nThis allows two modes of operation:\n1) when VIN1 is conntected to LED+\nvia a solder bridge, then the LEDs are \npowered from the receiver.\n2) A separate power supply can be \nconnected to LED+ (and the nearby GND),\ne.g. for higher voltages
Text Notes 1300 5550 0    85   ~ 0
Slave out
Wire Wire Line
	2400 2400 4000 2400
Text Label 1700 3200 0    50   ~ 0
ST-Rx-in
Text Label 1700 3600 0    50   ~ 0
TH-Tx-in
Text Label 1700 4000 0    50   ~ 0
CH3-in
Text Label 1700 4400 0    50   ~ 0
OUT-ISP-out
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5F1D0E80
P 850 4700
F 0 "J2" H 768 4175 50  0000 C CNN
F 1 "Conn_01x06" H 768 4266 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 850 4700 50  0001 C CNN
F 3 "~" H 850 4700 50  0001 C CNN
	1    850  4700
	-1   0    0    1   
$EndComp
Wire Wire Line
	1050 4400 1950 4400
Wire Wire Line
	1450 4000 1450 4500
Wire Wire Line
	1450 4500 1050 4500
Wire Wire Line
	1300 3600 1300 4600
Wire Wire Line
	1300 4600 1050 4600
Wire Wire Line
	1300 3600 2250 3600
Wire Wire Line
	1200 3200 1200 4700
Wire Wire Line
	1200 4700 1050 4700
Wire Wire Line
	1200 3200 2250 3200
Wire Wire Line
	1050 4800 1600 4800
Text Label 1600 4800 2    50   ~ 0
VIN
Wire Wire Line
	15600 2550 15600 1750
Text Label 15600 1750 0    50   ~ 0
LED+
Connection ~ 15600 2550
Connection ~ 15600 2650
Wire Wire Line
	15600 2650 15600 2550
Connection ~ 15600 2750
Wire Wire Line
	15600 2750 15600 2650
Connection ~ 15600 2850
Wire Wire Line
	15600 2850 15600 2750
Connection ~ 15600 2950
Wire Wire Line
	15600 2950 15600 2850
Connection ~ 15600 3050
Wire Wire Line
	15600 3050 15600 2950
Connection ~ 15600 3150
Wire Wire Line
	15600 3150 15600 3050
Wire Wire Line
	15600 3150 15600 3250
Connection ~ 15600 3250
Wire Wire Line
	15600 3250 15600 3350
Connection ~ 15600 3350
Wire Wire Line
	15600 3350 15600 3450
Connection ~ 15600 3450
Wire Wire Line
	15600 3450 15600 3550
Connection ~ 15600 3550
Wire Wire Line
	15600 3550 15600 3650
Connection ~ 15600 3650
Wire Wire Line
	15600 3650 15600 3750
Connection ~ 15600 3750
Wire Wire Line
	15600 3750 15600 3850
Connection ~ 15600 3850
Wire Wire Line
	15600 3850 15600 3950
Connection ~ 15600 3950
Wire Wire Line
	15600 3950 15600 4050
Connection ~ 15600 4050
Wire Wire Line
	15600 4050 15600 4150
Wire Wire Line
	14600 1700 14600 2550
Wire Wire Line
	14600 2550 14850 2550
Wire Wire Line
	13000 1700 14600 1700
Wire Wire Line
	14850 2650 14600 2650
Wire Wire Line
	14600 2650 14600 2550
Connection ~ 14600 2550
Wire Wire Line
	14400 2300 14400 2750
Wire Wire Line
	14400 2750 14850 2750
Wire Wire Line
	13000 2300 14400 2300
Wire Wire Line
	14850 2850 14400 2850
Wire Wire Line
	14400 2850 14400 2750
Connection ~ 14400 2750
Wire Wire Line
	14050 2900 14050 2950
Wire Wire Line
	14050 2950 14850 2950
Wire Wire Line
	14850 3050 14050 3050
Wire Wire Line
	14050 3050 14050 2950
Connection ~ 14050 2950
Wire Wire Line
	14850 3150 14050 3150
Wire Wire Line
	14050 3150 14050 3250
Wire Wire Line
	14850 3250 14050 3250
Connection ~ 14050 3250
Wire Wire Line
	14050 3250 14050 3500
Wire Wire Line
	14850 3350 14150 3350
Wire Wire Line
	14150 3350 14150 3450
Wire Wire Line
	13000 4100 14150 4100
Wire Wire Line
	14850 3450 14150 3450
Connection ~ 14150 3450
Wire Wire Line
	14150 3450 14150 4100
Wire Wire Line
	14850 3550 14200 3550
Wire Wire Line
	14200 3550 14200 3650
Wire Wire Line
	13000 4700 14200 4700
Wire Wire Line
	14850 3650 14200 3650
Connection ~ 14200 3650
Wire Wire Line
	14200 3650 14200 4700
Wire Wire Line
	14850 3750 14300 3750
Wire Wire Line
	14300 3750 14300 3850
Wire Wire Line
	13000 5300 14300 5300
Wire Wire Line
	14850 3850 14300 3850
Connection ~ 14300 3850
Wire Wire Line
	14300 3850 14300 5300
Wire Wire Line
	14850 3950 14450 3950
Wire Wire Line
	14450 3950 14450 4050
Wire Wire Line
	13000 5900 14450 5900
Wire Wire Line
	14850 4050 14450 4050
Connection ~ 14450 4050
Wire Wire Line
	14450 4050 14450 5900
Wire Wire Line
	14850 4150 14850 6500
Wire Wire Line
	13000 6500 14850 6500
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 5F244636
P 850 6250
F 0 "J3" H 768 5725 50  0000 C CNN
F 1 "Conn_01x06" H 768 5816 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 850 6250 50  0001 C CNN
F 3 "~" H 850 6250 50  0001 C CNN
	1    850  6250
	-1   0    0    1   
$EndComp
Wire Wire Line
	1050 6050 1250 6050
Wire Wire Line
	1050 6350 1250 6350
Wire Wire Line
	1250 6350 1250 6050
Connection ~ 1250 6050
Wire Wire Line
	1250 6050 1700 6050
Wire Wire Line
	1050 6150 1550 6150
Wire Wire Line
	1550 6150 1550 6450
Connection ~ 1550 6450
Wire Wire Line
	1550 6450 2000 6450
Wire Wire Line
	1050 5950 1700 5950
Wire Wire Line
	15600 2850 15350 2850
Wire Wire Line
	15350 2750 15600 2750
Wire Wire Line
	15350 2650 15600 2650
Wire Wire Line
	15350 4150 15600 4150
Wire Wire Line
	15350 4050 15600 4050
Wire Wire Line
	15350 3950 15600 3950
Wire Wire Line
	15350 3850 15600 3850
Wire Wire Line
	15350 3750 15600 3750
Wire Wire Line
	15350 3650 15600 3650
Wire Wire Line
	15350 3550 15600 3550
Wire Wire Line
	15350 3450 15600 3450
Wire Wire Line
	15350 2550 15600 2550
Wire Wire Line
	15350 3350 15600 3350
Wire Wire Line
	15350 3250 15600 3250
Wire Wire Line
	15350 3150 15600 3150
Wire Wire Line
	15350 3050 15600 3050
Wire Wire Line
	15350 2950 15600 2950
$Comp
L Connector_Generic:Conn_02x17_Odd_Even J1
U 1 1 5F1ABEB8
P 15150 3350
F 0 "J1" H 15200 4367 50  0000 C CNN
F 1 "Conn_02x17_Odd_Even" H 15200 4276 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x17_P2.54mm_Horizontal" H 15150 3350 50  0001 C CNN
F 3 "~" H 15150 3350 50  0001 C CNN
	1    15150 3350
	-1   0    0    -1  
$EndComp
$EndSCHEMATC
