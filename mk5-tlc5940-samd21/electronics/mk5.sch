EESchema Schematic File Version 2
LIBS:bat54c
LIBS:power
LIBS:device
LIBS:switches
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:texas
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:diode
LIBS:ESD_Protection
LIBS:mk5-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L TLC5940PWP U2
U 1 1 59C45A36
P 9250 3650
F 0 "U2" H 8750 4525 50  0000 L CNN
F 1 "TLC5940PWP" H 9300 4525 50  0000 L CNN
F 2 "Housings_SSOP_modified:HTSSOP-28_4.4x9.7mm_Pitch0.65mm_ThermalPad_paste" H 9275 2675 50  0001 L CNN
F 3 "" H 8850 4350 50  0001 C CNN
	1    9250 3650
	1    0    0    -1  
$EndComp
$Comp
L USB_OTG J2
U 1 1 59C45BE8
P 1250 5150
F 0 "J2" H 1050 5600 50  0000 L CNN
F 1 "USB" H 1050 5500 50  0000 L CNN
F 2 "USB:Mirco_USB_Type_B_eBay_AliExpress" H 1400 5100 50  0001 C CNN
F 3 "" H 1400 5100 50  0001 C CNN
	1    1250 5150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR15
U 1 1 59C45D35
P 3750 4250
F 0 "#PWR15" H 3750 4000 50  0001 C CNN
F 1 "GND" H 3750 4100 50  0000 C CNN
F 2 "" H 3750 4250 50  0001 C CNN
F 3 "" H 3750 4250 50  0001 C CNN
	1    3750 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR25
U 1 1 59C45D7F
P 9250 5000
F 0 "#PWR25" H 9250 4750 50  0001 C CNN
F 1 "GND" H 9250 4850 50  0000 C CNN
F 2 "" H 9250 5000 50  0001 C CNN
F 3 "" H 9250 5000 50  0001 C CNN
	1    9250 5000
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR24
U 1 1 59C45E41
P 9250 2350
F 0 "#PWR24" H 9250 2200 50  0001 C CNN
F 1 "+3V3" H 9250 2490 50  0000 C CNN
F 2 "" H 9250 2350 50  0001 C CNN
F 3 "" H 9250 2350 50  0001 C CNN
	1    9250 2350
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR13
U 1 1 59C45E7A
P 3700 1950
F 0 "#PWR13" H 3700 1800 50  0001 C CNN
F 1 "+3V3" H 3700 2090 50  0000 C CNN
F 2 "" H 3700 1950 50  0001 C CNN
F 3 "" H 3700 1950 50  0001 C CNN
	1    3700 1950
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW1
U 1 1 59C45F57
P 1100 3700
F 0 "SW1" H 1150 3800 50  0000 L CNN
F 1 "BTN" H 1100 3640 50  0000 C CNN
F 2 "" H 1100 3900 50  0001 C CNN
F 3 "" H 1100 3900 50  0001 C CNN
	1    1100 3700
	0    1    1    0   
$EndComp
$Comp
L TEST TP1
U 1 1 59C46169
P 5850 5450
F 0 "TP1" V 5850 5650 50  0000 L BNN
F 1 "SWCLK" V 5900 5650 50  0000 L CNN
F 2 "SMD-pads:Pin_Header_Straight_1x01_Pitch2.54mm_no_3D_model" H 5850 5450 50  0001 C CNN
F 3 "" H 5850 5450 50  0001 C CNN
	1    5850 5450
	0    1    1    0   
$EndComp
$Comp
L TEST TP2
U 1 1 59C46225
P 5850 5700
F 0 "TP2" V 5850 5900 50  0000 L BNN
F 1 "SWDIO" V 5900 5900 50  0000 L CNN
F 2 "SMD-pads:Pin_Header_Straight_1x01_Pitch2.54mm_no_3D_model" H 5850 5700 50  0001 C CNN
F 3 "" H 5850 5700 50  0001 C CNN
	1    5850 5700
	0    1    1    0   
$EndComp
Text Label 7450 3750 2    60   ~ 0
DP
Text Label 7450 3650 2    60   ~ 0
DM
$Comp
L GND #PWR26
U 1 1 59C46A64
P 9550 1750
F 0 "#PWR26" H 9550 1500 50  0001 C CNN
F 1 "GND" H 9550 1600 50  0000 C CNN
F 2 "" H 9550 1750 50  0001 C CNN
F 3 "" H 9550 1750 50  0001 C CNN
	1    9550 1750
	1    0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 59C46F6E
P 7800 3300
F 0 "R7" V 7880 3300 50  0000 C CNN
F 1 "2K 1%" V 7700 3300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7730 3300 50  0001 C CNN
F 3 "" H 7800 3300 50  0001 C CNN
	1    7800 3300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR21
U 1 1 59C4705C
P 7800 3850
F 0 "#PWR21" H 7800 3600 50  0001 C CNN
F 1 "GND" H 7800 3700 50  0000 C CNN
F 2 "" H 7800 3850 50  0001 C CNN
F 3 "" H 7800 3850 50  0001 C CNN
	1    7800 3850
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 59C47154
P 9500 2450
F 0 "C3" H 9525 2550 50  0000 L CNN
F 1 "100n" H 9525 2350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 9538 2300 50  0001 C CNN
F 3 "" H 9500 2450 50  0001 C CNN
	1    9500 2450
	0    1    1    0   
$EndComp
$Comp
L C C4
U 1 1 59C471E5
P 3700 2800
F 0 "C4" H 3725 2900 50  0000 L CNN
F 1 "100n" H 3725 2700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3738 2650 50  0001 C CNN
F 3 "" H 3700 2800 50  0001 C CNN
	1    3700 2800
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR14
U 1 1 59C472D9
P 3700 3050
F 0 "#PWR14" H 3700 2800 50  0001 C CNN
F 1 "GND" H 3700 2900 50  0000 C CNN
F 2 "" H 3700 3050 50  0001 C CNN
F 3 "" H 3700 3050 50  0001 C CNN
	1    3700 3050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR27
U 1 1 59C47427
P 9700 2500
F 0 "#PWR27" H 9700 2250 50  0001 C CNN
F 1 "GND" H 9700 2350 50  0000 C CNN
F 2 "" H 9700 2500 50  0001 C CNN
F 3 "" H 9700 2500 50  0001 C CNN
	1    9700 2500
	1    0    0    -1  
$EndComp
$Comp
L MCP1703A-3302_SOT23 U3
U 1 1 59C474C3
P 3250 6600
F 0 "U3" H 3400 6350 50  0000 C CNN
F 1 "MCP1702-3302" H 3000 6750 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 3250 6800 50  0001 C CNN
F 3 "" H 3250 6550 50  0001 C CNN
	1    3250 6600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR10
U 1 1 59C476EE
P 3250 7100
F 0 "#PWR10" H 3250 6850 50  0001 C CNN
F 1 "GND" H 3250 6950 50  0000 C CNN
F 2 "" H 3250 7100 50  0001 C CNN
F 3 "" H 3250 7100 50  0001 C CNN
	1    3250 7100
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR16
U 1 1 59C477CD
P 3750 6500
F 0 "#PWR16" H 3750 6350 50  0001 C CNN
F 1 "+3V3" H 3750 6640 50  0000 C CNN
F 2 "" H 3750 6500 50  0001 C CNN
F 3 "" H 3750 6500 50  0001 C CNN
	1    3750 6500
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 59C4792E
P 2700 6850
F 0 "C1" H 2725 6950 50  0000 L CNN
F 1 "1u/16V" H 2725 6750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 2738 6700 50  0001 C CNN
F 3 "" H 2700 6850 50  0001 C CNN
	1    2700 6850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR17
U 1 1 59C47A90
P 3750 7100
F 0 "#PWR17" H 3750 6850 50  0001 C CNN
F 1 "GND" H 3750 6950 50  0000 C CNN
F 2 "" H 3750 7100 50  0001 C CNN
F 3 "" H 3750 7100 50  0001 C CNN
	1    3750 7100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR9
U 1 1 59C47B21
P 2700 7100
F 0 "#PWR9" H 2700 6850 50  0001 C CNN
F 1 "GND" H 2700 6950 50  0000 C CNN
F 2 "" H 2700 7100 50  0001 C CNN
F 3 "" H 2700 7100 50  0001 C CNN
	1    2700 7100
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J3
U 1 1 59C4821D
P 9550 800
F 0 "J3" H 9550 900 50  0000 C CNN
F 1 "15S" H 9550 700 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 9550 800 50  0001 C CNN
F 3 "" H 9550 800 50  0001 C CNN
	1    9550 800 
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR3
U 1 1 59C49331
P 1250 5650
F 0 "#PWR3" H 1250 5400 50  0001 C CNN
F 1 "GND" H 1250 5500 50  0000 C CNN
F 2 "" H 1250 5650 50  0001 C CNN
F 3 "" H 1250 5650 50  0001 C CNN
	1    1250 5650
	1    0    0    -1  
$EndComp
NoConn ~ 1550 5350
Text Label 3350 5150 2    60   ~ 0
DP
Text Label 3350 5250 2    60   ~ 0
DM
$Comp
L PWR_FLAG #FLG1
U 1 1 59C4A3FC
P 1700 4800
F 0 "#FLG1" H 1700 4875 50  0001 C CNN
F 1 "PWR_FLAG" H 1700 4950 50  0000 C CNN
F 2 "" H 1700 4800 50  0001 C CNN
F 3 "" H 1700 4800 50  0001 C CNN
	1    1700 4800
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG2
U 1 1 59C4A5F6
P 1750 2100
F 0 "#FLG2" H 1750 2175 50  0001 C CNN
F 1 "PWR_FLAG" H 1750 2250 50  0000 C CNN
F 2 "" H 1750 2100 50  0001 C CNN
F 3 "" H 1750 2100 50  0001 C CNN
	1    1750 2100
	1    0    0    -1  
$EndComp
NoConn ~ 8550 3850
NoConn ~ 8550 4450
$Comp
L Conn_01x01 J7
U 1 1 59C4AFD0
P 10550 2950
F 0 "J7" H 10700 2950 50  0000 C CNN
F 1 "0" H 10850 2950 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 2950 50  0001 C CNN
F 3 "" H 10550 2950 50  0001 C CNN
	1    10550 2950
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J8
U 1 1 59C4B114
P 10550 3050
F 0 "J8" H 10700 3050 50  0000 C CNN
F 1 "1" H 10850 3050 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3050 50  0001 C CNN
F 3 "" H 10550 3050 50  0001 C CNN
	1    10550 3050
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J9
U 1 1 59C4B188
P 10550 3150
F 0 "J9" H 10700 3150 50  0000 C CNN
F 1 "2" H 10850 3150 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3150 50  0001 C CNN
F 3 "" H 10550 3150 50  0001 C CNN
	1    10550 3150
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J10
U 1 1 59C4B238
P 10550 3250
F 0 "J10" H 10700 3250 50  0000 C CNN
F 1 "3" H 10850 3250 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3250 50  0001 C CNN
F 3 "" H 10550 3250 50  0001 C CNN
	1    10550 3250
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J11
U 1 1 59C4B296
P 10550 3350
F 0 "J11" H 10700 3350 50  0000 C CNN
F 1 "4" H 10850 3350 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3350 50  0001 C CNN
F 3 "" H 10550 3350 50  0001 C CNN
	1    10550 3350
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J12
U 1 1 59C4B2F7
P 10550 3450
F 0 "J12" H 10700 3450 50  0000 C CNN
F 1 "5" H 10850 3450 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3450 50  0001 C CNN
F 3 "" H 10550 3450 50  0001 C CNN
	1    10550 3450
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J13
U 1 1 59C4B35F
P 10550 3550
F 0 "J13" H 10700 3550 50  0000 C CNN
F 1 "6" H 10850 3550 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3550 50  0001 C CNN
F 3 "" H 10550 3550 50  0001 C CNN
	1    10550 3550
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J14
U 1 1 59C4B3C6
P 10550 3650
F 0 "J14" H 10700 3650 50  0000 C CNN
F 1 "7" H 10850 3650 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3650 50  0001 C CNN
F 3 "" H 10550 3650 50  0001 C CNN
	1    10550 3650
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J15
U 1 1 59C4B44E
P 10550 3750
F 0 "J15" H 10700 3750 50  0000 C CNN
F 1 "8" H 10850 3750 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3750 50  0001 C CNN
F 3 "" H 10550 3750 50  0001 C CNN
	1    10550 3750
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J16
U 1 1 59C4B4BB
P 10550 3850
F 0 "J16" H 10700 3850 50  0000 C CNN
F 1 "9" H 10850 3850 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3850 50  0001 C CNN
F 3 "" H 10550 3850 50  0001 C CNN
	1    10550 3850
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J17
U 1 1 59C4B52B
P 10550 3950
F 0 "J17" H 10700 3950 50  0000 C CNN
F 1 "10" H 10850 3950 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 3950 50  0001 C CNN
F 3 "" H 10550 3950 50  0001 C CNN
	1    10550 3950
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J18
U 1 1 59C4B5A2
P 10550 4050
F 0 "J18" H 10700 4050 50  0000 C CNN
F 1 "11" H 10850 4050 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 4050 50  0001 C CNN
F 3 "" H 10550 4050 50  0001 C CNN
	1    10550 4050
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J19
U 1 1 59C4B6C2
P 10550 4150
F 0 "J19" H 10700 4150 50  0000 C CNN
F 1 "12" H 10850 4150 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 4150 50  0001 C CNN
F 3 "" H 10550 4150 50  0001 C CNN
	1    10550 4150
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J20
U 1 1 59C4B73B
P 10550 4250
F 0 "J20" H 10700 4250 50  0000 C CNN
F 1 "13" H 10850 4250 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 4250 50  0001 C CNN
F 3 "" H 10550 4250 50  0001 C CNN
	1    10550 4250
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J21
U 1 1 59C4B7B7
P 10550 4350
F 0 "J21" H 10700 4350 50  0000 C CNN
F 1 "14" H 10850 4350 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 4350 50  0001 C CNN
F 3 "" H 10550 4350 50  0001 C CNN
	1    10550 4350
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J22
U 1 1 59C4B836
P 10550 4450
F 0 "J22" H 10700 4450 50  0000 C CNN
F 1 "15" H 10850 4450 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10550 4450 50  0001 C CNN
F 3 "" H 10550 4450 50  0001 C CNN
	1    10550 4450
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG4
U 1 1 59C4CC7B
P 2300 6450
F 0 "#FLG4" H 2300 6525 50  0001 C CNN
F 1 "PWR_FLAG" H 2300 6600 50  0000 C CNN
F 2 "" H 2300 6450 50  0001 C CNN
F 3 "" H 2300 6450 50  0001 C CNN
	1    2300 6450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR4
U 1 1 59C4DC2B
P 1500 2250
F 0 "#PWR4" H 1500 2000 50  0001 C CNN
F 1 "GND" H 1500 2100 50  0000 C CNN
F 2 "" H 1500 2250 50  0001 C CNN
F 3 "" H 1500 2250 50  0001 C CNN
	1    1500 2250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR5
U 1 1 59C4DE54
P 1750 1100
F 0 "#PWR5" H 1750 950 50  0001 C CNN
F 1 "VCC" H 1750 1250 50  0000 C CNN
F 2 "" H 1750 1100 50  0001 C CNN
F 3 "" H 1750 1100 50  0001 C CNN
	1    1750 1100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR1
U 1 1 59C4E307
P 850 6650
F 0 "#PWR1" H 850 6500 50  0001 C CNN
F 1 "VCC" H 850 6800 50  0000 C CNN
F 2 "" H 850 6650 50  0001 C CNN
F 3 "" H 850 6650 50  0001 C CNN
	1    850  6650
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 59C50F45
P 2150 1350
F 0 "R1" V 2100 1200 50  0000 R CNN
F 1 "680" V 2150 1350 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2080 1350 50  0001 C CNN
F 3 "" H 2150 1350 50  0001 C CNN
	1    2150 1350
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 59C512D3
P 2150 1650
F 0 "R4" V 2100 1500 50  0000 R CNN
F 1 "680" V 2150 1650 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2080 1650 50  0001 C CNN
F 3 "" H 2150 1650 50  0001 C CNN
	1    2150 1650
	0    1    1    0   
$EndComp
Text Label 7450 3450 2    60   ~ 0
OUT/Tx
Text Label 2700 1350 2    60   ~ 0
ST/Rx
Text Label 2700 1650 2    60   ~ 0
OUT/Tx
$Comp
L PWR_FLAG #FLG3
U 1 1 59C5625A
P 2150 1100
F 0 "#FLG3" H 2150 1175 50  0001 C CNN
F 1 "PWR_FLAG" H 2150 1250 50  0000 C CNN
F 2 "" H 2150 1100 50  0001 C CNN
F 3 "" H 2150 1100 50  0001 C CNN
	1    2150 1100
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J6
U 1 1 59C4B6A6
P 9450 5850
F 0 "J6" H 9450 5950 50  0000 C CNN
F 1 "+LED" H 9600 5850 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 9450 5850 50  0001 C CNN
F 3 "" H 9450 5850 50  0001 C CNN
	1    9450 5850
	0    -1   -1   0   
$EndComp
$Comp
L Conn_01x01 J23
U 1 1 59C4B79E
P 9650 5850
F 0 "J23" H 9650 5950 50  0000 C CNN
F 1 "+LED" H 9800 5850 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 9650 5850 50  0001 C CNN
F 3 "" H 9650 5850 50  0001 C CNN
	1    9650 5850
	0    -1   -1   0   
$EndComp
$Comp
L Conn_01x01 J24
U 1 1 59C4BD3B
P 9850 5850
F 0 "J24" H 9850 5950 50  0000 C CNN
F 1 "+LED" H 10000 5850 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 9850 5850 50  0001 C CNN
F 3 "" H 9850 5850 50  0001 C CNN
	1    9850 5850
	0    -1   -1   0   
$EndComp
$Comp
L Conn_01x01 J25
U 1 1 59C4BDD3
P 10050 5850
F 0 "J25" H 10050 5950 50  0000 C CNN
F 1 "+LED" H 10200 5850 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10050 5850 50  0001 C CNN
F 3 "" H 10050 5850 50  0001 C CNN
	1    10050 5850
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR22
U 1 1 59C4C32D
P 8850 5950
F 0 "#PWR22" H 8850 5800 50  0001 C CNN
F 1 "VCC" H 8850 6100 50  0000 C CNN
F 2 "" H 8850 5950 50  0001 C CNN
F 3 "" H 8850 5950 50  0001 C CNN
	1    8850 5950
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q1
U 1 1 59C4D834
P 9450 1350
F 0 "Q1" H 9650 1400 50  0000 L CNN
F 1 "MOSFET 20V 3A" H 9650 1300 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 9650 1450 50  0001 C CNN
F 3 "" H 9450 1350 50  0001 C CNN
	1    9450 1350
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J26
U 1 1 59DB376D
P 10250 5850
F 0 "J26" H 10250 5950 50  0000 C CNN
F 1 "+LED" H 10400 5850 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10250 5850 50  0001 C CNN
F 3 "" H 10250 5850 50  0001 C CNN
	1    10250 5850
	0    -1   -1   0   
$EndComp
$Comp
L Conn_01x01 J27
U 1 1 59DB380B
P 10450 5850
F 0 "J27" H 10450 5950 50  0000 C CNN
F 1 "+LED" H 10600 5850 50  0000 C CNN
F 2 "SMD-pads:SMD50x100" H 10450 5850 50  0001 C CNN
F 3 "" H 10450 5850 50  0001 C CNN
	1    10450 5850
	0    -1   -1   0   
$EndComp
$Comp
L TEST TP4
U 1 1 59DB5CFC
P 5850 5950
F 0 "TP4" V 5850 6150 50  0000 L BNN
F 1 "GND" V 5900 6150 50  0000 L CNN
F 2 "SMD-pads:Pin_Header_Straight_1x01_Pitch2.54mm_no_3D_model" H 5850 5950 50  0001 C CNN
F 3 "" H 5850 5950 50  0001 C CNN
	1    5850 5950
	0    1    1    0   
$EndComp
$Comp
L GND #PWR19
U 1 1 59DB5EB3
P 5850 6050
F 0 "#PWR19" H 5850 5800 50  0001 C CNN
F 1 "GND" H 5850 5900 50  0000 C CNN
F 2 "" H 5850 6050 50  0001 C CNN
F 3 "" H 5850 6050 50  0001 C CNN
	1    5850 6050
	1    0    0    -1  
$EndComp
$Comp
L LED_ALT D3
U 1 1 59DC7006
P 7750 5300
F 0 "D3" H 7750 5400 50  0000 C CNN
F 1 "LED" H 7750 5200 50  0000 C CNN
F 2 "LEDs:LED_0603" H 7750 5300 50  0001 C CNN
F 3 "" H 7750 5300 50  0001 C CNN
	1    7750 5300
	0    -1   -1   0   
$EndComp
$Comp
L R R6
U 1 1 59DC70B5
P 7750 5700
F 0 "R6" V 7830 5700 50  0000 C CNN
F 1 "1k2" V 7750 5700 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 5700 50  0001 C CNN
F 3 "" H 7750 5700 50  0001 C CNN
	1    7750 5700
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR20
U 1 1 59DC74AB
P 7750 5900
F 0 "#PWR20" H 7750 5650 50  0001 C CNN
F 1 "GND" H 7750 5750 50  0000 C CNN
F 2 "" H 7750 5900 50  0001 C CNN
F 3 "" H 7750 5900 50  0001 C CNN
	1    7750 5900
	1    0    0    -1  
$EndComp
Text Label 7450 2250 2    60   ~ 0
XLAT
Text Label 7450 1850 2    60   ~ 0
OUT15T
$Comp
L GS2 J4
U 1 1 59DCA798
P 1300 6600
F 0 "J4" H 1400 6750 50  0000 C CNN
F 1 "GS2" H 1400 6451 50  0000 C CNN
F 2 "SMD-pads:SMD_solder_jumper_2pin" V 1374 6600 50  0001 C CNN
F 3 "" H 1300 6600 50  0001 C CNN
	1    1300 6600
	-1   0    0    1   
$EndComp
Text Label 8150 3250 0    60   ~ 0
GSCLK
Text Label 7450 2350 2    60   ~ 0
BLANK
Text Label 7450 3350 2    60   ~ 0
TH
Text Label 7450 2550 2    60   ~ 0
GSCLK
Text Label 7450 4050 2    60   ~ 0
SWCLK
Text Label 7450 4150 2    60   ~ 0
SWDIO
Text Label 5400 5450 0    60   ~ 0
SWCLK
Text Label 5400 5700 0    60   ~ 0
SWDIO
Text Label 3400 1650 0    60   ~ 0
RESET
Text Label 7450 3250 2    60   ~ 0
CH3
$Comp
L GND #PWR2
U 1 1 5A0ED963
P 1100 4000
F 0 "#PWR2" H 1100 3750 50  0001 C CNN
F 1 "GND" H 1100 3850 50  0000 C CNN
F 2 "" H 1100 4000 50  0001 C CNN
F 3 "" H 1100 4000 50  0001 C CNN
	1    1100 4000
	1    0    0    -1  
$EndComp
Text Label 2500 3200 2    60   ~ 0
Button
Text Label 8150 3450 0    60   ~ 0
BLANK
Text Label 8150 3550 0    60   ~ 0
XLAT
Text Label 8150 4250 0    60   ~ 0
SCLK
Text Label 8150 4350 0    60   ~ 0
SIN
Text Label 8600 1350 0    60   ~ 0
OUT15T
$Comp
L Conn_01x08_Male J1
U 1 1 5A0F5BA3
P 1100 1550
F 0 "J1" H 1100 1850 50  0000 C CNN
F 1 "IN/OUT" H 1100 1150 50  0000 C CNN
F 2 "SMD-pads:CONN_8_SMD_FLAT" H 1100 1550 50  0001 C CNN
F 3 "" H 1100 1550 50  0001 C CNN
	1    1100 1550
	1    0    0    1   
$EndComp
Text Label 7450 2150 2    60   ~ 0
SCLK
Text Label 7450 2050 2    60   ~ 0
SIN
Text Label 7450 2750 2    60   ~ 0
BUTTON
Text Label 1700 5800 1    60   ~ 0
VUSB
Text Label 2400 6600 0    60   ~ 0
V_IN
Text Label 8150 3050 0    60   ~ 0
IREF
$Comp
L BAT54C D1
U 1 1 5A105797
P 1700 6600
F 0 "D1" H 1725 6450 50  0000 L CNN
F 1 "BAT54C" H 1450 6725 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 1775 6725 50  0001 L CNN
F 3 "" H 1570 6600 50  0001 C CNN
	1    1700 6600
	0    -1   1    0   
$EndComp
$Comp
L GND #PWR7
U 1 1 5A111798
P 2050 3400
F 0 "#PWR7" H 2050 3150 50  0001 C CNN
F 1 "GND" H 2050 3250 50  0000 C CNN
F 2 "" H 2050 3400 50  0001 C CNN
F 3 "" H 2050 3400 50  0001 C CNN
	1    2050 3400
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR6
U 1 1 5A111DE2
P 2050 3000
F 0 "#PWR6" H 2050 2850 50  0001 C CNN
F 1 "+3V3" H 2050 3140 50  0000 C CNN
F 2 "" H 2050 3000 50  0001 C CNN
F 3 "" H 2050 3000 50  0001 C CNN
	1    2050 3000
	1    0    0    -1  
$EndComp
Text Notes 850  3150 0    30   ~ 0
External push button
$Comp
L Conn_01x01 J28
U 1 1 5A114975
P 1750 3200
F 0 "J28" H 1750 3000 50  0000 C CNN
F 1 "Button" H 1900 3200 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1750 3200 50  0001 C CNN
F 3 "" H 1750 3200 50  0001 C CNN
	1    1750 3200
	-1   0    0    1   
$EndComp
$Comp
L Conn_01x01 J5
U 1 1 5A114E6C
P 1750 3100
F 0 "J5" H 1550 3250 50  0000 C CNN
F 1 "+3V3" H 1900 3100 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1750 3100 50  0001 C CNN
F 3 "" H 1750 3100 50  0001 C CNN
	1    1750 3100
	-1   0    0    1   
$EndComp
$Comp
L Conn_01x01 J29
U 1 1 5A114F10
P 1750 3300
F 0 "J29" H 1750 3400 50  0000 C CNN
F 1 "GND" H 1900 3300 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1750 3300 50  0001 C CNN
F 3 "" H 1750 3300 50  0001 C CNN
	1    1750 3300
	-1   0    0    1   
$EndComp
$Comp
L CP C2
U 1 1 5A1150A8
P 3750 6850
F 0 "C2" H 3775 6950 50  0000 L CNN
F 1 "47u/6.3V Polymer 0805" H 3775 6750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 3788 6700 50  0001 C CNN
F 3 "" H 3750 6850 50  0001 C CNN
	1    3750 6850
	1    0    0    -1  
$EndComp
Text Notes 9800 900  0    60   ~ 0
High-power switched\nlight output
Text Notes 1000 7650 0    60   ~ 0
Solder jumper to\npower LEDs from USB\nWARNING: do not connect\nUSB and another power \nsupply (ESC, Receiver) \nat the same time!
$Comp
L SAMD20E15A-MU U1
U 1 1 5A2E1938
P 5100 2900
F 0 "U1" H 4050 4300 50  0000 C CNN
F 1 "SAMD21E15A-MU" H 5950 1500 50  0000 C CNN
F 2 "Housings_DFN_QFN:QFN-32-1EP_5x5mm_Pitch0.5mm" H 5100 1900 50  0001 C CIN
F 3 "" H 5100 2900 50  0001 C CNN
	1    5100 2900
	1    0    0    -1  
$EndComp
Text Label 7450 3550 2    60   ~ 0
ST/Rx
NoConn ~ 6700 1650
$Comp
L PRTR5V0U2X D2
U 1 1 5A2E4F71
P 2450 5650
F 0 "D2" H 2450 5900 50  0000 C CNN
F 1 "PRTR5V0U2X" H 2450 5400 50  0000 C CNN
F 2 "SOT143B:SOT143B" H 2500 5600 50  0001 C CNN
F 3 "" H 2500 5600 50  0001 C CNN
	1    2450 5650
	0    -1   -1   0   
$EndComp
NoConn ~ 1150 5650
$Comp
L GND #PWR8
U 1 1 5A2E6905
P 2550 6050
F 0 "#PWR8" H 2550 5800 50  0001 C CNN
F 1 "GND" H 2550 5900 50  0000 C CNN
F 2 "" H 2550 6050 50  0001 C CNN
F 3 "" H 2550 6050 50  0001 C CNN
	1    2550 6050
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 5A2F6947
P 3300 2800
F 0 "C5" H 3325 2900 50  0000 L CNN
F 1 "1u" H 3325 2700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3338 2650 50  0001 C CNN
F 3 "" H 3300 2800 50  0001 C CNN
	1    3300 2800
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR12
U 1 1 5A2F6A32
P 3300 3050
F 0 "#PWR12" H 3300 2800 50  0001 C CNN
F 1 "GND" H 3300 2900 50  0000 C CNN
F 2 "" H 3300 3050 50  0001 C CNN
F 3 "" H 3300 3050 50  0001 C CNN
	1    3300 3050
	1    0    0    -1  
$EndComp
Text Label 3300 2400 0    60   ~ 0
VCORE
NoConn ~ 6700 1950
Text Label 7350 5050 0    60   ~ 0
LED
Text Label 7450 1750 2    60   ~ 0
LED
$Comp
L R R3
U 1 1 5A8803DC
P 2150 1550
F 0 "R3" V 2100 1400 50  0000 R CNN
F 1 "680" V 2150 1550 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2080 1550 50  0001 C CNN
F 3 "" H 2150 1550 50  0001 C CNN
	1    2150 1550
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 5A88049B
P 2150 1450
F 0 "R2" V 2100 1300 50  0000 R CNN
F 1 "680" V 2150 1450 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2080 1450 50  0001 C CNN
F 3 "" H 2150 1450 50  0001 C CNN
	1    2150 1450
	0    1    1    0   
$EndComp
Text Label 2700 1450 2    60   ~ 0
TH
Text Label 2700 1550 2    60   ~ 0
CH3
Text Label 10000 2950 0    60   ~ 0
OUT0
Text Label 10000 3050 0    60   ~ 0
OUT1
Text Label 10000 3150 0    60   ~ 0
OUT2
Text Label 10000 3250 0    60   ~ 0
OUT3
Text Label 10000 3350 0    60   ~ 0
OUT4
Text Label 10000 3450 0    60   ~ 0
OUT5
Text Label 10000 3550 0    60   ~ 0
OUT6
Text Label 10000 3650 0    60   ~ 0
OUT7
Text Label 10000 3750 0    60   ~ 0
OUT8
Text Label 10000 3850 0    60   ~ 0
OUT9
Text Label 10000 3950 0    60   ~ 0
OUT10
Text Label 10000 4050 0    60   ~ 0
OUT11
Text Label 10000 4150 0    60   ~ 0
OUT12
Text Label 10000 4250 0    60   ~ 0
OUT13
Text Label 10000 4450 0    60   ~ 0
OUT15
Text Label 10000 4350 0    60   ~ 0
OUT14
Text Label 9550 1100 0    60   ~ 0
OUT15S
Text Label 6450 1950 0    60   ~ 0
PA03
Text Label 6450 1650 0    60   ~ 0
PA00
Text Label 6450 2450 0    60   ~ 0
PA08
NoConn ~ 6700 2450
NoConn ~ 6700 2650
Text Label 6450 2650 0    60   ~ 0
PA10
Text Label 6450 2850 0    60   ~ 0
PA14
Text Label 6450 2950 0    60   ~ 0
PA15
Text Label 6450 3050 0    60   ~ 0
PA16
Text Label 6450 3150 0    60   ~ 0
PA17
NoConn ~ 6700 2850
NoConn ~ 6700 2950
NoConn ~ 6700 3050
NoConn ~ 6700 3150
NoConn ~ 6700 3850
NoConn ~ 6700 3950
Text Label 6450 3850 0    60   ~ 0
PA27
Text Label 6450 3950 0    60   ~ 0
PA28
Wire Wire Line
	3750 4050 3750 4250
Wire Wire Line
	9250 2350 9250 2650
Wire Wire Line
	6350 2150 7450 2150
Wire Wire Line
	8150 3250 8550 3250
Wire Wire Line
	1100 3200 1100 3500
Wire Wire Line
	8150 3550 8550 3550
Wire Wire Line
	6350 2250 7450 2250
Wire Wire Line
	6350 3650 7450 3650
Wire Wire Line
	6350 3750 7450 3750
Wire Wire Line
	9550 1550 9550 1750
Wire Wire Line
	9550 1000 9550 1150
Wire Wire Line
	8050 2450 9350 2450
Wire Wire Line
	8450 2450 8450 3150
Wire Wire Line
	8450 2950 8550 2950
Connection ~ 9250 2450
Wire Wire Line
	8450 3150 8550 3150
Connection ~ 8450 2950
Wire Wire Line
	7800 3050 8550 3050
Wire Wire Line
	7800 3450 7800 3850
Wire Wire Line
	9650 2450 9700 2450
Wire Wire Line
	9700 2450 9700 2500
Wire Wire Line
	3250 6900 3250 7100
Wire Wire Line
	2700 6600 2700 6700
Wire Wire Line
	3550 6600 3750 6600
Wire Wire Line
	3750 6500 3750 6700
Connection ~ 3750 6600
Wire Wire Line
	3750 7000 3750 7100
Wire Wire Line
	2700 7000 2700 7100
Connection ~ 2700 6600
Connection ~ 2300 6600
Wire Wire Line
	1700 4800 1700 6300
Wire Wire Line
	1550 5150 3350 5150
Wire Wire Line
	1550 5250 3350 5250
Wire Wire Line
	1250 5550 1250 5650
Wire Wire Line
	1150 5550 1150 5650
Connection ~ 1700 5950
Wire Wire Line
	9950 4450 10350 4450
Wire Wire Line
	9950 4350 10350 4350
Wire Wire Line
	9950 4250 10350 4250
Wire Wire Line
	9950 4150 10350 4150
Wire Wire Line
	9950 4050 10350 4050
Wire Wire Line
	9950 3950 10350 3950
Wire Wire Line
	9950 3850 10350 3850
Wire Wire Line
	9950 3750 10350 3750
Wire Wire Line
	9950 3650 10350 3650
Wire Wire Line
	9950 3550 10350 3550
Wire Wire Line
	9950 3450 10350 3450
Wire Wire Line
	9950 3350 10350 3350
Wire Wire Line
	9950 3250 10350 3250
Wire Wire Line
	9950 3150 10350 3150
Wire Wire Line
	9950 2950 10350 2950
Wire Wire Line
	9950 3050 10350 3050
Wire Wire Line
	1300 1850 1500 1850
Wire Wire Line
	1500 1150 1500 2250
Wire Wire Line
	850  6650 850  7000
Wire Wire Line
	1750 1100 1750 1750
Wire Wire Line
	8600 1350 9250 1350
Wire Wire Line
	1300 1550 2000 1550
Wire Wire Line
	2300 1550 2700 1550
Wire Wire Line
	2150 1250 2150 1100
Wire Wire Line
	1300 1250 2150 1250
Connection ~ 1750 1250
Wire Wire Line
	8850 5950 8850 6250
Wire Wire Line
	8850 6250 10450 6250
Wire Wire Line
	9450 6250 9450 6050
Wire Wire Line
	9650 6250 9650 6050
Connection ~ 9450 6250
Wire Wire Line
	9850 6250 9850 6050
Connection ~ 9650 6250
Wire Wire Line
	10050 6250 10050 6050
Connection ~ 9850 6250
Wire Wire Line
	10250 6250 10250 6050
Connection ~ 10050 6250
Wire Wire Line
	10450 6250 10450 6050
Connection ~ 10250 6250
Wire Wire Line
	5850 5950 5850 6050
Connection ~ 1500 1850
Wire Wire Line
	1750 1750 1300 1750
Wire Wire Line
	7750 5850 7750 5900
Wire Wire Line
	1750 2100 1750 2150
Wire Wire Line
	1750 2150 1500 2150
Connection ~ 1500 2150
Wire Wire Line
	9250 4750 9250 5000
Wire Wire Line
	9150 4750 9150 4900
Wire Wire Line
	9150 4900 9250 4900
Connection ~ 9250 4900
Wire Wire Line
	6350 2050 7450 2050
Wire Wire Line
	6350 4050 7450 4050
Wire Wire Line
	6350 4150 7450 4150
Wire Wire Line
	3300 1650 3850 1650
Wire Wire Line
	6350 2350 7450 2350
Wire Wire Line
	1100 3900 1100 4000
Wire Wire Line
	8550 4250 8150 4250
Wire Wire Line
	8550 4350 8150 4350
Wire Wire Line
	7800 3050 7800 3150
Wire Wire Line
	5400 5450 5850 5450
Wire Wire Line
	5850 5700 5400 5700
Wire Wire Line
	850  7000 1700 7000
Wire Wire Line
	1700 7000 1700 6900
Wire Wire Line
	1900 6600 2950 6600
Wire Wire Line
	2300 6450 2300 6600
Wire Notes Line
	800  3050 800  4300
Wire Notes Line
	800  4300 1400 4300
Wire Notes Line
	1400 4300 1400 3050
Wire Notes Line
	1400 3050 800  3050
Wire Wire Line
	1950 3300 2050 3300
Wire Wire Line
	2050 3300 2050 3400
Wire Wire Line
	1950 3100 2050 3100
Wire Wire Line
	2050 3100 2050 3000
Wire Wire Line
	1100 3200 2500 3200
Wire Wire Line
	1300 6800 1300 7000
Connection ~ 1300 7000
Wire Wire Line
	1300 6400 1300 6200
Wire Wire Line
	1300 6200 1700 6200
Connection ~ 1700 6200
Wire Wire Line
	3700 1950 3700 2650
Wire Wire Line
	3700 2050 3850 2050
Wire Wire Line
	3850 2550 3700 2550
Connection ~ 3700 2550
Wire Wire Line
	3850 4150 3750 4150
Wire Wire Line
	3850 4050 3750 4050
Connection ~ 3750 4150
Wire Wire Line
	6350 3550 7450 3550
Connection ~ 3700 2050
Wire Wire Line
	3700 2950 3700 3050
Wire Wire Line
	6350 3350 7450 3350
Wire Wire Line
	1700 5950 2350 5950
Wire Wire Line
	1550 4950 1700 4950
Connection ~ 1700 4950
Wire Wire Line
	2550 5950 2550 6050
Wire Wire Line
	2350 5350 2350 5150
Connection ~ 2350 5150
Wire Wire Line
	2550 5350 2550 5250
Connection ~ 2550 5250
Wire Wire Line
	3300 2950 3300 3050
Wire Wire Line
	6350 2750 7450 2750
Wire Wire Line
	6350 2550 7450 2550
Wire Wire Line
	6350 3450 7450 3450
Wire Wire Line
	6350 1850 7450 1850
Wire Wire Line
	7750 5050 7350 5050
Wire Wire Line
	6350 1750 7450 1750
Wire Wire Line
	6350 3250 7450 3250
Wire Wire Line
	1500 1150 1300 1150
Wire Wire Line
	2300 1450 2700 1450
Wire Wire Line
	2300 1350 2700 1350
Wire Wire Line
	1300 1350 2000 1350
Wire Wire Line
	2000 1450 1300 1450
Wire Wire Line
	7750 5050 7750 5150
Wire Wire Line
	7750 5450 7750 5550
Wire Wire Line
	6700 1950 6350 1950
Wire Wire Line
	6700 1650 6350 1650
Wire Wire Line
	6350 2450 6700 2450
Wire Wire Line
	6700 2650 6350 2650
Wire Wire Line
	6700 2850 6350 2850
Wire Wire Line
	6700 2950 6350 2950
Wire Wire Line
	6700 3050 6350 3050
Wire Wire Line
	6700 3150 6350 3150
Wire Wire Line
	6700 3850 6350 3850
Wire Wire Line
	6700 3950 6350 3950
Text Notes 2200 4750 0    60   ~ 0
USB and power supply
Text Notes 4850 1350 0    60   ~ 0
Microcontroller
Text Notes 1150 750  0    60   ~ 0
Input/Output connector
Text Notes 4850 4750 0    60   ~ 0
SWD debug connector and test points
Text Notes 7350 4750 0    60   ~ 0
Status LED
Text Notes 8750 2100 0    60   ~ 0
Constant-current LED driver
Text Notes 9400 5500 0    60   ~ 0
LED supply connections
Connection ~ 1950 3200
$Comp
L TEST TP3
U 1 1 5A8BCF2D
P 5850 6350
F 0 "TP3" V 5850 6550 50  0000 L BNN
F 1 "RESET" V 5900 6550 50  0000 L CNN
F 2 "SMD-pads:Measurement_Point_Round-SMD-Pad_1mm" H 5850 6350 50  0001 C CNN
F 3 "" H 5850 6350 50  0001 C CNN
	1    5850 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	5350 6350 5850 6350
Text Label 5350 6350 0    60   ~ 0
RESET
$Comp
L TEST TP5
U 1 1 5A8BE653
P 5850 6550
F 0 "TP5" V 5850 6750 50  0000 L BNN
F 1 "RESET" V 5900 6750 50  0000 L CNN
F 2 "SMD-pads:Measurement_Point_Round-SMD-Pad_1mm" H 5850 6550 50  0001 C CNN
F 3 "" H 5850 6550 50  0001 C CNN
	1    5850 6550
	0    1    1    0   
$EndComp
Wire Wire Line
	5850 6550 5750 6550
Wire Wire Line
	5750 6550 5750 6350
Connection ~ 5750 6350
$Comp
L TEST TP7
U 1 1 5A8E31C0
P 5850 7050
F 0 "TP7" V 5850 7250 50  0000 L BNN
F 1 "D" V 5900 7250 50  0000 L CNN
F 2 "SMD-pads:Measurement_Point_Round-SMD-Pad_1mm" H 5850 7050 50  0001 C CNN
F 3 "" H 5850 7050 50  0001 C CNN
	1    5850 7050
	0    1    1    0   
$EndComp
$Comp
L TEST TP8
U 1 1 5A8E34EA
P 5850 7250
F 0 "TP8" V 5850 7450 50  0000 L BNN
F 1 "X" V 5900 7450 50  0000 L CNN
F 2 "SMD-pads:Measurement_Point_Round-SMD-Pad_1mm" H 5850 7250 50  0001 C CNN
F 3 "" H 5850 7250 50  0001 C CNN
	1    5850 7250
	0    1    1    0   
$EndComp
$Comp
L TEST TP9
U 1 1 5A8E35A3
P 5850 7450
F 0 "TP9" V 5850 7650 50  0000 L BNN
F 1 "B" V 5900 7650 50  0000 L CNN
F 2 "SMD-pads:Measurement_Point_Round-SMD-Pad_1mm" H 5850 7450 50  0001 C CNN
F 3 "" H 5850 7450 50  0001 C CNN
	1    5850 7450
	0    1    1    0   
$EndComp
$Comp
L TEST TP10
U 1 1 5A8E365F
P 5850 7650
F 0 "TP10" V 5850 7850 50  0000 L BNN
F 1 "G" V 5900 7850 50  0000 L CNN
F 2 "SMD-pads:Measurement_Point_Round-SMD-Pad_1mm" H 5850 7650 50  0001 C CNN
F 3 "" H 5850 7650 50  0001 C CNN
	1    5850 7650
	0    1    1    0   
$EndComp
$Comp
L TEST TP6
U 1 1 5A8E371E
P 5850 6850
F 0 "TP6" V 5850 7050 50  0000 L BNN
F 1 "C" V 5900 7050 50  0000 L CNN
F 2 "SMD-pads:Measurement_Point_Round-SMD-Pad_1mm" H 5850 6850 50  0001 C CNN
F 3 "" H 5850 6850 50  0001 C CNN
	1    5850 6850
	0    1    1    0   
$EndComp
Wire Wire Line
	5850 6850 5350 6850
Wire Wire Line
	5350 7050 5850 7050
Wire Wire Line
	5350 7250 5850 7250
Wire Wire Line
	5350 7450 5850 7450
Wire Wire Line
	5350 7650 5850 7650
Text Label 5350 6850 0    60   ~ 0
SCLK
Text Label 5350 7050 0    60   ~ 0
SIN
Text Label 5350 7250 0    60   ~ 0
XLAT
Text Label 5350 7450 0    60   ~ 0
BLANK
Text Label 5350 7650 0    60   ~ 0
GSCLK
$Comp
L R R5
U 1 1 5A8E5E71
P 5800 5200
F 0 "R5" V 5880 5200 50  0000 C CNN
F 1 "10k" V 5800 5200 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5730 5200 50  0001 C CNN
F 3 "" H 5800 5200 50  0001 C CNN
	1    5800 5200
	-1   0    0    1   
$EndComp
$Comp
L +3V3 #PWR18
U 1 1 5A8E6550
P 5800 5000
F 0 "#PWR18" H 5800 4850 50  0001 C CNN
F 1 "+3V3" H 5800 5140 50  0000 C CNN
F 2 "" H 5800 5000 50  0001 C CNN
F 3 "" H 5800 5000 50  0001 C CNN
	1    5800 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR11
U 1 1 5A8E7447
P 3300 2050
F 0 "#PWR11" H 3300 1800 50  0001 C CNN
F 1 "GND" H 3150 2000 50  0000 C CNN
F 2 "" H 3300 2050 50  0001 C CNN
F 3 "" H 3300 2050 50  0001 C CNN
	1    3300 2050
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5A8E7506
P 3300 1850
F 0 "C6" H 3325 1950 50  0000 L CNN
F 1 "100n" H 3325 1750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3338 1700 50  0001 C CNN
F 3 "" H 3300 1850 50  0001 C CNN
	1    3300 1850
	-1   0    0    1   
$EndComp
Wire Wire Line
	3300 2000 3300 2050
Wire Wire Line
	3300 1650 3300 1700
Wire Wire Line
	3850 2200 3300 2200
Wire Wire Line
	3300 2200 3300 2650
Wire Wire Line
	5800 5000 5800 5050
Wire Wire Line
	5800 5350 5800 5450
Connection ~ 5800 5450
Wire Wire Line
	1300 1650 2000 1650
Wire Wire Line
	2300 1650 2700 1650
$Comp
L R R9
U 1 1 5A9553F3
P 9100 1550
F 0 "R9" V 9180 1550 50  0000 C CNN
F 1 "100k" V 9000 1550 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 9030 1550 50  0001 C CNN
F 3 "" H 9100 1550 50  0001 C CNN
	1    9100 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR23
U 1 1 5A9555C5
P 9100 1750
F 0 "#PWR23" H 9100 1500 50  0001 C CNN
F 1 "GND" H 9100 1600 50  0000 C CNN
F 2 "" H 9100 1750 50  0001 C CNN
F 3 "" H 9100 1750 50  0001 C CNN
	1    9100 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 1700 9100 1750
Wire Wire Line
	9100 1400 9100 1350
Connection ~ 9100 1350
$Comp
L R R8
U 1 1 5A9560F6
P 8050 2800
F 0 "R8" V 8130 2800 50  0000 C CNN
F 1 "100k" V 7950 2800 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7980 2800 50  0001 C CNN
F 3 "" H 8050 2800 50  0001 C CNN
	1    8050 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 3450 8050 3450
Wire Wire Line
	8050 3450 8050 2950
Wire Wire Line
	8050 2650 8050 2450
Connection ~ 8450 2450
$EndSCHEMATC
