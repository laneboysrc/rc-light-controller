EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "WebUSB Programmer for Light Controller Mk4"
Date "2022-03-24"
Rev "4"
Comp "LANE Boys RC"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:LED D2
U 1 1 5E6F6F74
P 3500 2850
F 0 "D2" V 3539 2733 50  0000 R CNN
F 1 "OK" V 3448 2733 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 3500 2850 50  0001 C CNN
F 3 "2011131907_TUOZHAN-TZ-P2-1204YGCTA1-1-5T" H 3500 2850 50  0001 C CNN
F 4 "C91608" V 3500 2850 50  0001 C CNN "LCSC part number"
	1    3500 2850
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D3
U 1 1 5E6F790B
P 4100 2850
F 0 "D3" V 4139 2732 50  0000 R CNN
F 1 "BUSY" V 4048 2732 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 4100 2850 50  0001 C CNN
F 3 "2009041237_TUOZHAN-TZ-P2-1204-0TA1-1-5T" H 4100 2850 50  0001 C CNN
F 4 "C779805" V 4100 2850 50  0001 C CNN "LCSC part number"
	1    4100 2850
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D4
U 1 1 5E6F7DEF
P 4700 2850
F 0 "D4" V 4739 2733 50  0000 R CNN
F 1 "ERROR" V 4648 2733 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 4700 2850 50  0001 C CNN
F 3 "1905151403_MEIHUA-MHS110KECT" H 4700 2850 50  0001 C CNN
F 4 "C389527" V 4700 2850 50  0001 C CNN "LCSC part number"
	1    4700 2850
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D5
U 1 1 5E6F89A9
P 5250 2850
F 0 "D5" V 5289 2733 50  0000 R CNN
F 1 "MCU POWER" V 5198 2733 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 5250 2850 50  0001 C CNN
F 3 "2009041237_TUOZHAN-TZ-P2-1204WYS2-1-5T" H 5250 2850 50  0001 C CNN
F 4 "C779804" V 5250 2850 50  0001 C CNN "LCSC part number"
	1    5250 2850
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D1
U 1 1 5E6F8DF4
P 9250 2450
F 0 "D1" V 9289 2332 50  0000 R CNN
F 1 "LC POWER" V 9198 2332 50  0000 R CNN
F 2 "WLA_LED_reverse_mount:LED_1204_side_mout" H 9250 2450 50  0001 C CNN
F 3 "2009041238_TUOZHAN-TZ-P2-1204BTS2-1-5T" H 9250 2450 50  0001 C CNN
F 4 "C779807" V 9250 2450 50  0001 C CNN "LCSC part number"
	1    9250 2450
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 5E6FE83D
P 9950 1700
F 0 "J1" H 9922 1582 50  0000 R CNN
F 1 "Light Controller" H 9922 1673 50  0000 R CNN
F 2 "WLA_pinheader:PinHeader_1x06_P2.54mm_Flat" H 9950 1700 50  0001 C CNN
F 3 "~" H 9950 1700 50  0001 C CNN
	1    9950 1700
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5E710261
P 9250 2700
F 0 "#PWR05" H 9250 2450 50  0001 C CNN
F 1 "GND" H 9255 2527 50  0000 C CNN
F 2 "" H 9250 2700 50  0001 C CNN
F 3 "" H 9250 2700 50  0001 C CNN
	1    9250 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 2200 9250 2300
Wire Wire Line
	9250 2600 9250 2700
$Comp
L power:GND #PWR03
U 1 1 5E713E19
P 9650 2000
F 0 "#PWR03" H 9650 1750 50  0001 C CNN
F 1 "GND" H 9655 1827 50  0000 C CNN
F 2 "" H 9650 2000 50  0001 C CNN
F 3 "" H 9650 2000 50  0001 C CNN
	1    9650 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 2000 9650 1900
Wire Wire Line
	9650 1900 9750 1900
$Comp
L power:GND #PWR07
U 1 1 5E73F918
P 5250 3250
F 0 "#PWR07" H 5250 3000 50  0001 C CNN
F 1 "GND" H 5255 3077 50  0000 C CNN
F 2 "" H 5250 3250 50  0001 C CNN
F 3 "" H 5250 3250 50  0001 C CNN
	1    5250 3250
	1    0    0    -1  
$EndComp
NoConn ~ 1500 3300
$Comp
L power:GND #PWR08
U 1 1 5E74A406
P 1200 3600
F 0 "#PWR08" H 1200 3350 50  0001 C CNN
F 1 "GND" H 1205 3427 50  0000 C CNN
F 2 "" H 1200 3600 50  0001 C CNN
F 3 "" H 1200 3600 50  0001 C CNN
	1    1200 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 3500 1200 3550
$Comp
L power:+5V #PWR06
U 1 1 5E75A54A
P 1850 2750
F 0 "#PWR06" H 1850 2600 50  0001 C CNN
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
Wire Wire Line
	9550 1700 9750 1700
Wire Wire Line
	9550 1600 9750 1600
Wire Wire Line
	9550 1400 9750 1400
Wire Wire Line
	9150 1600 8850 1600
Wire Wire Line
	8850 1700 9150 1700
Text Label 8850 1700 0    50   ~ 0
TX
Text Label 8850 1600 0    50   ~ 0
RX
Wire Wire Line
	8850 1400 9150 1400
Text Label 8850 1400 0    50   ~ 0
ISP
Text Label 8850 1800 0    50   ~ 0
VLIGHT
Text Label 5750 5850 2    50   ~ 0
TX
Text Label 5750 5750 2    50   ~ 0
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
L power:+3V3 #PWR02
U 1 1 5E788A3B
P 5250 1850
F 0 "#PWR02" H 5250 1700 50  0001 C CNN
F 1 "+3V3" H 5265 2023 50  0000 C CNN
F 2 "" H 5250 1850 50  0001 C CNN
F 3 "" H 5250 1850 50  0001 C CNN
	1    5250 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5E7914FA
P 4300 4950
F 0 "C2" H 4415 4996 50  0000 L CNN
F 1 "100n" H 4415 4905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4338 4800 50  0001 C CNN
F 3 "~" H 4300 4950 50  0001 C CNN
	1    4300 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5E79198F
P 3300 5150
F 0 "#PWR012" H 3300 4900 50  0001 C CNN
F 1 "GND" H 3305 4977 50  0000 C CNN
F 2 "" H 3300 5150 50  0001 C CNN
F 3 "" H 3300 5150 50  0001 C CNN
	1    3300 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 5100 3300 5150
Wire Wire Line
	9250 1900 9250 1800
Connection ~ 9250 1800
Wire Wire Line
	9250 1800 9750 1800
Wire Wire Line
	5150 5950 5750 5950
Wire Wire Line
	1400 5950 2100 5950
Wire Wire Line
	1400 5850 2100 5850
Text Label 1400 5750 0    50   ~ 0
~BUSY
Text Label 1400 5950 0    50   ~ 0
~ERROR
Text Label 1400 6150 0    50   ~ 0
ISP
Wire Wire Line
	5750 5750 5150 5750
$Comp
L Connector:USB_B_Micro J2
U 1 1 5E6F135C
P 1200 3100
F 0 "J2" H 1257 3567 50  0000 C CNN
F 1 "USB_B_Micro" H 1257 3476 50  0000 C CNN
F 2 "WLA_USB:Mirco_USB_Type_B_LCSC_C40943" H 1350 3050 50  0001 C CNN
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
Text Label 5750 5950 2    50   ~ 0
INB
Wire Wire Line
	5150 6050 5750 6050
Wire Wire Line
	9550 1500 9750 1500
Wire Wire Line
	9150 1500 8850 1500
Text Label 8850 1500 0    50   ~ 0
CH3
Wire Wire Line
	5150 5850 5750 5850
Text Label 1400 6050 0    50   ~ 0
CH3
$Comp
L Device:R_Pack04 RN1
U 1 1 5FDCE5F9
P 9350 1600
F 0 "RN1" V 8933 1600 50  0000 C CNN
F 1 "1k" V 9024 1600 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 9625 1600 50  0001 C CNN
F 3 "~" H 9350 1600 50  0001 C CNN
	1    9350 1600
	0    1    1    0   
$EndComp
Text Notes 1000 2350 0    50   ~ 0
Important: for mechanical \nstability use a Micro-USB \nconnector that uses \nthrough-hole mountings.
Text Notes 5400 3050 0    50   ~ 0
White
Text Notes 3650 3050 0    50   ~ 0
Green
Text Notes 4250 3050 0    50   ~ 0
Yellow
Text Notes 4850 3050 0    50   ~ 0
Red
Text Notes 9400 2650 0    50   ~ 0
Blue
$Comp
L wla-project-specific-components:CH552G U2
U 1 1 604D2755
P 3800 6050
F 0 "U2" H 2250 6700 50  0000 C CNN
F 1 "CH552G or CH551G" H 4650 5600 50  0000 C CNN
F 2 "Package_SO:SO-16_3.9x9.9mm_P1.27mm" H 2200 6000 50  0001 C CNN
F 3 "" H 2200 6000 50  0001 C CNN
F 4 "C111292" H 3800 6050 50  0001 C CNN "LCSC part number"
	1    3800 6050
	1    0    0    -1  
$EndComp
Text Label 5750 6050 2    50   ~ 0
~OK
Text Label 5750 6250 2    50   ~ 0
USB-DP
Wire Wire Line
	5750 6250 5150 6250
Wire Wire Line
	5750 6350 5150 6350
Text Label 5750 6350 2    50   ~ 0
USB-DM
Wire Wire Line
	3300 4800 3300 4750
Wire Wire Line
	3300 4750 3650 4750
Wire Wire Line
	3950 5350 3950 4750
$Comp
L power:+3V3 #PWR010
U 1 1 6052F571
P 3950 4600
F 0 "#PWR010" H 3950 4450 50  0001 C CNN
F 1 "+3V3" H 3965 4773 50  0000 C CNN
F 2 "" H 3950 4600 50  0001 C CNN
F 3 "" H 3950 4600 50  0001 C CNN
	1    3950 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 4600 3950 4750
Connection ~ 3950 4750
Wire Wire Line
	1400 6150 2100 6150
Wire Wire Line
	1400 6050 2100 6050
NoConn ~ 2100 6350
Text Label 1400 5850 0    50   ~ 0
INA
Wire Wire Line
	1400 5750 2100 5750
NoConn ~ 5150 6150
$Comp
L power:GND #PWR014
U 1 1 605D5465
P 3800 6900
F 0 "#PWR014" H 3800 6650 50  0001 C CNN
F 1 "GND" H 3805 6727 50  0000 C CNN
F 2 "" H 3800 6900 50  0001 C CNN
F 3 "" H 3800 6900 50  0001 C CNN
	1    3800 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 6900 3800 6550
Wire Wire Line
	5250 3000 5250 3250
$Comp
L Device:R R4
U 1 1 5E6FD675
P 5250 2450
F 0 "R4" H 5320 2496 50  0000 L CNN
F 1 "1k8" H 5320 2405 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 5180 2450 50  0001 C CNN
F 3 "~" H 5250 2450 50  0001 C CNN
	1    5250 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 2600 5250 2700
Wire Wire Line
	5250 1850 5250 2100
Connection ~ 5250 2100
Wire Wire Line
	5250 2100 5250 2300
Connection ~ 4700 2100
Wire Wire Line
	4700 2100 5250 2100
$Comp
L Device:C C1
U 1 1 605205A6
P 3300 4950
F 0 "C1" H 3415 4996 50  0000 L CNN
F 1 "10u" H 3415 4905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3338 4800 50  0001 C CNN
F 3 "~" H 3300 4950 50  0001 C CNN
	1    3300 4950
	1    0    0    -1  
$EndComp
Text Notes 3300 1800 0    50   ~ 0
The LEDs are conneced at the cathode\nto prevent them from lighting\nup after power up before the software\ncan initialize the ports (pull-up by default)\nError and Busy use the same resistor value, \nand since they never light up together we \ncan share one resistor. \nOK (green) uses a different technology \nso needs its own resistor.
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
L power:+3V3 #PWR011
U 1 1 605104D2
P 8100 5100
F 0 "#PWR011" H 8100 4950 50  0001 C CNN
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
	3950 4750 4300 4750
Wire Wire Line
	4300 4750 4300 4800
$Comp
L power:GND #PWR013
U 1 1 6056195B
P 4300 5150
F 0 "#PWR013" H 4300 4900 50  0001 C CNN
F 1 "GND" H 4305 4977 50  0000 C CNN
F 2 "" H 4300 5150 50  0001 C CNN
F 3 "" H 4300 5150 50  0001 C CNN
	1    4300 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 5100 4300 5150
$Comp
L power:+5V #PWR09
U 1 1 605A6E18
P 3650 4600
F 0 "#PWR09" H 3650 4450 50  0001 C CNN
F 1 "+5V" H 3665 4773 50  0000 C CNN
F 2 "" H 3650 4600 50  0001 C CNN
F 3 "" H 3650 4600 50  0001 C CNN
	1    3650 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 4600 3650 4750
Connection ~ 3650 4750
Wire Wire Line
	3650 4750 3650 5350
Wire Wire Line
	4100 2650 4100 2700
Wire Wire Line
	4700 2650 4700 2700
Connection ~ 4700 2650
Wire Wire Line
	4700 2650 4100 2650
Wire Wire Line
	3500 2100 4700 2100
Wire Wire Line
	4700 2100 4700 2300
Wire Wire Line
	4700 2600 4700 2650
Wire Wire Line
	3500 2100 3500 2300
Wire Wire Line
	3500 2600 3500 2700
$Comp
L Device:R R2
U 1 1 5E6FABAC
P 3500 2450
F 0 "R2" H 3570 2496 50  0000 L CNN
F 1 "180" H 3570 2405 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3430 2450 50  0001 C CNN
F 3 "~" H 3500 2450 50  0001 C CNN
	1    3500 2450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5E6FC0B8
P 4700 2450
F 0 "R3" H 4770 2496 50  0000 L CNN
F 1 "1k2" H 4770 2405 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4630 2450 50  0001 C CNN
F 3 "~" H 4700 2450 50  0001 C CNN
	1    4700 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 3000 3500 3450
Wire Wire Line
	4100 3000 4100 3450
Wire Wire Line
	4700 3000 4700 3450
Text Label 4700 3450 1    50   ~ 0
~ERROR
Text Label 4100 3450 1    50   ~ 0
~BUSY
Text Label 3500 3450 1    50   ~ 0
~OK
$Comp
L wla-project-specific-components:TC118S U1
U 1 1 6180984C
P 7300 1700
F 0 "U1" H 7600 2000 50  0000 C CNN
F 1 "TC118S" H 7600 1350 50  0000 C CNN
F 2 "Package_SO:SOP-8_3.9x4.9mm_P1.27mm" H 7300 1700 50  0001 C CNN
F 3 "" H 7300 1700 50  0001 C CNN
F 4 "C88308" H 7300 1700 50  0001 C CNN "LCSC part number"
	1    7300 1700
	1    0    0    -1  
$EndComp
NoConn ~ 7750 1600
$Comp
L power:GND #PWR04
U 1 1 6180D4B5
P 7200 2300
F 0 "#PWR04" H 7200 2050 50  0001 C CNN
F 1 "GND" H 7205 2127 50  0000 C CNN
F 2 "" H 7200 2300 50  0001 C CNN
F 3 "" H 7200 2300 50  0001 C CNN
	1    7200 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 2100 7200 2200
Wire Wire Line
	7350 2100 7350 2200
Wire Wire Line
	7350 2200 7200 2200
Connection ~ 7200 2200
Wire Wire Line
	7200 2200 7200 2300
$Comp
L power:+5V #PWR01
U 1 1 61812E22
P 7300 1200
F 0 "#PWR01" H 7300 1050 50  0001 C CNN
F 1 "+5V" H 7315 1373 50  0000 C CNN
F 2 "" H 7300 1200 50  0001 C CNN
F 3 "" H 7300 1200 50  0001 C CNN
	1    7300 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 1200 7300 1350
Wire Wire Line
	6850 1600 6400 1600
Wire Wire Line
	6400 1800 6850 1800
Text Label 6400 1600 0    50   ~ 0
INA
Text Label 6400 1800 0    50   ~ 0
INB
Text Notes 7500 3100 0    50   ~ 0
Motor driver IC used\nas power switch for \nthe light controller\n\nINA INB OUTA OUTB \nL    L  Hi-Z  Hi-Z\nH    L   H    L\nL    H   L    H\nH    H   L    L
$Comp
L Device:R R1
U 1 1 5E6FD03E
P 9250 2050
F 0 "R1" H 9320 2096 50  0000 L CNN
F 1 "1k8" H 9320 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9180 2050 50  0001 C CNN
F 3 "~" H 9250 2050 50  0001 C CNN
	1    9250 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 1800 9250 1800
Text Notes 1700 5050 0    50   ~ 0
C1 needs to be rather large, \nbecause when the Light Controller \nis powered up the 47uF needs to \nbe charged, causing a voltage \ndrop that could crash the WebUSB \nprogrammer -- especially when \nusing longer USB cables.
$EndSCHEMATC
