EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Pre-processor for RC Light Controller"
Date "2021-04-11"
Rev "3"
Comp "LANE Boys RC"
Comment1 "Support for 3 AUX channels"
Comment2 "LPC832 version due to LPC812 shortage in 2021"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Regulator_Linear:MCP1703A-3302_SOT23 U1
U 1 1 58AF9BBC
P 8800 5300
F 0 "U1" H 8600 5400 50  0000 L BNN
F 1 "MCP1703T-3302E/CB" H 8600 5500 50  0000 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23W" H 8950 4850 30  0001 C CNN
F 3 "" H 8800 5300 60  0000 C CNN
	1    8800 5300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 P1
U 1 1 58AF9C1E
P 1650 1900
F 0 "P1" H 1650 2100 50  0000 C CNN
F 1 "ST-IN" V 1750 1900 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 1900 50  0001 C CNN
F 3 "" H 1650 1900 50  0000 C CNN
	1    1650 1900
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 P2
U 1 1 58AF9CB3
P 1650 2400
F 0 "P2" H 1650 2600 50  0000 C CNN
F 1 "ST-OUT" V 1750 2400 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 2400 50  0001 C CNN
F 3 "" H 1650 2400 50  0000 C CNN
	1    1650 2400
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 P3
U 1 1 58AF9CE6
P 1650 2900
F 0 "P3" H 1650 3100 50  0000 C CNN
F 1 "TH-IN" V 1750 2900 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 2900 50  0001 C CNN
F 3 "" H 1650 2900 50  0000 C CNN
	1    1650 2900
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 P4
U 1 1 58AF9D12
P 1650 3350
F 0 "P4" H 1650 3550 50  0000 C CNN
F 1 "TH-OUT" V 1750 3350 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 3350 50  0001 C CNN
F 3 "" H 1650 3350 50  0000 C CNN
	1    1650 3350
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 P5
U 1 1 58AF9D7F
P 1650 3850
F 0 "P5" H 1650 4050 50  0000 C CNN
F 1 "AUX-IN" V 1750 3850 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 3850 50  0001 C CNN
F 3 "" H 1650 3850 50  0000 C CNN
	1    1650 3850
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 P6
U 1 1 58AF9DBB
P 9600 1800
F 0 "P6" H 9600 2000 50  0000 C CNN
F 1 "LIGHT CONTROLLER" V 9700 1800 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 9600 1800 50  0001 C CNN
F 3 "" H 9600 1800 50  0000 C CNN
	1    9600 1800
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 58AFA097
P 1950 5550
F 0 "#PWR01" H 1950 5300 50  0001 C CNN
F 1 "GND" H 1950 5400 50  0000 C CNN
F 2 "" H 1950 5550 50  0000 C CNN
F 3 "" H 1950 5550 50  0000 C CNN
	1    1950 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 58AFA0BE
P 8050 5800
F 0 "#PWR02" H 8050 5550 50  0001 C CNN
F 1 "GND" H 8050 5650 50  0000 C CNN
F 2 "" H 8050 5800 50  0000 C CNN
F 3 "" H 8050 5800 50  0000 C CNN
	1    8050 5800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 58AFA0EC
P 4850 3450
F 0 "#PWR03" H 4850 3200 50  0001 C CNN
F 1 "GND" H 4850 3300 50  0000 C CNN
F 2 "" H 4850 3450 50  0000 C CNN
F 3 "" H 4850 3450 50  0000 C CNN
	1    4850 3450
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR04
U 1 1 58AFA135
P 4850 1650
F 0 "#PWR04" H 4850 1500 50  0001 C CNN
F 1 "+3V3" H 4850 1790 50  0000 C CNN
F 2 "" H 4850 1650 50  0000 C CNN
F 3 "" H 4850 1650 50  0000 C CNN
	1    4850 1650
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR05
U 1 1 58AFA16C
P 2100 1200
F 0 "#PWR05" H 2100 1050 50  0001 C CNN
F 1 "VCC" H 2100 1350 50  0000 C CNN
F 2 "" H 2100 1200 50  0000 C CNN
F 3 "" H 2100 1200 50  0000 C CNN
	1    2100 1200
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR06
U 1 1 58AFA193
P 8050 5200
F 0 "#PWR06" H 8050 5050 50  0001 C CNN
F 1 "VCC" H 8050 5350 50  0000 C CNN
F 2 "" H 8050 5200 50  0000 C CNN
F 3 "" H 8050 5200 50  0000 C CNN
	1    8050 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 58AFA243
P 8050 5500
F 0 "C1" H 8075 5600 50  0000 L CNN
F 1 "1u" H 8075 5400 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8088 5350 50  0001 C CNN
F 3 "" H 8050 5500 50  0000 C CNN
	1    8050 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 58AFA4AA
P 8800 5800
F 0 "#PWR07" H 8800 5550 50  0001 C CNN
F 1 "GND" H 8800 5650 50  0000 C CNN
F 2 "" H 8800 5800 50  0000 C CNN
F 3 "" H 8800 5800 50  0000 C CNN
	1    8800 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 5200 8050 5300
Wire Wire Line
	8050 5650 8050 5800
Connection ~ 8050 5300
$Comp
L Device:CP C2
U 1 1 58AFA5B0
P 9700 5550
F 0 "C2" H 9725 5650 50  0000 L CNN
F 1 "47u/6V3 Polymer 0805" H 9725 5450 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9738 5400 50  0001 C CNN
F 3 "" H 9700 5550 50  0000 C CNN
	1    9700 5550
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR08
U 1 1 58AFA5F4
P 9700 5200
F 0 "#PWR08" H 9700 5050 50  0001 C CNN
F 1 "+3V3" H 9700 5340 50  0000 C CNN
F 2 "" H 9700 5200 50  0000 C CNN
F 3 "" H 9700 5200 50  0000 C CNN
	1    9700 5200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 58AFA6CA
P 9700 5800
F 0 "#PWR09" H 9700 5550 50  0001 C CNN
F 1 "GND" H 9700 5650 50  0000 C CNN
F 2 "" H 9700 5800 50  0000 C CNN
F 3 "" H 9700 5800 50  0000 C CNN
	1    9700 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 5700 9700 5800
Wire Wire Line
	9700 5200 9700 5300
Connection ~ 9700 5300
Wire Wire Line
	2100 1900 1850 1900
Wire Wire Line
	2100 1200 2100 1350
Wire Wire Line
	2100 2400 1850 2400
Connection ~ 2100 1900
Wire Wire Line
	2100 2900 1850 2900
Connection ~ 2100 2400
Wire Wire Line
	2100 3350 1850 3350
Connection ~ 2100 2900
Wire Wire Line
	2100 3850 1850 3850
Connection ~ 2100 3350
Wire Wire Line
	1950 3950 1850 3950
Wire Wire Line
	1950 2000 1950 2500
Wire Wire Line
	1850 3450 1950 3450
Connection ~ 1950 3950
Wire Wire Line
	1850 3000 1950 3000
Connection ~ 1950 3450
Wire Wire Line
	1850 2500 1950 2500
Connection ~ 1950 3000
Wire Wire Line
	1850 2000 1950 2000
Connection ~ 1950 2500
$Comp
L power:VCC #PWR010
U 1 1 58AFAD17
P 9300 1500
F 0 "#PWR010" H 9300 1350 50  0001 C CNN
F 1 "VCC" H 9300 1650 50  0000 C CNN
F 2 "" H 9300 1500 50  0000 C CNN
F 3 "" H 9300 1500 50  0000 C CNN
	1    9300 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 58AFADBD
P 9300 2000
F 0 "#PWR011" H 9300 1750 50  0001 C CNN
F 1 "GND" H 9300 1850 50  0000 C CNN
F 2 "" H 9300 2000 50  0000 C CNN
F 3 "" H 9300 2000 50  0000 C CNN
	1    9300 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 1800 9300 1800
Wire Wire Line
	9300 1800 9300 1500
Wire Wire Line
	9400 1900 9300 1900
Wire Wire Line
	9300 1900 9300 2000
Wire Wire Line
	1850 1800 2450 1800
Wire Wire Line
	1850 2800 2450 2800
Wire Wire Line
	8600 1700 9400 1700
NoConn ~ 5750 2150
NoConn ~ 4150 2350
NoConn ~ 4150 2450
NoConn ~ 4150 2850
Wire Wire Line
	1850 2300 2450 2300
Wire Wire Line
	2450 2300 2450 1800
Connection ~ 2450 1800
Wire Wire Line
	1850 3250 2450 3250
Wire Wire Line
	2450 3250 2450 2800
Connection ~ 2450 2800
$Comp
L power:PWR_FLAG #FLG012
U 1 1 58AFB479
P 2400 1250
F 0 "#FLG012" H 2400 1345 50  0001 C CNN
F 1 "PWR_FLAG" H 2400 1430 50  0000 C CNN
F 2 "" H 2400 1250 50  0000 C CNN
F 3 "" H 2400 1250 50  0000 C CNN
	1    2400 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 1250 2400 1350
Wire Wire Line
	2400 1350 2100 1350
Connection ~ 2100 1350
Text Label 2900 3750 2    50   ~ 0
AUX_IN
Text Label 2900 2800 2    50   ~ 0
TH_IN
Text Label 2900 1800 2    50   ~ 0
ST_IN
Text Label 8600 1700 0    50   ~ 0
LIGHT_OUT
$Comp
L power:PWR_FLAG #FLG013
U 1 1 58C7863F
P 2250 5400
F 0 "#FLG013" H 2250 5475 50  0001 C CNN
F 1 "PWR_FLAG" H 2250 5550 50  0000 C CNN
F 2 "" H 2250 5400 50  0001 C CNN
F 3 "" H 2250 5400 50  0001 C CNN
	1    2250 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 5400 2250 5500
Wire Wire Line
	2250 5500 1950 5500
Connection ~ 1950 5500
Wire Wire Line
	8050 5300 8050 5350
Wire Wire Line
	9700 5300 9700 5400
Wire Wire Line
	2100 1900 2100 2400
Wire Wire Line
	2100 2400 2100 2900
Wire Wire Line
	2100 2900 2100 3350
Wire Wire Line
	2100 3350 2100 3850
Wire Wire Line
	1950 3450 1950 3950
Wire Wire Line
	1950 3000 1950 3450
Wire Wire Line
	1950 2500 1950 3000
Wire Wire Line
	2450 1800 2900 1800
Wire Wire Line
	2450 2800 2900 2800
Wire Wire Line
	2100 1350 2100 1900
Wire Wire Line
	1950 5500 1950 5550
$Comp
L Connector_Generic:Conn_01x03 P7
U 1 1 5D282BAE
P 1650 4350
F 0 "P7" H 1650 4550 50  0000 C CNN
F 1 "AUX2-IN" V 1750 4350 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 4350 50  0001 C CNN
F 3 "" H 1650 4350 50  0000 C CNN
	1    1650 4350
	-1   0    0    1   
$EndComp
Wire Wire Line
	2100 4350 1850 4350
Wire Wire Line
	1950 4450 1850 4450
Connection ~ 2100 3850
$Comp
L Connector_Generic:Conn_01x03 P8
U 1 1 5D287718
P 1650 4850
F 0 "P8" H 1650 5050 50  0000 C CNN
F 1 "AUX3-IN" V 1750 4850 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 4850 50  0001 C CNN
F 3 "" H 1650 4850 50  0000 C CNN
	1    1650 4850
	-1   0    0    1   
$EndComp
Wire Wire Line
	2100 4850 1850 4850
Wire Wire Line
	1950 4950 1850 4950
Connection ~ 1950 4950
Text Label 2900 4750 2    50   ~ 0
AUX3_IN
Wire Wire Line
	1950 4950 1950 5500
Wire Wire Line
	1950 3950 1950 4450
Wire Wire Line
	2100 3850 2100 4350
Wire Wire Line
	1950 4450 1950 4950
Connection ~ 1950 4450
Wire Wire Line
	2100 4350 2100 4850
Connection ~ 2100 4350
$Comp
L Device:R R7
U 1 1 5D282842
P 6150 3000
F 0 "R7" V 6230 3000 50  0000 C CNN
F 1 "100k" V 6150 3000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 6080 3000 50  0001 C CNN
F 3 "" H 6150 3000 50  0001 C CNN
	1    6150 3000
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5D284613
P 6150 3300
F 0 "#PWR012" H 6150 3050 50  0001 C CNN
F 1 "GND" H 6150 3150 50  0000 C CNN
F 2 "" H 6150 3300 50  0000 C CNN
F 3 "" H 6150 3300 50  0000 C CNN
	1    6150 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 3150 6150 3300
Text Notes 2600 7000 0    50   ~ 0
Light Controller pin usage:\nPIO0_0   (16, TDO, ISP-Rx)   Steering input / Rx\nPIO0_1   (9,  TDI)           TLC5940 GSCLK\nPIO0_2   (6,  TMS, SWDIO)    TLC5940 SCLK\nPIO0_3   (5,  TCK, SWCLK)    TLC5940 XLAT\nPIO0_4   (4,  TRST, ISP-Tx)  Throttle input / Tx\nPIO0_5   (3,  RESET)         NC (test point)\nPIO0_6   (15)                TLC5940 BLANK\nPIO0_7   (14)                TLC5940 SIN\nPIO0_8   (11, XTALIN)        ===> AUX2\nPIO0_9   (10, XTALOUT)       Switched light output (for driving a load via a MOSFET)\nPIO0_10  (8,  Open drain)    NC\nPIO0_11  (7,  Open drain)    ===> AUX3 (100K pull-down)\nPIO0_12  (2,  ISP-entry)     OUT / ISP\nPIO0_13  (1)                 CH3 input\n
Text Notes 6300 3200 0    50   ~ 0
Pull-down required because \nPIO0_11 is open drain!
Wire Wire Line
	9100 5300 9700 5300
Wire Wire Line
	8050 5300 8500 5300
Wire Wire Line
	8800 5600 8800 5800
Wire Wire Line
	3600 2550 4150 2550
Wire Wire Line
	6450 2250 6150 2250
Wire Wire Line
	3600 2150 4150 2150
Text Label 6450 2350 2    50   ~ 0
LIGHT
Text Label 3600 2150 0    50   ~ 0
ST
Text Label 3600 2550 0    50   ~ 0
TH
Text Label 6450 2450 2    50   ~ 0
AUX
Text Label 3600 2750 0    50   ~ 0
AUX2
Text Label 6450 2250 2    50   ~ 0
AUX3
$Comp
L Device:R_Pack04 RN1
U 1 1 6020567D
P 4750 4250
F 0 "RN1" V 4333 4250 50  0000 C CNN
F 1 "R_Pack04" V 4424 4250 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 5025 4250 50  0001 C CNN
F 3 "~" H 4750 4250 50  0001 C CNN
	1    4750 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	4000 4050 4550 4050
Wire Wire Line
	1850 3750 2900 3750
Wire Wire Line
	4000 4150 4550 4150
Wire Wire Line
	1850 4750 2900 4750
Wire Wire Line
	4000 4250 4550 4250
Wire Wire Line
	4550 4350 4000 4350
Text Label 5500 4350 2    50   ~ 0
LIGHT
Wire Wire Line
	4950 4350 5500 4350
Text Label 4000 4350 0    50   ~ 0
LIGHT_OUT
Wire Wire Line
	4950 4250 5500 4250
Wire Wire Line
	4950 4150 5500 4150
Wire Wire Line
	4950 4050 5500 4050
Text Label 5500 4050 2    50   ~ 0
AUX
Text Label 5500 4150 2    50   ~ 0
AUX2
Text Label 5500 4250 2    50   ~ 0
AUX3
NoConn ~ 4150 2250
NoConn ~ 4150 2650
Wire Wire Line
	4150 2750 3600 2750
Connection ~ 6150 2250
Wire Wire Line
	6150 2250 5750 2250
Wire Wire Line
	6150 2250 6150 2850
Wire Wire Line
	5750 2350 6450 2350
Wire Wire Line
	5750 2450 6450 2450
NoConn ~ 5750 2550
NoConn ~ 5750 2650
NoConn ~ 5750 2850
NoConn ~ 5750 2750
$Comp
L Device:R_Pack04 RN2
U 1 1 6028CA7F
P 4750 5000
F 0 "RN2" V 4333 5000 50  0000 C CNN
F 1 "R_Pack04" V 4424 5000 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 5025 5000 50  0001 C CNN
F 3 "~" H 4750 5000 50  0001 C CNN
	1    4750 5000
	0    1    1    0   
$EndComp
Text Label 4000 4050 0    50   ~ 0
AUX_IN
Wire Wire Line
	1850 4250 2900 4250
Text Label 2900 4250 2    50   ~ 0
AUX2_IN
Text Label 4000 4150 0    50   ~ 0
AUX2_IN
Text Label 4000 4250 0    50   ~ 0
AUX3_IN
Wire Wire Line
	4000 5000 4550 5000
Wire Wire Line
	4000 5100 4550 5100
NoConn ~ 4550 4900
NoConn ~ 4550 4800
NoConn ~ 4950 4900
NoConn ~ 4950 4800
Wire Wire Line
	5500 5000 4950 5000
Wire Wire Line
	5500 5100 4950 5100
Text Label 4000 5000 0    50   ~ 0
ST_IN
Text Label 4000 5100 0    50   ~ 0
TH_IN
Text Label 5500 5000 2    50   ~ 0
ST
Text Label 5500 5100 2    50   ~ 0
TH
Wire Wire Line
	4850 1650 4850 1700
Text Notes 7650 3900 0    50   ~ 0
We are using PIO0_8 for AUX2.\nNote that on the 5ch-PP-S PIO0_6 is used\nso that in the future it is possible to make\na 9-output Mk4S with 5-channel input.\n\nWhen switching hardware is detected \n(PIO0_14 is GND) then the software will\nuse PIO0_6 for AUX2, otherwise PIO0_8\n\nIn our this cae PIO0_14 must therefore left \nfloating!
$Comp
L MCU_NXP_LPC:LPC832M101FDH20 U2
U 1 1 6072E63F
P 4950 2550
F 0 "U2" H 5450 3250 50  0000 C CNN
F 1 "LPC832M101FDH20" H 4300 3250 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 6000 3300 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC83X.pdf" H 5100 3200 50  0001 L CNN
	1    4950 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 3350 4850 3400
Wire Wire Line
	5050 3350 5050 3400
Wire Wire Line
	5050 3400 4850 3400
Connection ~ 4850 3400
Wire Wire Line
	4850 3400 4850 3450
Wire Wire Line
	5050 1750 5050 1700
Wire Wire Line
	5050 1700 4850 1700
Connection ~ 4850 1700
Wire Wire Line
	4850 1700 4850 1750
$EndSCHEMATC
