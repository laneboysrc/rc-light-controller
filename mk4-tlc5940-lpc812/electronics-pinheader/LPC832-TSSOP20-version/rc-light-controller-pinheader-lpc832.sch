EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "DIY RC Light Controller Mk4 S"
Date "2022-01-09"
Rev "3"
Comp "LANE Boys RC"
Comment1 "laneboysrc@gmail.com"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5250 2300 5250 1900
Wire Wire Line
	3550 3100 3550 2850
Wire Wire Line
	4400 2600 4400 3100
Wire Wire Line
	2350 6050 2350 5950
Wire Wire Line
	2350 5950 1950 5950
Text Label 4050 5350 2    50   ~ 0
TH-TX
Wire Wire Line
	3550 2550 3550 2300
Connection ~ 3550 2300
Text Label 2550 2300 0    70   ~ 0
VIN
Text Label 4050 5250 2    50   ~ 0
ST-RX
Text Label 4050 5450 2    50   ~ 0
OUT-ISP
Text Label 4050 5550 2    50   ~ 0
CH3
$Comp
L Device:C C1
U 1 1 30D010B6
P 3550 2700
F 0 "C1" H 3650 2750 59  0000 L BNN
F 1 "1u/16V" H 3650 2550 59  0000 L BNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3550 2700 50  0001 C CNN
F 3 "" H 3550 2700 50  0001 C CNN
	1    3550 2700
	1    0    0    -1  
$EndComp
Text Notes 6050 9650 0    56   ~ 0
Special pins:\nPIO0_0  (19)   ISP UART RX\nPIO0_4  ( 5)   ISP UART TX\nPIO0_5  ( 4)   RESET\nPIO0_10 ( 9)   Open drain\nPIO0_11 ( 8)   Open drain\nPIO0_2  ( 7)   SWDIO\nPIO0_3  ( 6)   SWCLK
Text Notes 1900 4900 0    85   ~ 0
Servo/Pre-processor in/out
Text Notes 3850 1650 0    85   ~ 0
Voltage regulator
Text Notes 7300 6300 0    85   ~ 0
Microcontroller
Text Notes 2950 2900 0    59   ~ 0
X7R or X5R
Text Notes 2550 2550 0    59   ~ 0
Input voltage range:\n4 ... 10V
Text Notes 5550 2500 0    59   ~ 0
LDO: \nMCP1702, MCP1703\nME6209A33M3G
Text Notes 8400 9300 0    59   ~ 0
NXP LPC832\nARM Cortex-M0\n16K Flash, 4K RAM\nTSSOP-20
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
	5250 2300 5250 2700
Connection ~ 5250 2300
Wire Wire Line
	5250 3000 5250 3100
$Comp
L Device:CP C2
U 1 1 5C870864
P 5250 2850
F 0 "C2" H 5368 2896 50  0000 L CNN
F 1 "47u/6V3" H 5368 2805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5288 2700 50  0001 C CNN
F 3 "https://www.vishay.com/doc?40189" H 5250 2850 50  0001 C CNN
	1    5250 2850
	1    0    0    -1  
$EndComp
Text Notes 5350 3100 0    50   ~ 0
Polymer
$Comp
L power:GND #PWR0101
U 1 1 5CCABD51
P 2350 6050
F 0 "#PWR0101" H 2350 5800 50  0001 C CNN
F 1 "GND" H 2355 5877 50  0000 C CNN
F 2 "" H 2350 6050 50  0001 C CNN
F 3 "" H 2350 6050 50  0001 C CNN
	1    2350 6050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CCAC0F7
P 7700 9150
F 0 "#PWR0103" H 7700 8900 50  0001 C CNN
F 1 "GND" H 7705 8977 50  0000 C CNN
F 2 "" H 7700 9150 50  0001 C CNN
F 3 "" H 7700 9150 50  0001 C CNN
	1    7700 9150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5CCAC156
P 3550 3100
F 0 "#PWR0104" H 3550 2850 50  0001 C CNN
F 1 "GND" H 3555 2927 50  0000 C CNN
F 2 "" H 3550 3100 50  0001 C CNN
F 3 "" H 3550 3100 50  0001 C CNN
	1    3550 3100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5CCAC492
P 4400 3100
F 0 "#PWR0105" H 4400 2850 50  0001 C CNN
F 1 "GND" H 4405 2927 50  0000 C CNN
F 2 "" H 4400 3100 50  0001 C CNN
F 3 "" H 4400 3100 50  0001 C CNN
	1    4400 3100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5CCAC4F1
P 5250 3100
F 0 "#PWR0106" H 5250 2850 50  0001 C CNN
F 1 "GND" H 5255 2927 50  0000 C CNN
F 2 "" H 5250 3100 50  0001 C CNN
F 3 "" H 5250 3100 50  0001 C CNN
	1    5250 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 9050 7700 9100
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
Text Notes 13250 1750 0    85   ~ 0
LED output
Wire Wire Line
	7700 6650 7700 6850
Text Label 9150 8550 2    50   ~ 0
SIN
$Comp
L power:GND #PWR02
U 1 1 5EFD836E
P 2050 7900
F 0 "#PWR02" H 2050 7650 50  0001 C CNN
F 1 "GND" H 2055 7727 50  0000 C CNN
F 2 "" H 2050 7900 50  0001 C CNN
F 3 "" H 2050 7900 50  0001 C CNN
	1    2050 7900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 7750 2050 7750
Wire Wire Line
	2050 7750 2050 7900
Wire Wire Line
	2850 7550 1950 7550
Text Label 2850 7250 2    50   ~ 0
VIN
Text Label 2850 7350 2    50   ~ 0
LED+
Text Notes 1700 8900 0    59   ~ 0
VIN1 is physically close to LED+.\nThis allows two modes of operation:\n1) when VIN1 is conntected to LED+\nvia a jumper, then the LEDs are \npowered from the receiver.\n2) A separate power supply can be \nconnected to LED+ (and the nearby GND),\ne.g. for higher voltages
Text Notes 1700 6950 0    85   ~ 0
Output connector
Wire Wire Line
	2550 2300 3550 2300
Text Label 2800 5250 2    50   ~ 0
ST-Rx-in
Text Label 2800 5350 2    50   ~ 0
TH-Tx-in
Text Label 2800 5550 2    50   ~ 0
CH3-in
Text Label 2800 5450 2    50   ~ 0
OUT-ISP-out
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5F1D0E80
P 1750 5750
F 0 "J2" H 1668 5225 50  0000 C CNN
F 1 "Pinheader straight 1x06" H 1668 5316 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 1750 5750 50  0001 C CNN
F 3 "~" H 1750 5750 50  0001 C CNN
	1    1750 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	2200 5350 2200 5650
Wire Wire Line
	2200 5650 1950 5650
Wire Wire Line
	2100 5250 2100 5750
Wire Wire Line
	2100 5750 1950 5750
Wire Wire Line
	1950 5850 2800 5850
Text Label 2800 5850 2    50   ~ 0
VIN
Wire Wire Line
	14450 2550 14450 2300
Text Label 14450 2300 0    50   ~ 0
LED+
Connection ~ 14450 2550
Connection ~ 14450 2650
Wire Wire Line
	14450 2650 14450 2550
Connection ~ 14450 2750
Wire Wire Line
	14450 2750 14450 2650
Connection ~ 14450 2850
Wire Wire Line
	14450 2850 14450 2750
Connection ~ 14450 2950
Wire Wire Line
	14450 2950 14450 2850
Connection ~ 14450 3050
Wire Wire Line
	14450 3050 14450 2950
Connection ~ 14450 3150
Wire Wire Line
	14450 3150 14450 3050
Wire Wire Line
	14450 3150 14450 3250
Connection ~ 14450 3250
Wire Wire Line
	14450 3250 14450 3350
Connection ~ 14450 3350
Wire Wire Line
	14450 3350 14450 3450
Connection ~ 14450 3450
Wire Wire Line
	14450 3450 14450 3550
Connection ~ 14450 3550
Wire Wire Line
	14450 3550 14450 3650
Connection ~ 14450 3650
Wire Wire Line
	14450 3650 14450 3750
Connection ~ 14450 3750
Wire Wire Line
	14450 3750 14450 3850
Connection ~ 14450 3850
Wire Wire Line
	14450 3850 14450 3950
Connection ~ 14450 3950
Wire Wire Line
	14450 3950 14450 4050
Connection ~ 14450 4050
Wire Wire Line
	14450 4050 14450 4150
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 5F244636
P 1750 7550
F 0 "J3" H 1668 7025 50  0000 C CNN
F 1 "Pinheader straight 1x06" H 1668 7116 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:PinHeader_1x06_P2.54mm_Flat" H 1750 7550 50  0001 C CNN
F 3 "~" H 1750 7550 50  0001 C CNN
	1    1750 7550
	-1   0    0    1   
$EndComp
Wire Wire Line
	1950 7650 2150 7650
Wire Wire Line
	1950 7450 2050 7450
Wire Wire Line
	2050 7450 2050 7750
Connection ~ 2050 7750
Wire Wire Line
	1950 7250 2150 7250
Wire Wire Line
	14450 2850 14200 2850
Wire Wire Line
	14200 2750 14450 2750
Wire Wire Line
	14200 2650 14450 2650
Wire Wire Line
	14200 4150 14450 4150
Wire Wire Line
	14200 4050 14450 4050
Wire Wire Line
	14200 3950 14450 3950
Wire Wire Line
	14200 3850 14450 3850
Wire Wire Line
	14200 3750 14450 3750
Wire Wire Line
	14200 3650 14450 3650
Wire Wire Line
	14200 3550 14450 3550
Wire Wire Line
	14200 3450 14450 3450
Wire Wire Line
	14200 2550 14450 2550
Wire Wire Line
	14200 3350 14450 3350
Wire Wire Line
	14200 3250 14450 3250
Wire Wire Line
	14200 3150 14450 3150
Wire Wire Line
	14200 3050 14450 3050
Wire Wire Line
	14200 2950 14450 2950
$Comp
L Connector_Generic:Conn_02x17_Odd_Even J1
U 1 1 5F1ABEB8
P 14000 3350
F 0 "J1" H 14050 4367 50  0000 C CNN
F 1 "Pinheader right-angle 2x17" H 14050 4276 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x17_P2.54mm_Horizontal" H 14000 3350 50  0001 C CNN
F 3 "~" H 14000 3350 50  0001 C CNN
	1    14000 3350
	-1   0    0    1   
$EndComp
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
$Comp
L Device:C C3
U 1 1 5F2418F7
P 8000 7000
F 0 "C3" H 8115 7046 50  0000 L CNN
F 1 "100n" H 8115 6955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8038 6850 50  0001 C CNN
F 3 "~" H 8000 7000 50  0001 C CNN
	1    8000 7000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5F2421F0
P 8000 7200
F 0 "#PWR03" H 8000 6950 50  0001 C CNN
F 1 "GND" H 8005 7027 50  0000 C CNN
F 2 "" H 8000 7200 50  0001 C CNN
F 3 "" H 8000 7200 50  0001 C CNN
	1    8000 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 7150 8000 7200
Wire Wire Line
	8000 6850 7700 6850
Connection ~ 7700 6850
Wire Wire Line
	7700 6850 7700 7350
Text Label 9150 8350 2    50   ~ 0
BLANK
$Comp
L power:GND #PWR01
U 1 1 5EEF5842
P 12800 7150
F 0 "#PWR01" H 12800 6900 50  0001 C CNN
F 1 "GND" H 12805 6977 50  0000 C CNN
F 2 "" H 12800 7150 50  0001 C CNN
F 3 "" H 12800 7150 50  0001 C CNN
	1    12800 7150
	1    0    0    -1  
$EndComp
Text Notes 13050 7000 0    56   ~ 0
N-Channel MOSFET\nSOT23 package\ne.g. PMV16UN, PMV30UN, \nSI2302. AO3400, DMN2075U-7
Text Label 12100 6850 0    50   ~ 0
OUT15S
Wire Wire Line
	12100 6850 12500 6850
Wire Wire Line
	12800 7050 12800 7150
$Comp
L Device:Q_NMOS_GSD T9
U 1 1 5EEE69E5
P 12700 6850
F 0 "T9" H 12600 7000 59  0000 L BNN
F 1 "PMV30UN" H 12800 6850 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 12700 6850 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 12700 6850 50  0001 C CNN
	1    12700 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10300 2550 11050 2550
Wire Wire Line
	10300 2650 11050 2650
Wire Wire Line
	10300 2750 11050 2750
Wire Wire Line
	10300 2850 11050 2850
Wire Wire Line
	10300 2950 11050 2950
Wire Wire Line
	10300 3050 11050 3050
Wire Wire Line
	10300 3150 11050 3150
Wire Wire Line
	10300 3250 11050 3250
Wire Wire Line
	10300 3350 11050 3350
Wire Wire Line
	10300 3450 11050 3450
Wire Wire Line
	10300 3550 11050 3550
Wire Wire Line
	10300 3650 11050 3650
Wire Wire Line
	10300 3750 11050 3750
Wire Wire Line
	10300 3850 11050 3850
Wire Wire Line
	10300 3950 11050 3950
Wire Wire Line
	10300 4050 11050 4050
Text Label 11050 2550 2    50   ~ 0
OUT0
Text Label 11050 2650 2    50   ~ 0
OUT1
Text Label 11050 2750 2    50   ~ 0
OUT2
Text Label 11050 2850 2    50   ~ 0
OUT3
Text Label 11050 2950 2    50   ~ 0
OUT4
Text Label 11050 3050 2    50   ~ 0
OUT5
Text Label 11050 3150 2    50   ~ 0
OUT6
Text Label 11050 3250 2    50   ~ 0
OUT7
Text Label 11050 3350 2    50   ~ 0
OUT8
Text Label 11050 3450 2    50   ~ 0
OUT9
Text Label 11050 3550 2    50   ~ 0
OUT10
Text Label 11050 3650 2    50   ~ 0
OUT11
Text Label 11050 3750 2    50   ~ 0
OUT12
Text Label 11050 3850 2    50   ~ 0
OUT13
Text Label 11050 3950 2    50   ~ 0
OUT14
Text Label 11050 4050 2    50   ~ 0
OUT15
$Comp
L power:GND #PWR06
U 1 1 5F231909
P 9600 4450
F 0 "#PWR06" H 9600 4200 50  0001 C CNN
F 1 "GND" H 9605 4277 50  0000 C CNN
F 2 "" H 9600 4450 50  0001 C CNN
F 3 "" H 9600 4450 50  0001 C CNN
	1    9600 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 4350 9600 4400
$Comp
L Device:R R1
U 1 1 5F23623F
P 8000 2950
F 0 "R1" H 8070 2996 50  0000 L CNN
F 1 "2k 1%" H 8070 2905 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7930 2950 50  0001 C CNN
F 3 "~" H 8000 2950 50  0001 C CNN
	1    8000 2950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5F23655D
P 8000 3200
F 0 "#PWR04" H 8000 2950 50  0001 C CNN
F 1 "GND" H 8005 3027 50  0000 C CNN
F 2 "" H 8000 3200 50  0001 C CNN
F 3 "" H 8000 3200 50  0001 C CNN
	1    8000 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 3100 8000 3200
Wire Wire Line
	8000 2650 8000 2800
Wire Wire Line
	8900 3050 8400 3050
Text Label 8400 3050 0    50   ~ 0
BLANK
Wire Wire Line
	8400 2850 8900 2850
Text Label 8400 2850 0    50   ~ 0
GSCLK
Wire Wire Line
	8400 3850 8900 3850
Text Label 8400 3850 0    50   ~ 0
SCLK
Wire Wire Line
	8400 3950 8900 3950
Text Label 8400 3950 0    50   ~ 0
SIN
Wire Wire Line
	8400 3150 8900 3150
Text Label 8400 3150 0    50   ~ 0
XLAT
Wire Wire Line
	13700 4050 13000 4050
Wire Wire Line
	13000 3950 13700 3950
Wire Wire Line
	13000 3850 13700 3850
Wire Wire Line
	13000 3750 13700 3750
Wire Wire Line
	13000 3650 13700 3650
Wire Wire Line
	13000 3550 13700 3550
Wire Wire Line
	13000 3450 13700 3450
Wire Wire Line
	13000 3350 13700 3350
Wire Wire Line
	13000 3250 13700 3250
Wire Wire Line
	13000 3150 13700 3150
Wire Wire Line
	13000 3050 13700 3050
Wire Wire Line
	13000 2950 13700 2950
Wire Wire Line
	13000 2850 13700 2850
Wire Wire Line
	13000 2750 13700 2750
Wire Wire Line
	13000 2650 13700 2650
Wire Wire Line
	13000 2550 13700 2550
Text Label 13000 2550 0    50   ~ 0
OUT0
Text Label 13000 2650 0    50   ~ 0
OUT1
Text Label 13000 2750 0    50   ~ 0
OUT2
Text Label 13000 2850 0    50   ~ 0
OUT3
Text Label 13000 2950 0    50   ~ 0
OUT4
Text Label 13000 3050 0    50   ~ 0
OUT5
Text Label 13000 3150 0    50   ~ 0
OUT6
Text Label 13000 3250 0    50   ~ 0
OUT7
Text Label 13000 3350 0    50   ~ 0
OUT8
Text Label 13000 3450 0    50   ~ 0
OUT9
Text Label 13000 3550 0    50   ~ 0
OUT10
Text Label 13000 3650 0    50   ~ 0
OUT11
Text Label 13000 3750 0    50   ~ 0
OUT12
Text Label 13000 3850 0    50   ~ 0
OUT13
Text Label 13000 3950 0    50   ~ 0
OUT14
Text Label 13000 4050 0    50   ~ 0
OUT15
$Comp
L Driver_LED:TLC5940PWP U2
U 1 1 5F1B616D
P 9600 3250
F 0 "U2" H 9600 4431 50  0000 C CNN
F 1 "TLC5940PWP" H 9600 4340 50  0000 C CNN
F 2 "rc-light-controller-tlc5940-lpc812:SOP65P640X120-29N" H 9625 2275 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tlc5940.pdf" H 9200 3950 50  0001 C CNN
	1    9600 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 4350 9500 4400
Wire Wire Line
	9500 4400 9600 4400
Connection ~ 9600 4400
Wire Wire Line
	9600 4400 9600 4450
Wire Wire Line
	8700 2100 8700 2250
Wire Wire Line
	8000 2650 8900 2650
Wire Wire Line
	9600 2250 8700 2250
NoConn ~ 8900 3450
NoConn ~ 8900 4050
Wire Wire Line
	8900 2550 8700 2550
Wire Wire Line
	8700 2550 8700 2250
Connection ~ 8700 2250
Wire Wire Line
	8900 2750 8700 2750
Wire Wire Line
	8700 2750 8700 2550
Connection ~ 8700 2550
Text Label 2850 7550 2    50   ~ 0
OUT-ISP-out
Wire Wire Line
	4050 5550 3450 5550
Wire Wire Line
	3450 5450 4050 5450
Wire Wire Line
	4050 5350 3450 5350
Wire Wire Line
	2200 5350 3050 5350
Wire Wire Line
	4050 5250 3450 5250
Wire Wire Line
	2100 5250 3050 5250
$Comp
L Device:R_Pack04 RN1
U 1 1 5F192F7F
P 3250 5450
F 0 "RN1" V 2833 5450 50  0000 C CNN
F 1 "1k x4" V 2924 5450 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 3525 5450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/szlcsc/1810311812_UNI-ROYAL-Uniroyal-Elec-4D03WGJ0102T5E_C20197.pdf" H 3250 5450 50  0001 C CNN
	1    3250 5450
	0    1    1    0   
$EndComp
Wire Wire Line
	1950 5550 3050 5550
Wire Wire Line
	1950 5450 3050 5450
Text Label 13400 6400 2    50   ~ 0
OUT15S_OUT
Text Label 13000 4150 0    50   ~ 0
OUT15S_OUT
Wire Wire Line
	12800 6650 12800 6400
Wire Wire Line
	12800 6400 13400 6400
Text Notes 9100 1750 0    85   ~ 0
LED driver
Wire Wire Line
	13000 4150 13700 4150
Text Notes 12600 5950 0    85   ~ 0
Switched output driver
$Comp
L Regulator_Linear:MCP1703A-3302_SOT23 U3
U 1 1 5F40039F
P 4400 2300
F 0 "U3" H 4400 2542 50  0000 C CNN
F 1 "MCP1703A-3302_SOT23" H 4400 2451 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23W" H 4400 2500 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/20005122B.pdf" H 4400 2250 50  0001 C CNN
	1    4400 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 2300 4100 2300
Wire Wire Line
	4700 2300 5250 2300
$Comp
L power:+3V3 #PWR05
U 1 1 5F1AFD5A
P 5250 1900
F 0 "#PWR05" H 5250 1750 50  0001 C CNN
F 1 "+3V3" H 5265 2073 50  0000 C CNN
F 2 "" H 5250 1900 50  0001 C CNN
F 3 "" H 5250 1900 50  0001 C CNN
	1    5250 1900
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR08
U 1 1 5F1B1B4B
P 8700 2100
F 0 "#PWR08" H 8700 1950 50  0001 C CNN
F 1 "+3V3" H 8715 2273 50  0000 C CNN
F 2 "" H 8700 2100 50  0001 C CNN
F 3 "" H 8700 2100 50  0001 C CNN
	1    8700 2100
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR07
U 1 1 5F1BA1EF
P 7700 6650
F 0 "#PWR07" H 7700 6500 50  0001 C CNN
F 1 "+3V3" H 7715 6823 50  0000 C CNN
F 2 "" H 7700 6650 50  0001 C CNN
F 3 "" H 7700 6650 50  0001 C CNN
	1    7700 6650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 7350 2850 7350
Wire Wire Line
	2150 7650 2150 7250
Connection ~ 2150 7250
Wire Wire Line
	2150 7250 2850 7250
Text Notes 9900 8500 0    50   ~ 0
PIO0_14 serves as detection whether \nthe hardware has the TLC5940 or is the switching version. \nFor TLC5940 it must be left floating (pull-up).\nFor switching version it must be pulled to GND.
$Comp
L MCU_NXP_LPC:LPC832M101FDH20 U1
U 1 1 60608195
P 7800 8250
F 0 "U1" H 8350 8950 50  0000 C CNN
F 1 "LPC832M101FDH20" H 7200 8950 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 8850 9000 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC83X.pdf" H 7950 8900 50  0001 L CNN
	1    7800 8250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 7850 6400 7850
Text Label 6400 7850 0    50   ~ 0
ST-RX
Wire Wire Line
	6400 7950 7000 7950
Text Label 6400 7950 0    50   ~ 0
GSCLK
Wire Wire Line
	6400 8050 7000 8050
Text Label 6400 8050 0    50   ~ 0
SCLK
Wire Wire Line
	6400 8150 7000 8150
Text Label 6400 8150 0    50   ~ 0
XLAT
Wire Wire Line
	6400 8250 7000 8250
Text Label 6400 8250 0    50   ~ 0
TH-TX
Wire Wire Line
	6400 8550 7000 8550
Text Label 6400 8550 0    50   ~ 0
OUT15S
Wire Wire Line
	9150 8050 8600 8050
Text Label 9150 8050 2    50   ~ 0
OUT-ISP
Wire Wire Line
	9150 8150 8600 8150
Text Label 9150 8150 2    50   ~ 0
CH3
Wire Wire Line
	9150 8250 8600 8250
NoConn ~ 9150 8250
NoConn ~ 7000 8350
Wire Wire Line
	9150 8350 8600 8350
NoConn ~ 8600 7950
NoConn ~ 8600 8450
Wire Wire Line
	7900 7450 7900 7350
Wire Wire Line
	7900 7350 7700 7350
Connection ~ 7700 7350
Wire Wire Line
	7700 7350 7700 7450
Wire Wire Line
	7900 9050 7900 9100
Wire Wire Line
	7900 9100 7700 9100
Connection ~ 7700 9100
Wire Wire Line
	7700 9100 7700 9150
NoConn ~ 7000 8450
Wire Wire Line
	9150 8550 8600 8550
Text Notes 9900 9200 0    50   ~ 0
Pin assignment is similar to LPC812,\nwith the following exceptions:\nBLANK is PIO0_15 instesad of PIO0_6\nSIN is PIO0_23 instesad of PIO0_7
$Comp
L Device:Q_NMOS_GSD T1
U 1 1 61DAA0C6
P 10100 6900
F 0 "T1" H 10000 7050 59  0000 L BNN
F 1 "PMV30UN" H 10200 6900 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 10100 6900 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMV30UN.pdf" H 10100 6900 50  0001 C CNN
	1    10100 6900
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 61DB406C
P 10000 7200
F 0 "#PWR010" H 10000 6950 50  0001 C CNN
F 1 "GND" H 10005 7027 50  0000 C CNN
F 2 "" H 10000 7200 50  0001 C CNN
F 3 "" H 10000 7200 50  0001 C CNN
	1    10000 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 7100 10000 7200
$Comp
L Device:R R2
U 1 1 61DBD148
P 10000 6400
F 0 "R2" H 10070 6446 50  0000 L CNN
F 1 "100k" H 10070 6355 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9930 6400 50  0001 C CNN
F 3 "~" H 10000 6400 50  0001 C CNN
	1    10000 6400
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR09
U 1 1 61DC58F4
P 10000 6150
F 0 "#PWR09" H 10000 6000 50  0001 C CNN
F 1 "+3V3" H 10015 6323 50  0000 C CNN
F 2 "" H 10000 6150 50  0001 C CNN
F 3 "" H 10000 6150 50  0001 C CNN
	1    10000 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 6150 10000 6250
Text Label 10700 6900 2    50   ~ 0
ST-RX
Wire Wire Line
	10300 6900 10700 6900
Wire Wire Line
	8600 7850 9250 7850
Wire Wire Line
	10000 6550 10000 6650
Wire Wire Line
	9250 7850 9250 6650
Wire Wire Line
	9250 6650 10000 6650
Connection ~ 10000 6650
Wire Wire Line
	10000 6650 10000 6700
Text Notes 9600 5850 0    85   ~ 0
S.BUS inverter
$EndSCHEMATC
