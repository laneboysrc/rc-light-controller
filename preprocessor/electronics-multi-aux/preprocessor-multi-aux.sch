EESchema Schematic File Version 4
LIBS:preprocessor-multi-aux-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Pre-processor for RC Light Controller"
Date "2019-07-12"
Rev "3.0"
Comp "LANE Boys RC"
Comment1 "Support for 3 AUX channels"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L WLA-NXP-LPC:LPC811M001JDH16_TSSOP16 U2
U 1 1 58AF9BA8
P 4850 2550
F 0 "U2" H 4750 2650 10  0001 C CNN
F 1 "LPC811M001JDH16_TSSOP16" H 5100 3250 60  0000 C CNN
F 2 "Package_SO:TSSOP-16_4.4x5mm_P0.65mm" H 4750 2650 60  0001 C CNN
F 3 "" H 4750 2650 60  0000 C CNN
	1    4850 2550
	1    0    0    -1  
$EndComp
$Comp
L preprocessor-multi-aux-rescue:MCP1703T-3302E_CB-preprocessor-rescue U1
U 1 1 58AF9BBC
P 8750 5150
F 0 "U1" H 8250 4650 50  0000 L BNN
F 1 "MCP1703T-3302E/CB" H 8350 5400 50  0000 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23W_Handsoldering" H 8900 4700 30  0001 C CNN
F 3 "" H 8750 5150 60  0000 C CNN
	1    8750 5150
	1    0    0    -1  
$EndComp
$Comp
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P1
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
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P2
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
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P3
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
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P4
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
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P5
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
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P6
U 1 1 58AF9DBB
P 8200 2550
F 0 "P6" H 8200 2750 50  0000 C CNN
F 1 "LIGHT CONTROLLER" V 8300 2550 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 8200 2550 50  0001 C CNN
F 3 "" H 8200 2550 50  0000 C CNN
	1    8200 2550
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
P 7750 5650
F 0 "#PWR02" H 7750 5400 50  0001 C CNN
F 1 "GND" H 7750 5500 50  0000 C CNN
F 2 "" H 7750 5650 50  0000 C CNN
F 3 "" H 7750 5650 50  0000 C CNN
	1    7750 5650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 58AFA0EC
P 3550 3250
F 0 "#PWR03" H 3550 3000 50  0001 C CNN
F 1 "GND" H 3550 3100 50  0000 C CNN
F 2 "" H 3550 3250 50  0000 C CNN
F 3 "" H 3550 3250 50  0000 C CNN
	1    3550 3250
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR04
U 1 1 58AFA135
P 3550 1950
F 0 "#PWR04" H 3550 1800 50  0001 C CNN
F 1 "+3V3" H 3550 2090 50  0000 C CNN
F 2 "" H 3550 1950 50  0000 C CNN
F 3 "" H 3550 1950 50  0000 C CNN
	1    3550 1950
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
P 7750 4800
F 0 "#PWR06" H 7750 4650 50  0001 C CNN
F 1 "VCC" H 7750 4950 50  0000 C CNN
F 2 "" H 7750 4800 50  0000 C CNN
F 3 "" H 7750 4800 50  0000 C CNN
	1    7750 4800
	1    0    0    -1  
$EndComp
$Comp
L preprocessor-multi-aux-rescue:C-preprocessor-rescue C1
U 1 1 58AFA243
P 7750 5350
F 0 "C1" H 7775 5450 50  0000 L CNN
F 1 "1u" H 7775 5250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 7788 5200 50  0001 C CNN
F 3 "" H 7750 5350 50  0000 C CNN
	1    7750 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 58AFA4AA
P 7950 5650
F 0 "#PWR07" H 7950 5400 50  0001 C CNN
F 1 "GND" H 7950 5500 50  0000 C CNN
F 2 "" H 7950 5650 50  0000 C CNN
F 3 "" H 7950 5650 50  0000 C CNN
	1    7950 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 4800 7750 5150
Wire Wire Line
	7750 5500 7750 5650
Wire Wire Line
	8050 5150 7750 5150
Connection ~ 7750 5150
Wire Wire Line
	8050 5350 7950 5350
Wire Wire Line
	7950 5350 7950 5650
$Comp
L preprocessor-multi-aux-rescue:C-preprocessor-rescue C2
U 1 1 58AFA5B0
P 9650 5400
F 0 "C2" H 9675 5500 50  0000 L CNN
F 1 "47u/6V3 Polymer 0805" H 9675 5300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9688 5250 50  0001 C CNN
F 3 "" H 9650 5400 50  0000 C CNN
	1    9650 5400
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR08
U 1 1 58AFA5F4
P 9650 5050
F 0 "#PWR08" H 9650 4900 50  0001 C CNN
F 1 "+3V3" H 9650 5190 50  0000 C CNN
F 2 "" H 9650 5050 50  0000 C CNN
F 3 "" H 9650 5050 50  0000 C CNN
	1    9650 5050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 58AFA6CA
P 9650 5650
F 0 "#PWR09" H 9650 5400 50  0001 C CNN
F 1 "GND" H 9650 5500 50  0000 C CNN
F 2 "" H 9650 5650 50  0000 C CNN
F 3 "" H 9650 5650 50  0000 C CNN
	1    9650 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 5550 9650 5650
Wire Wire Line
	9650 5050 9650 5150
Wire Wire Line
	9450 5150 9650 5150
Connection ~ 9650 5150
Wire Wire Line
	3550 3150 3550 3250
Wire Wire Line
	3550 1950 3550 2050
Wire Wire Line
	3550 2050 3650 2050
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
P 7900 2250
F 0 "#PWR010" H 7900 2100 50  0001 C CNN
F 1 "VCC" H 7900 2400 50  0000 C CNN
F 2 "" H 7900 2250 50  0000 C CNN
F 3 "" H 7900 2250 50  0000 C CNN
	1    7900 2250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 58AFADBD
P 7900 2750
F 0 "#PWR011" H 7900 2500 50  0001 C CNN
F 1 "GND" H 7900 2600 50  0000 C CNN
F 2 "" H 7900 2750 50  0000 C CNN
F 3 "" H 7900 2750 50  0000 C CNN
	1    7900 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 2550 7900 2550
Wire Wire Line
	7900 2550 7900 2250
Wire Wire Line
	8000 2650 7900 2650
Wire Wire Line
	7900 2650 7900 2750
Wire Wire Line
	1850 1800 2450 1800
Wire Wire Line
	3050 1800 3050 2250
Wire Wire Line
	1850 2800 2450 2800
Wire Wire Line
	3050 2800 3050 2650
Wire Wire Line
	7200 2450 8000 2450
NoConn ~ 6000 3150
NoConn ~ 6000 2250
NoConn ~ 3650 2350
NoConn ~ 3650 2450
NoConn ~ 3650 2550
NoConn ~ 3650 2750
NoConn ~ 3650 2850
NoConn ~ 3650 2950
Wire Wire Line
	6300 3750 6300 2550
Wire Wire Line
	6300 2550 6000 2550
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
Wire Wire Line
	3650 3150 3550 3150
Text Label 2650 3750 0    60   ~ 0
AUX
Text Label 2650 2800 0    60   ~ 0
TH
Text Label 2650 1800 0    60   ~ 0
ST
Text Label 7450 2450 0    60   ~ 0
LIGHT
Text Label 9650 5150 0    60   ~ 0
3V3
Text Label 7750 4900 0    60   ~ 0
VCC
$Comp
L preprocessor-multi-aux-rescue:R-preprocessor-rescue R1
U 1 1 58C77CC2
P 3300 2250
F 0 "R1" V 3380 2250 50  0000 C CNN
F 1 "1K" V 3300 2250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3230 2250 50  0001 C CNN
F 3 "" H 3300 2250 50  0001 C CNN
	1    3300 2250
	0    1    1    0   
$EndComp
$Comp
L preprocessor-multi-aux-rescue:R-preprocessor-rescue R2
U 1 1 58C77D77
P 3300 2650
F 0 "R2" V 3380 2650 50  0000 C CNN
F 1 "1K" V 3300 2650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3230 2650 50  0001 C CNN
F 3 "" H 3300 2650 50  0001 C CNN
	1    3300 2650
	0    1    1    0   
$EndComp
$Comp
L preprocessor-multi-aux-rescue:R-preprocessor-rescue R3
U 1 1 58C77DC4
P 3300 3750
F 0 "R3" V 3380 3750 50  0000 C CNN
F 1 "1K" V 3300 3750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3230 3750 50  0001 C CNN
F 3 "" H 3300 3750 50  0001 C CNN
	1    3300 3750
	0    1    1    0   
$EndComp
$Comp
L preprocessor-multi-aux-rescue:R-preprocessor-rescue R4
U 1 1 58C77E14
P 7050 2450
F 0 "R4" V 7130 2450 50  0000 C CNN
F 1 "1K" V 7050 2450 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 6980 2450 50  0001 C CNN
F 3 "" H 7050 2450 50  0001 C CNN
	1    7050 2450
	0    1    1    0   
$EndComp
Wire Wire Line
	3050 2250 3150 2250
Wire Wire Line
	3450 2250 3650 2250
Wire Wire Line
	3050 2650 3150 2650
Wire Wire Line
	3450 2650 3650 2650
Wire Wire Line
	1850 3750 3150 3750
Wire Wire Line
	3450 3750 6300 3750
Wire Wire Line
	6900 2450 6000 2450
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
	7750 5150 7750 5200
Wire Wire Line
	9650 5150 9650 5250
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
	2450 1800 3050 1800
Wire Wire Line
	2450 2800 3050 2800
Wire Wire Line
	2100 1350 2100 1900
Wire Wire Line
	1950 5500 1950 5550
$Comp
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P7
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
Text Label 2650 4250 0    60   ~ 0
AUX2
$Comp
L preprocessor-multi-aux-rescue:R-preprocessor-rescue R5
U 1 1 5D282BB8
P 3300 4250
F 0 "R5" V 3380 4250 50  0000 C CNN
F 1 "1K" V 3300 4250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3230 4250 50  0001 C CNN
F 3 "" H 3300 4250 50  0001 C CNN
	1    3300 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	1850 4250 3150 4250
Connection ~ 2100 3850
$Comp
L preprocessor-multi-aux-rescue:CONN_01X03-preprocessor-rescue P8
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
Text Label 2650 4750 0    60   ~ 0
AUX3
$Comp
L preprocessor-multi-aux-rescue:R-preprocessor-rescue R6
U 1 1 5D287722
P 3300 4750
F 0 "R6" V 3380 4750 50  0000 C CNN
F 1 "1K" V 3300 4750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3230 4750 50  0001 C CNN
F 3 "" H 3300 4750 50  0001 C CNN
	1    3300 4750
	0    1    1    0   
$EndComp
Wire Wire Line
	1850 4750 3150 4750
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
Wire Wire Line
	6550 2350 6000 2350
Wire Wire Line
	6000 3050 6450 3050
Wire Wire Line
	6450 3050 6450 4250
$Comp
L preprocessor-multi-aux-rescue:R-preprocessor-rescue R7
U 1 1 5D282842
P 7050 3600
F 0 "R7" V 7130 3600 50  0000 C CNN
F 1 "100k" V 7050 3600 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 6980 3600 50  0001 C CNN
F 3 "" H 7050 3600 50  0001 C CNN
	1    7050 3600
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5D284613
P 7050 3900
F 0 "#PWR012" H 7050 3650 50  0001 C CNN
F 1 "GND" H 7050 3750 50  0000 C CNN
F 2 "" H 7050 3900 50  0000 C CNN
F 3 "" H 7050 3900 50  0000 C CNN
	1    7050 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 3750 7050 3900
Wire Wire Line
	7050 3450 7050 2800
Wire Wire Line
	7050 2800 6550 2800
Connection ~ 6550 2800
Wire Wire Line
	6550 2800 6550 2350
Wire Wire Line
	3450 4250 6450 4250
Wire Wire Line
	6550 4750 3450 4750
Wire Wire Line
	6550 2800 6550 4750
$EndSCHEMATC
