EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "WebUSB Programmer for Light Controller Mk4"
Date "2020-03-16"
Rev "1"
Comp "LANE Boys RC"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:LED D2
U 1 1 5E6F6F74
P 3150 2200
F 0 "D2" V 3189 2083 50  0000 R CNN
F 1 "OK" V 3098 2083 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_reverse_mout" H 3150 2200 50  0001 C CNN
F 3 "2011131907_TUOZHAN-TZ-P2-1204YGCTA1-1-5T" H 3150 2200 50  0001 C CNN
F 4 "C91608" V 3150 2200 50  0001 C CNN "LCSC part number"
	1    3150 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D3
U 1 1 5E6F790B
P 3750 2200
F 0 "D3" V 3789 2082 50  0000 R CNN
F 1 "BUSY" V 3698 2082 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_reverse_mout" H 3750 2200 50  0001 C CNN
F 3 "2009041237_TUOZHAN-TZ-P2-1204-0TA1-1-5T" H 3750 2200 50  0001 C CNN
F 4 "C779805" V 3750 2200 50  0001 C CNN "LCSC part number"
	1    3750 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D4
U 1 1 5E6F7DEF
P 4350 2200
F 0 "D4" V 4389 2083 50  0000 R CNN
F 1 "ERROR" V 4298 2083 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_reverse_mout" H 4350 2200 50  0001 C CNN
F 3 "1905151403_MEIHUA-MHS110KECT" H 4350 2200 50  0001 C CNN
F 4 "C389527" V 4350 2200 50  0001 C CNN "LCSC part number"
	1    4350 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D1
U 1 1 5E6F89A9
P 4900 2200
F 0 "D1" V 4939 2083 50  0000 R CNN
F 1 "MCU POWER" V 4848 2083 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_reverse_mout" H 4900 2200 50  0001 C CNN
F 3 "2009041237_TUOZHAN-TZ-P2-1204WYS2-1-5T" H 4900 2200 50  0001 C CNN
F 4 "C779804" V 4900 2200 50  0001 C CNN "LCSC part number"
	1    4900 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D5
U 1 1 5E6F8DF4
P 9600 3400
F 0 "D5" V 9639 3282 50  0000 R CNN
F 1 "LC POWER" V 9548 3282 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_reverse_mout" H 9600 3400 50  0001 C CNN
F 3 "2009041238_TUOZHAN-TZ-P2-1204BTS2-1-5T" H 9600 3400 50  0001 C CNN
F 4 "C779807" V 9600 3400 50  0001 C CNN "LCSC part number"
	1    9600 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R7
U 1 1 5E6FD03E
P 9600 3000
F 0 "R7" H 9670 3046 50  0000 L CNN
F 1 "1k8" H 9670 2955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9530 3000 50  0001 C CNN
F 3 "~" H 9600 3000 50  0001 C CNN
	1    9600 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J2
U 1 1 5E6FE83D
P 10300 2650
F 0 "J2" H 10272 2532 50  0000 R CNN
F 1 "Light Controller" H 10272 2623 50  0000 R CNN
F 2 "WLA_pinheader:PinHeader_1x06_P2.54mm_Flat" H 10300 2650 50  0001 C CNN
F 3 "~" H 10300 2650 50  0001 C CNN
	1    10300 2650
	-1   0    0    1   
$EndComp
$Comp
L wla-atsamd21:STMPS2141STR U3
U 1 1 5E709E73
P 8150 2750
F 0 "U3" H 8150 3117 50  0000 C CNN
F 1 "STMPS2141STR" H 8150 3026 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 8150 3250 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps22917.pdf" H 8200 2050 50  0001 C CNN
	1    8150 2750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 5E710261
P 9600 3650
F 0 "#PWR022" H 9600 3400 50  0001 C CNN
F 1 "GND" H 9605 3477 50  0000 C CNN
F 2 "" H 9600 3650 50  0001 C CNN
F 3 "" H 9600 3650 50  0001 C CNN
	1    9600 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR019
U 1 1 5E71085D
P 8150 3150
F 0 "#PWR019" H 8150 2900 50  0001 C CNN
F 1 "GND" H 8155 2977 50  0000 C CNN
F 2 "" H 8150 3150 50  0001 C CNN
F 3 "" H 8150 3150 50  0001 C CNN
	1    8150 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 3050 8150 3150
Wire Wire Line
	9600 3150 9600 3250
Wire Wire Line
	9600 3550 9600 3650
$Comp
L power:GND #PWR023
U 1 1 5E713E19
P 10000 2950
F 0 "#PWR023" H 10000 2700 50  0001 C CNN
F 1 "GND" H 10005 2777 50  0000 C CNN
F 2 "" H 10000 2950 50  0001 C CNN
F 3 "" H 10000 2950 50  0001 C CNN
	1    10000 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 2950 10000 2850
Wire Wire Line
	10000 2850 10100 2850
$Comp
L power:GND #PWR09
U 1 1 5E73F918
P 4900 2600
F 0 "#PWR09" H 4900 2350 50  0001 C CNN
F 1 "GND" H 4905 2427 50  0000 C CNN
F 2 "" H 4900 2600 50  0001 C CNN
F 3 "" H 4900 2600 50  0001 C CNN
	1    4900 2600
	1    0    0    -1  
$EndComp
NoConn ~ 1500 3300
$Comp
L power:GND #PWR01
U 1 1 5E74A406
P 1200 3600
F 0 "#PWR01" H 1200 3350 50  0001 C CNN
F 1 "GND" H 1205 3427 50  0000 C CNN
F 2 "" H 1200 3600 50  0001 C CNN
F 3 "" H 1200 3600 50  0001 C CNN
	1    1200 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 3500 1200 3550
$Comp
L power:+5V #PWR04
U 1 1 5E75A54A
P 1850 2750
F 0 "#PWR04" H 1850 2600 50  0001 C CNN
F 1 "+5V" H 1865 2923 50  0000 C CNN
F 2 "" H 1850 2750 50  0001 C CNN
F 3 "" H 1850 2750 50  0001 C CNN
	1    1850 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 2750 1850 2900
Wire Wire Line
	1850 2900 1500 2900
$Comp
L power:+5V #PWR018
U 1 1 5E75CDA0
P 7550 2450
F 0 "#PWR018" H 7550 2300 50  0001 C CNN
F 1 "+5V" H 7565 2623 50  0000 C CNN
F 2 "" H 7550 2450 50  0001 C CNN
F 3 "" H 7550 2450 50  0001 C CNN
	1    7550 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 2450 7550 2650
Wire Wire Line
	7550 2650 7750 2650
Wire Wire Line
	9900 2650 10100 2650
Wire Wire Line
	9900 2550 10100 2550
Wire Wire Line
	9900 2350 10100 2350
Wire Wire Line
	9500 2550 9200 2550
Wire Wire Line
	9200 2650 9500 2650
Text Label 9200 2650 0    50   ~ 0
TX
Text Label 9200 2550 0    50   ~ 0
RX
Wire Wire Line
	9200 2350 9500 2350
Text Label 9200 2350 0    50   ~ 0
ISP
Text Label 9200 2750 0    50   ~ 0
VLIGHT
Text Label 1800 5800 0    50   ~ 0
TX
Text Label 1800 5700 0    50   ~ 0
RX
Wire Wire Line
	1500 3100 2300 3100
Wire Wire Line
	2300 3200 1500 3200
Text Label 2300 3100 2    50   ~ 0
USB-DP
Text Label 2300 3200 2    50   ~ 0
USB-DM
$Comp
L power:+3V3 #PWR08
U 1 1 5E788A3B
P 4900 1200
F 0 "#PWR08" H 4900 1050 50  0001 C CNN
F 1 "+3V3" H 4915 1373 50  0000 C CNN
F 2 "" H 4900 1200 50  0001 C CNN
F 3 "" H 4900 1200 50  0001 C CNN
	1    4900 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 5E78EC25
P 4650 4600
F 0 "C5" H 4765 4646 50  0000 L CNN
F 1 "100n" H 4765 4555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4688 4450 50  0001 C CNN
F 3 "~" H 4650 4600 50  0001 C CNN
	1    4650 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5E78FA6F
P 4650 4850
F 0 "#PWR012" H 4650 4600 50  0001 C CNN
F 1 "GND" H 4655 4677 50  0000 C CNN
F 2 "" H 4650 4850 50  0001 C CNN
F 3 "" H 4650 4850 50  0001 C CNN
	1    4650 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 4750 4650 4850
$Comp
L Device:C C4
U 1 1 5E7914FA
P 3700 4600
F 0 "C4" H 3815 4646 50  0000 L CNN
F 1 "100n" H 3815 4555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3738 4450 50  0001 C CNN
F 3 "~" H 3700 4600 50  0001 C CNN
	1    3700 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5E79198F
P 3700 4800
F 0 "#PWR010" H 3700 4550 50  0001 C CNN
F 1 "GND" H 3705 4627 50  0000 C CNN
F 2 "" H 3700 4800 50  0001 C CNN
F 3 "" H 3700 4800 50  0001 C CNN
	1    3700 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 4750 3700 4800
Text Label 3150 2800 1    50   ~ 0
OK
Text Label 3750 2800 1    50   ~ 0
BUSY
Text Label 4350 2800 1    50   ~ 0
ERROR
Wire Wire Line
	8550 2750 9050 2750
Wire Wire Line
	9600 2850 9600 2750
Connection ~ 9600 2750
Wire Wire Line
	9600 2750 10100 2750
Wire Wire Line
	5550 5600 6150 5600
Wire Wire Line
	1800 5600 2500 5600
Wire Wire Line
	1800 5500 2500 5500
Text Label 1800 5400 0    50   ~ 0
BUSY
Text Label 1800 5600 0    50   ~ 0
ERROR
Text Label 6150 5400 2    50   ~ 0
ISP
NoConn ~ 7750 2850
Text Label 7350 2750 0    50   ~ 0
EN
Wire Wire Line
	7750 2750 7350 2750
Wire Wire Line
	6150 5400 5550 5400
$Comp
L Connector:USB_B_Micro J1
U 1 1 5E6F135C
P 1200 3100
F 0 "J1" H 1257 3567 50  0000 C CNN
F 1 "USB_B_Micro" H 1257 3476 50  0000 C CNN
F 2 "USB:Mirco_USB_Type_B_LCSC_C40943" H 1350 3050 50  0001 C CNN
F 3 "~" H 1350 3050 50  0001 C CNN
F 4 "C40943" H 1200 3100 50  0001 C CNN "LCSC part number"
	1    1200 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 3500 1100 3550
Wire Wire Line
	1100 3550 1200 3550
Connection ~ 1200 3550
Wire Wire Line
	1200 3550 1200 3600
$Comp
L Transistor_FET:BSS138 Q1
U 1 1 5E77A5F9
P 8950 3400
F 0 "Q1" H 9154 3446 50  0000 L CNN
F 1 "PMV30UN" H 9154 3355 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9150 3325 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/BS/BSS138.pdf" H 8950 3400 50  0001 L CNN
	1    8950 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5E7943C7
P 9050 3650
F 0 "#PWR020" H 9050 3400 50  0001 C CNN
F 1 "GND" H 9055 3477 50  0000 C CNN
F 2 "" H 9050 3650 50  0001 C CNN
F 3 "" H 9050 3650 50  0001 C CNN
	1    9050 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 3600 9050 3650
$Comp
L Device:R R6
U 1 1 5E7997E9
P 9050 3000
F 0 "R6" H 9120 3046 50  0000 L CNN
F 1 "10" H 9120 2955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 8980 3000 50  0001 C CNN
F 3 "~" H 9050 3000 50  0001 C CNN
	1    9050 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 3150 9050 3200
Wire Wire Line
	9050 2850 9050 2750
Connection ~ 9050 2750
Wire Wire Line
	9050 2750 9600 2750
Text Label 6150 5600 2    50   ~ 0
SHORT
Text Label 8400 3400 0    50   ~ 0
SHORT
Wire Wire Line
	8400 3400 8750 3400
Wire Wire Line
	5550 5700 6150 5700
Wire Wire Line
	9900 2450 10100 2450
Wire Wire Line
	9500 2450 9200 2450
Text Label 9200 2450 0    50   ~ 0
CH3
Wire Wire Line
	5550 5500 6150 5500
Text Label 6150 5500 2    50   ~ 0
CH3
$Comp
L Device:R_Pack04 RN1
U 1 1 5FDCE5F9
P 9700 2550
F 0 "RN1" V 9283 2550 50  0000 C CNN
F 1 "1k" V 9374 2550 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 9975 2550 50  0001 C CNN
F 3 "~" H 9700 2550 50  0001 C CNN
	1    9700 2550
	0    1    1    0   
$EndComp
Text Notes 1000 2350 0    50   ~ 0
Important: for mechanical \nstability use a Micro-USB \nconnector that uses \nthrough-hole mountings.
Text Notes 7650 4550 0    50   ~ 0
The transistor shorts the power \nsupply of the light controller to\nground via 10 Ohms. \nThis discharges the large cap \nin the light controller, otherwise \nsubsequant programming \nattempts may fail as the MCU in \nthe light controller is still residually\npowered (when no firmware is \nflashed yet!)
Text Notes 7850 2300 0    50   ~ 0
High-side power switch,\nactive low
Text Notes 5050 2400 0    50   ~ 0
White
Text Notes 3300 2400 0    50   ~ 0
Green
Text Notes 3900 2400 0    50   ~ 0
Yellow
Text Notes 4500 2400 0    50   ~ 0
Red
Text Notes 9750 3600 0    50   ~ 0
Blue
$Comp
L wla-atsamd21:CH552G U1
U 1 1 604D2755
P 4200 5700
F 0 "U1" H 4025 5111 50  0000 C CNN
F 1 "CH552G" H 4025 5020 50  0000 C CNN
F 2 "Package_SO:SO-16_3.9x9.9mm_P1.27mm" H 2600 5650 50  0001 C CNN
F 3 "" H 2600 5650 50  0001 C CNN
	1    4200 5700
	1    0    0    -1  
$EndComp
Text Label 6150 5700 2    50   ~ 0
OK
Text Label 6150 5900 2    50   ~ 0
USB-DP
Wire Wire Line
	6150 5900 5550 5900
Wire Wire Line
	6150 6000 5550 6000
Text Label 6150 6000 2    50   ~ 0
USB-DM
$Comp
L power:+5V #PWR0101
U 1 1 6051237F
P 4050 4250
F 0 "#PWR0101" H 4050 4100 50  0001 C CNN
F 1 "+5V" H 4065 4423 50  0000 C CNN
F 2 "" H 4050 4250 50  0001 C CNN
F 3 "" H 4050 4250 50  0001 C CNN
	1    4050 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 4250 4050 4400
Wire Wire Line
	3700 4450 3700 4400
Wire Wire Line
	3700 4400 4050 4400
Connection ~ 4050 4400
Wire Wire Line
	4050 4400 4050 5000
Wire Wire Line
	4350 5000 4350 4400
Wire Wire Line
	4350 4400 4650 4400
$Comp
L power:+3V3 #PWR0102
U 1 1 6052F571
P 4350 4250
F 0 "#PWR0102" H 4350 4100 50  0001 C CNN
F 1 "+3V3" H 4365 4423 50  0000 C CNN
F 2 "" H 4350 4250 50  0001 C CNN
F 3 "" H 4350 4250 50  0001 C CNN
	1    4350 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 4250 4350 4400
Connection ~ 4350 4400
Wire Wire Line
	4650 4400 4650 4450
Wire Wire Line
	1800 5800 2500 5800
Wire Wire Line
	1800 5700 2500 5700
NoConn ~ 2500 6000
Text Label 1800 5500 0    50   ~ 0
EN
Wire Wire Line
	1800 5400 2500 5400
NoConn ~ 5550 5800
$Comp
L power:GND #PWR0103
U 1 1 605D5465
P 4200 6550
F 0 "#PWR0103" H 4200 6300 50  0001 C CNN
F 1 "GND" H 4205 6377 50  0000 C CNN
F 2 "" H 4200 6550 50  0001 C CNN
F 3 "" H 4200 6550 50  0001 C CNN
	1    4200 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 6550 4200 6200
Wire Wire Line
	4900 2350 4900 2600
Wire Wire Line
	4350 2350 4350 2800
Wire Wire Line
	3750 2350 3750 2800
Wire Wire Line
	3150 2350 3150 2800
$Comp
L Device:R R1
U 1 1 5E6FD675
P 4900 1800
F 0 "R1" H 4970 1846 50  0000 L CNN
F 1 "1k8" H 4970 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4830 1800 50  0001 C CNN
F 3 "~" H 4900 1800 50  0001 C CNN
	1    4900 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5E6FC0B8
P 4350 1800
F 0 "R4" H 4420 1846 50  0000 L CNN
F 1 "1k2" H 4420 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4280 1800 50  0001 C CNN
F 3 "~" H 4350 1800 50  0001 C CNN
	1    4350 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5E6FBCF2
P 3750 1800
F 0 "R3" H 3820 1846 50  0000 L CNN
F 1 "1k2" H 3820 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3680 1800 50  0001 C CNN
F 3 "~" H 3750 1800 50  0001 C CNN
	1    3750 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5E6FABAC
P 3150 1800
F 0 "R2" H 3220 1846 50  0000 L CNN
F 1 "2k7" H 3220 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3080 1800 50  0001 C CNN
F 3 "~" H 3150 1800 50  0001 C CNN
	1    3150 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1950 3150 2050
Wire Wire Line
	3750 1950 3750 2050
Wire Wire Line
	4350 1950 4350 2050
Wire Wire Line
	4900 1950 4900 2050
Wire Wire Line
	3150 1450 3150 1650
Wire Wire Line
	3750 1450 3750 1650
Wire Wire Line
	4350 1450 4350 1650
Wire Wire Line
	4900 1200 4900 1450
Wire Wire Line
	3150 1450 3750 1450
Connection ~ 4900 1450
Wire Wire Line
	4900 1450 4900 1650
Connection ~ 3750 1450
Wire Wire Line
	3750 1450 4350 1450
Connection ~ 4350 1450
Wire Wire Line
	4350 1450 4900 1450
$EndSCHEMATC
