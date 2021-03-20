EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "WebUSB Programmer for Light Controller Mk4"
Date "2021-03-15"
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
P 3750 2450
F 0 "D2" V 3789 2333 50  0000 R CNN
F 1 "OK" V 3698 2333 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 3750 2450 50  0001 C CNN
F 3 "2011131907_TUOZHAN-TZ-P2-1204YGCTA1-1-5T" H 3750 2450 50  0001 C CNN
F 4 "C91608" V 3750 2450 50  0001 C CNN "LCSC part number"
	1    3750 2450
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D3
U 1 1 5E6F790B
P 4350 2450
F 0 "D3" V 4389 2332 50  0000 R CNN
F 1 "BUSY" V 4298 2332 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 4350 2450 50  0001 C CNN
F 3 "2009041237_TUOZHAN-TZ-P2-1204-0TA1-1-5T" H 4350 2450 50  0001 C CNN
F 4 "C779805" V 4350 2450 50  0001 C CNN "LCSC part number"
	1    4350 2450
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D4
U 1 1 5E6F7DEF
P 4950 2450
F 0 "D4" V 4989 2333 50  0000 R CNN
F 1 "ERROR" V 4898 2333 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 4950 2450 50  0001 C CNN
F 3 "1905151403_MEIHUA-MHS110KECT" H 4950 2450 50  0001 C CNN
F 4 "C389527" V 4950 2450 50  0001 C CNN "LCSC part number"
	1    4950 2450
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D5
U 1 1 5E6F89A9
P 5500 2450
F 0 "D5" V 5539 2333 50  0000 R CNN
F 1 "MCU POWER" V 5448 2333 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 5500 2450 50  0001 C CNN
F 3 "2009041237_TUOZHAN-TZ-P2-1204WYS2-1-5T" H 5500 2450 50  0001 C CNN
F 4 "C779804" V 5500 2450 50  0001 C CNN "LCSC part number"
	1    5500 2450
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D1
U 1 1 5E6F8DF4
P 9150 2300
F 0 "D1" V 9189 2182 50  0000 R CNN
F 1 "LC POWER" V 9098 2182 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 9150 2300 50  0001 C CNN
F 3 "2009041238_TUOZHAN-TZ-P2-1204BTS2-1-5T" H 9150 2300 50  0001 C CNN
F 4 "C779807" V 9150 2300 50  0001 C CNN "LCSC part number"
	1    9150 2300
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 5E6FD03E
P 9150 1900
F 0 "R2" H 9220 1946 50  0000 L CNN
F 1 "1k8" H 9220 1855 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9080 1900 50  0001 C CNN
F 3 "~" H 9150 1900 50  0001 C CNN
	1    9150 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 5E6FE83D
P 9850 1550
F 0 "J1" H 9822 1432 50  0000 R CNN
F 1 "Light Controller" H 9822 1523 50  0000 R CNN
F 2 "WLA_pinheader:PinHeader_1x06_P2.54mm_Flat" H 9850 1550 50  0001 C CNN
F 3 "~" H 9850 1550 50  0001 C CNN
	1    9850 1550
	-1   0    0    1   
$EndComp
$Comp
L wla-atsamd21:STMPS2141STR U1
U 1 1 5E709E73
P 7700 1650
F 0 "U1" H 7700 2017 50  0000 C CNN
F 1 "STMPS2141STR" H 7700 1926 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 7700 2150 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps22917.pdf" H 7750 950 50  0001 C CNN
	1    7700 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5E710261
P 9150 2550
F 0 "#PWR07" H 9150 2300 50  0001 C CNN
F 1 "GND" H 9155 2377 50  0000 C CNN
F 2 "" H 9150 2550 50  0001 C CNN
F 3 "" H 9150 2550 50  0001 C CNN
	1    9150 2550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5E71085D
P 7700 2050
F 0 "#PWR03" H 7700 1800 50  0001 C CNN
F 1 "GND" H 7705 1877 50  0000 C CNN
F 2 "" H 7700 2050 50  0001 C CNN
F 3 "" H 7700 2050 50  0001 C CNN
	1    7700 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 1950 7700 2050
Wire Wire Line
	9150 2050 9150 2150
Wire Wire Line
	9150 2450 9150 2550
$Comp
L power:GND #PWR02
U 1 1 5E713E19
P 9550 1850
F 0 "#PWR02" H 9550 1600 50  0001 C CNN
F 1 "GND" H 9555 1677 50  0000 C CNN
F 2 "" H 9550 1850 50  0001 C CNN
F 3 "" H 9550 1850 50  0001 C CNN
	1    9550 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9550 1850 9550 1750
Wire Wire Line
	9550 1750 9650 1750
$Comp
L power:GND #PWR015
U 1 1 5E73F918
P 5500 2850
F 0 "#PWR015" H 5500 2600 50  0001 C CNN
F 1 "GND" H 5505 2677 50  0000 C CNN
F 2 "" H 5500 2850 50  0001 C CNN
F 3 "" H 5500 2850 50  0001 C CNN
	1    5500 2850
	1    0    0    -1  
$EndComp
NoConn ~ 1500 3300
$Comp
L power:GND #PWR011
U 1 1 5E74A406
P 1200 3600
F 0 "#PWR011" H 1200 3350 50  0001 C CNN
F 1 "GND" H 1205 3427 50  0000 C CNN
F 2 "" H 1200 3600 50  0001 C CNN
F 3 "" H 1200 3600 50  0001 C CNN
	1    1200 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 3500 1200 3550
$Comp
L power:+5V #PWR08
U 1 1 5E75A54A
P 1850 2750
F 0 "#PWR08" H 1850 2600 50  0001 C CNN
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
L power:+5V #PWR01
U 1 1 5E75CDA0
P 7100 1350
F 0 "#PWR01" H 7100 1200 50  0001 C CNN
F 1 "+5V" H 7115 1523 50  0000 C CNN
F 2 "" H 7100 1350 50  0001 C CNN
F 3 "" H 7100 1350 50  0001 C CNN
	1    7100 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 1350 7100 1550
Wire Wire Line
	7100 1550 7300 1550
Wire Wire Line
	9450 1550 9650 1550
Wire Wire Line
	9450 1450 9650 1450
Wire Wire Line
	9450 1250 9650 1250
Wire Wire Line
	9050 1450 8750 1450
Wire Wire Line
	8750 1550 9050 1550
Text Label 8750 1550 0    50   ~ 0
TX
Text Label 8750 1450 0    50   ~ 0
RX
Wire Wire Line
	8750 1250 9050 1250
Text Label 8750 1250 0    50   ~ 0
ISP
Text Label 8750 1650 0    50   ~ 0
VLIGHT
Text Label 1600 6500 0    50   ~ 0
TX
Text Label 1600 6400 0    50   ~ 0
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
L power:+3V3 #PWR012
U 1 1 5E788A3B
P 5500 1450
F 0 "#PWR012" H 5500 1300 50  0001 C CNN
F 1 "+3V3" H 5515 1623 50  0000 C CNN
F 2 "" H 5500 1450 50  0001 C CNN
F 3 "" H 5500 1450 50  0001 C CNN
	1    5500 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5E7914FA
P 4500 5300
F 0 "C2" H 4615 5346 50  0000 L CNN
F 1 "100n" H 4615 5255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4538 5150 50  0001 C CNN
F 3 "~" H 4500 5300 50  0001 C CNN
	1    4500 5300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5E79198F
P 3500 5500
F 0 "#PWR014" H 3500 5250 50  0001 C CNN
F 1 "GND" H 3505 5327 50  0000 C CNN
F 2 "" H 3500 5500 50  0001 C CNN
F 3 "" H 3500 5500 50  0001 C CNN
	1    3500 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 5450 3500 5500
Text Label 3750 3050 1    50   ~ 0
OK
Text Label 4350 3050 1    50   ~ 0
BUSY
Text Label 4950 3050 1    50   ~ 0
ERROR
Wire Wire Line
	8100 1650 8600 1650
Wire Wire Line
	9150 1750 9150 1650
Connection ~ 9150 1650
Wire Wire Line
	9150 1650 9650 1650
Wire Wire Line
	5350 6300 5950 6300
Wire Wire Line
	1600 6300 2300 6300
Wire Wire Line
	1600 6200 2300 6200
Text Label 1600 6100 0    50   ~ 0
BUSY
Text Label 1600 6300 0    50   ~ 0
ERROR
Text Label 5950 6100 2    50   ~ 0
ISP
NoConn ~ 7300 1750
Text Label 6900 1650 0    50   ~ 0
EN
Wire Wire Line
	7300 1650 6900 1650
Wire Wire Line
	5950 6100 5350 6100
$Comp
L Connector:USB_B_Micro J2
U 1 1 5E6F135C
P 1200 3100
F 0 "J2" H 1257 3567 50  0000 C CNN
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
L Device:R R1
U 1 1 5E7997E9
P 8600 1900
F 0 "R1" H 8670 1946 50  0000 L CNN
F 1 "100" H 8670 1855 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 8530 1900 50  0001 C CNN
F 3 "~" H 8600 1900 50  0001 C CNN
	1    8600 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 1750 8600 1650
Connection ~ 8600 1650
Wire Wire Line
	8600 1650 9150 1650
Text Label 5950 6300 2    50   ~ 0
SHORT
Text Label 8200 2300 0    50   ~ 0
SHORT
Wire Wire Line
	5350 6400 5950 6400
Wire Wire Line
	9450 1350 9650 1350
Wire Wire Line
	9050 1350 8750 1350
Text Label 8750 1350 0    50   ~ 0
CH3
Wire Wire Line
	5350 6200 5950 6200
Text Label 5950 6200 2    50   ~ 0
CH3
$Comp
L Device:R_Pack04 RN1
U 1 1 5FDCE5F9
P 9250 1450
F 0 "RN1" V 8833 1450 50  0000 C CNN
F 1 "1k" V 8924 1450 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 9525 1450 50  0001 C CNN
F 3 "~" H 9250 1450 50  0001 C CNN
	1    9250 1450
	0    1    1    0   
$EndComp
Text Notes 1000 2350 0    50   ~ 0
Important: for mechanical \nstability use a Micro-USB \nconnector that uses \nthrough-hole mountings.
Text Notes 7200 3450 0    50   ~ 0
The IO pin shorts the power \nsupply of the light controller to\nground via 10 Ohms. \nThis discharges the large cap \nin the light controller, otherwise \nsubsequant programming \nattempts may fail as the MCU in \nthe light controller is still residually\npowered (when no firmware is \nflashed yet!)
Text Notes 7400 1200 0    50   ~ 0
High-side power switch,\nactive low
Text Notes 5650 2650 0    50   ~ 0
White
Text Notes 3900 2650 0    50   ~ 0
Green
Text Notes 4500 2650 0    50   ~ 0
Yellow
Text Notes 5100 2650 0    50   ~ 0
Red
Text Notes 9300 2500 0    50   ~ 0
Blue
$Comp
L wla-atsamd21:CH552G U3
U 1 1 604D2755
P 4000 6400
F 0 "U3" H 3825 5811 50  0000 C CNN
F 1 "CH552G" H 3825 5720 50  0000 C CNN
F 2 "Package_SO:SO-16_3.9x9.9mm_P1.27mm" H 2400 6350 50  0001 C CNN
F 3 "" H 2400 6350 50  0001 C CNN
	1    4000 6400
	1    0    0    -1  
$EndComp
Text Label 5950 6400 2    50   ~ 0
OK
Text Label 5950 6600 2    50   ~ 0
USB-DP
Wire Wire Line
	5950 6600 5350 6600
Wire Wire Line
	5950 6700 5350 6700
Text Label 5950 6700 2    50   ~ 0
USB-DM
Wire Wire Line
	3500 5150 3500 5100
Wire Wire Line
	3500 5100 3850 5100
Wire Wire Line
	4150 5700 4150 5100
$Comp
L power:+3V3 #PWR013
U 1 1 6052F571
P 4150 4950
F 0 "#PWR013" H 4150 4800 50  0001 C CNN
F 1 "+3V3" H 4165 5123 50  0000 C CNN
F 2 "" H 4150 4950 50  0001 C CNN
F 3 "" H 4150 4950 50  0001 C CNN
	1    4150 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 4950 4150 5100
Connection ~ 4150 5100
Wire Wire Line
	1600 6500 2300 6500
Wire Wire Line
	1600 6400 2300 6400
NoConn ~ 2300 6700
Text Label 1600 6200 0    50   ~ 0
EN
Wire Wire Line
	1600 6100 2300 6100
NoConn ~ 5350 6500
$Comp
L power:GND #PWR016
U 1 1 605D5465
P 4000 7250
F 0 "#PWR016" H 4000 7000 50  0001 C CNN
F 1 "GND" H 4005 7077 50  0000 C CNN
F 2 "" H 4000 7250 50  0001 C CNN
F 3 "" H 4000 7250 50  0001 C CNN
	1    4000 7250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 7250 4000 6900
Wire Wire Line
	5500 2600 5500 2850
Wire Wire Line
	4950 2600 4950 3050
Wire Wire Line
	4350 2600 4350 3050
Wire Wire Line
	3750 2600 3750 3050
$Comp
L Device:R R6
U 1 1 5E6FD675
P 5500 2050
F 0 "R6" H 5570 2096 50  0000 L CNN
F 1 "1k8" H 5570 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 5430 2050 50  0001 C CNN
F 3 "~" H 5500 2050 50  0001 C CNN
	1    5500 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5E6FC0B8
P 4950 2050
F 0 "R5" H 5020 2096 50  0000 L CNN
F 1 "1k2" H 5020 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 2050 50  0001 C CNN
F 3 "~" H 4950 2050 50  0001 C CNN
	1    4950 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5E6FABAC
P 3750 2050
F 0 "R3" H 3820 2096 50  0000 L CNN
F 1 "2k7" H 3820 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3680 2050 50  0001 C CNN
F 3 "~" H 3750 2050 50  0001 C CNN
	1    3750 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 2200 3750 2300
Wire Wire Line
	4950 2200 4950 2250
Wire Wire Line
	5500 2200 5500 2300
Wire Wire Line
	3750 1700 3750 1900
Wire Wire Line
	4950 1700 4950 1900
Wire Wire Line
	5500 1450 5500 1700
Connection ~ 5500 1700
Wire Wire Line
	5500 1700 5500 1900
Connection ~ 4950 1700
Wire Wire Line
	4950 1700 5500 1700
$Comp
L Device:C C1
U 1 1 605205A6
P 3500 5300
F 0 "C1" H 3615 5346 50  0000 L CNN
F 1 "1u" H 3615 5255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3538 5150 50  0001 C CNN
F 3 "~" H 3500 5300 50  0001 C CNN
	1    3500 5300
	1    0    0    -1  
$EndComp
Text Notes 3800 3900 0    50   ~ 0
The LEDs are conneced at the cathode\nto prevent them from lighting\nup after power up before the software\ncan initialize the ports (pull-up by default)\nError and Busy use the same resistor value, \nand since they never light up together we \ncan share one resistor. Ok (green) is much\nbrighter so needs its own resistor.
$Comp
L Connector:TestPoint TP1
U 1 1 6050C7A0
P 8300 5250
F 0 "TP1" V 8254 5438 50  0000 L CNN
F 1 " " V 8345 5438 50  0000 L CNN
F 2 "WLA_pinheader:TestPoint_THTPad_1.0x1.0mm_Drill0.5mm_nosilk" H 8500 5250 50  0001 C CNN
F 3 "~" H 8500 5250 50  0001 C CNN
	1    8300 5250
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP2
U 1 1 6050F171
P 8300 5450
F 0 "TP2" V 8254 5638 50  0000 L CNN
F 1 " " V 8345 5638 50  0000 L CNN
F 2 "WLA_pinheader:TestPoint_THTPad_1.0x1.0mm_Drill0.5mm_nosilk" H 8500 5450 50  0001 C CNN
F 3 "~" H 8500 5450 50  0001 C CNN
	1    8300 5450
	0    1    1    0   
$EndComp
$Comp
L power:+3V3 #PWR017
U 1 1 605104D2
P 8100 5100
F 0 "#PWR017" H 8100 4950 50  0001 C CNN
F 1 "+3V3" H 8115 5273 50  0000 C CNN
F 2 "" H 8100 5100 50  0001 C CNN
F 3 "" H 8100 5100 50  0001 C CNN
	1    8100 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 5100 8100 5250
Wire Wire Line
	8100 5250 8300 5250
Text Label 7850 5450 0    50   ~ 0
USB-DP
Wire Wire Line
	7850 5450 8300 5450
Text Notes 8700 5450 0    50   ~ 0
Put a 10K resistor accross those \npoints while power on to start \nthe bootloader
Wire Wire Line
	4150 5100 4500 5100
Wire Wire Line
	4500 5100 4500 5150
$Comp
L power:GND #PWR0101
U 1 1 6056195B
P 4500 5500
F 0 "#PWR0101" H 4500 5250 50  0001 C CNN
F 1 "GND" H 4505 5327 50  0000 C CNN
F 2 "" H 4500 5500 50  0001 C CNN
F 3 "" H 4500 5500 50  0001 C CNN
	1    4500 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 5450 4500 5500
Wire Wire Line
	8600 2050 8600 2300
Wire Wire Line
	8200 2300 8600 2300
Wire Wire Line
	3750 1700 4950 1700
Wire Wire Line
	4950 2250 4350 2250
Connection ~ 4950 2250
Wire Wire Line
	4950 2250 4950 2300
Wire Wire Line
	4350 2250 4350 2300
$Comp
L power:+5V #PWR?
U 1 1 605A6E18
P 3850 4950
F 0 "#PWR?" H 3850 4800 50  0001 C CNN
F 1 "+5V" H 3865 5123 50  0000 C CNN
F 2 "" H 3850 4950 50  0001 C CNN
F 3 "" H 3850 4950 50  0001 C CNN
	1    3850 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 4950 3850 5100
Connection ~ 3850 5100
Wire Wire Line
	3850 5100 3850 5700
$EndSCHEMATC
