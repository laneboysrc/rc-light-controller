EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "DIY RC Light Controller Mk4"
Date "2022-05-03"
Rev "7"
Comp "LANE Boys RC"
Comment1 "laneboysrc@gmail.com"
Comment2 "LPC832 TSSOP20 SBUS version"
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	11600 2100 11300 2100
Connection ~ 11300 2100
Wire Wire Line
	11600 2300 11300 2300
Wire Wire Line
	10900 4250 10900 4000
Wire Wire Line
	4000 3200 4000 2950
Wire Wire Line
	5100 2700 5100 3200
Wire Wire Line
	1550 4600 1550 2800
Wire Wire Line
	1550 2800 1450 2800
Wire Wire Line
	12300 3900 12300 4050
Wire Wire Line
	11150 9150 11150 9250
Text Label 9500 3500 0    70   ~ 0
SIN
Wire Wire Line
	5550 8400 6150 8400
Text Label 6150 8400 2    50   ~ 0
SIN
Text Label 9500 3400 0    70   ~ 0
SCLK
Text Label 3350 7900 0    50   ~ 0
SCLK
Text Label 9500 2700 0    70   ~ 0
XLAT
Text Label 3350 8000 0    50   ~ 0
XLAT
Wire Wire Line
	5550 8200 6150 8200
Text Label 6150 8200 2    50   ~ 0
BLANK
Text Label 9500 2400 0    70   ~ 0
GSCLK
Wire Wire Line
	3350 7800 3950 7800
Text Label 3350 7800 0    50   ~ 0
GSCLK
Wire Wire Line
	10900 3700 10900 2200
Wire Wire Line
	3950 8100 3350 8100
Text Label 3350 8100 0    50   ~ 0
TH-TX
Text Label 5050 4350 2    70   ~ 0
TH-TX
Wire Wire Line
	4000 2650 4000 2400
Connection ~ 4000 2400
Wire Wire Line
	14900 6800 14400 6800
Wire Wire Line
	14900 6500 14400 6500
Text Label 13650 6500 0    70   ~ 0
VIN
Text Label 1900 2400 0    50   ~ 0
VIN
Wire Wire Line
	3950 7700 3350 7700
Text Label 3350 7700 0    50   ~ 0
ST-RX
Text Label 5050 4050 2    70   ~ 0
ST-RX
Wire Wire Line
	13000 2100 14200 2100
Wire Wire Line
	14200 2100 14200 1700
Wire Wire Line
	13000 2200 14300 2200
Wire Wire Line
	14300 2200 14300 2000
Wire Wire Line
	14300 2000 14900 2000
Wire Wire Line
	13000 2300 14900 2300
Wire Wire Line
	13000 2400 14300 2400
Wire Wire Line
	14300 2400 14300 2600
Wire Wire Line
	14300 2600 14900 2600
Wire Wire Line
	13000 3600 13100 3600
Wire Wire Line
	13100 3600 13100 6200
Wire Wire Line
	13100 6200 14900 6200
Wire Wire Line
	13000 3500 13200 3500
Wire Wire Line
	13200 3500 13200 5900
Wire Wire Line
	13200 5900 14900 5900
Wire Wire Line
	13000 3400 13300 3400
Wire Wire Line
	13300 3400 13300 5600
Wire Wire Line
	13300 5600 14900 5600
Wire Wire Line
	13000 3300 13400 3300
Wire Wire Line
	13400 3300 13400 5300
Wire Wire Line
	13400 5300 14900 5300
Wire Wire Line
	13000 3200 13500 3200
Wire Wire Line
	13500 3200 13500 5000
Wire Wire Line
	13500 5000 14900 5000
Wire Wire Line
	13000 3100 13600 3100
Wire Wire Line
	13600 3100 13600 4700
Wire Wire Line
	13600 4700 14900 4700
Wire Wire Line
	13000 3000 13700 3000
Wire Wire Line
	13700 3000 13700 4400
Wire Wire Line
	13700 4400 14900 4400
Wire Wire Line
	13000 2900 13800 2900
Wire Wire Line
	13800 2900 13800 4100
Wire Wire Line
	13800 4100 14900 4100
Wire Wire Line
	13000 2800 13900 2800
Wire Wire Line
	13900 2800 13900 3800
Wire Wire Line
	13900 3800 14900 3800
Wire Wire Line
	13000 2700 14000 2700
Wire Wire Line
	14000 2700 14000 3500
Wire Wire Line
	14000 3500 14900 3500
Wire Wire Line
	13000 2600 14100 2600
Wire Wire Line
	14100 2600 14100 3200
Wire Wire Line
	13000 2500 14200 2500
Wire Wire Line
	14200 2500 14200 2900
Wire Wire Line
	14200 2900 14900 2900
Wire Wire Line
	6150 7900 5550 7900
Text Label 6150 7900 2    50   ~ 0
OUT-ISP
Text Label 5050 4250 2    70   ~ 0
OUT-ISP
Text Label 5050 4150 2    70   ~ 0
AUX
Wire Wire Line
	6150 8000 5550 8000
Text Label 6150 8000 2    50   ~ 0
AUX
Wire Wire Line
	11950 8450 11150 8450
Wire Wire Line
	11150 8750 11150 8450
Wire Wire Line
	3950 8400 2950 8400
Text Label 2950 8400 0    50   ~ 0
OUT-SWITCHED
Wire Wire Line
	10850 8950 9750 8950
Text Label 9750 8950 0    70   ~ 0
OUT-SWITCHED
$Comp
L Driver_LED:TLC5940PWP U3
U 1 1 840783A6
P 12300 2800
F 0 "U3" H 11850 3650 59  0000 L BNN
F 1 "TLC5940PWP" H 13000 3750 59  0000 R TNN
F 2 "rc-light-controller-tlc5940-lpc812:SOP65P640X120-29N" H 12300 2800 50  0001 C CNN
F 3 "" H 12300 2800 50  0001 C CNN
	1    12300 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 80E41320
P 10900 3850
F 0 "R1" H 10650 3750 59  0000 L BNN
F 1 "2k0=20mA" H 10300 3850 59  0000 L BNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 10900 3850 50  0001 C CNN
F 3 "" H 10900 3850 50  0001 C CNN
	1    10900 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 4A30DC6E
P 3250 7100
F 0 "C3" H 3400 7100 59  0000 L BNN
F 1 "100n" H 3310 6915 59  0000 L BNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3250 7100 50  0001 C CNN
F 3 "" H 3250 7100 50  0001 C CNN
	1    3250 7100
	-1   0    0    1   
$EndComp
$Comp
L Regulator_Linear:MCP1703A-3302_SOT23 U1
U 1 1 09B5DC2D
P 5100 2400
F 0 "U1" H 4900 2550 69  0000 L BNN
F 1 "3V3" H 5200 2050 69  0000 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23W" H 5100 2400 50  0001 C CNN
F 3 "" H 5100 2400 50  0001 C CNN
	1    5100 2400
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 PAD2
U 1 1 A0CD53A2
P 1250 2800
F 0 "PAD2" H 1205 2873 59  0000 L BNN
F 1 "-" H 1205 2670 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1250 2800 50  0001 C CNN
F 3 "" H 1250 2800 50  0001 C CNN
	1    1250 2800
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 PAD3
U 1 1 1CFA7EBC
P 1250 3200
F 0 "PAD3" H 1205 3273 59  0000 L BNN
F 1 "ST/Rx" H 1205 3070 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1250 3200 50  0001 C CNN
F 3 "" H 1250 3200 50  0001 C CNN
	1    1250 3200
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 PAD4
U 1 1 9066C7CA
P 1250 3600
F 0 "PAD4" H 1205 3673 59  0000 L BNN
F 1 "TH/Tx" H 1205 3470 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1250 3600 50  0001 C CNN
F 3 "" H 1250 3600 50  0001 C CNN
	1    1250 3600
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 PAD6
U 1 1 6D080C98
P 1250 4400
F 0 "PAD6" H 1205 4473 59  0000 L BNN
F 1 "OUT/ISP" H 1205 4270 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1250 4400 50  0001 C CNN
F 3 "" H 1250 4400 50  0001 C CNN
	1    1250 4400
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT0
U 1 1 3C53439C
P 15100 1700
F 0 "OUT0" H 15055 1773 59  0000 L BNN
F 1 "SMD50X100" H 15055 1570 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 1700 50  0001 C CNN
F 3 "" H 15100 1700 50  0001 C CNN
	1    15100 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT2
U 1 1 861A1973
P 15100 2300
F 0 "OUT2" H 15055 2373 59  0000 L BNN
F 1 "SMD50X100" H 15055 2170 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 2300 50  0001 C CNN
F 3 "" H 15100 2300 50  0001 C CNN
	1    15100 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT3
U 1 1 FBBF8CFD
P 15100 2600
F 0 "OUT3" H 15055 2673 59  0000 L BNN
F 1 "SMD50X100" H 15055 2470 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 2600 50  0001 C CNN
F 3 "" H 15100 2600 50  0001 C CNN
	1    15100 2600
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT4
U 1 1 F2672D97
P 15100 2900
F 0 "OUT4" H 15055 2973 59  0000 L BNN
F 1 "SMD50X100" H 15055 2770 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 2900 50  0001 C CNN
F 3 "" H 15100 2900 50  0001 C CNN
	1    15100 2900
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT5
U 1 1 A0BFA14C
P 15100 3200
F 0 "OUT5" H 15055 3273 59  0000 L BNN
F 1 "SMD50X100" H 15055 3070 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 3200 50  0001 C CNN
F 3 "" H 15100 3200 50  0001 C CNN
	1    15100 3200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT6
U 1 1 F2728259
P 15100 3500
F 0 "OUT6" H 15055 3573 59  0000 L BNN
F 1 "SMD50X100" H 15055 3370 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 3500 50  0001 C CNN
F 3 "" H 15100 3500 50  0001 C CNN
	1    15100 3500
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT7
U 1 1 F2054039
P 15100 3800
F 0 "OUT7" H 15055 3873 59  0000 L BNN
F 1 "SMD50X100" H 15055 3670 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 3800 50  0001 C CNN
F 3 "" H 15100 3800 50  0001 C CNN
	1    15100 3800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT8
U 1 1 D45D03D4
P 15100 4100
F 0 "OUT8" H 15055 4173 59  0000 L BNN
F 1 "SMD50X100" H 15055 3970 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 4100 50  0001 C CNN
F 3 "" H 15100 4100 50  0001 C CNN
	1    15100 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT9
U 1 1 8C60748A
P 15100 4400
F 0 "OUT9" H 15055 4473 59  0000 L BNN
F 1 "SMD50X100" H 15055 4270 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 4400 50  0001 C CNN
F 3 "" H 15100 4400 50  0001 C CNN
	1    15100 4400
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT10
U 1 1 C996D4E7
P 15100 4700
F 0 "OUT10" H 15055 4773 59  0000 L BNN
F 1 "SMD50X100" H 15055 4570 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 4700 50  0001 C CNN
F 3 "" H 15100 4700 50  0001 C CNN
	1    15100 4700
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT11
U 1 1 7CE6024B
P 15100 5000
F 0 "OUT11" H 15055 5073 59  0000 L BNN
F 1 "SMD50X100" H 15055 4870 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 5000 50  0001 C CNN
F 3 "" H 15100 5000 50  0001 C CNN
	1    15100 5000
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT12
U 1 1 DB29FCCE
P 15100 5300
F 0 "OUT12" H 15055 5373 59  0000 L BNN
F 1 "SMD50X100" H 15055 5170 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 5300 50  0001 C CNN
F 3 "" H 15100 5300 50  0001 C CNN
	1    15100 5300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT13
U 1 1 BB8CF4E9
P 15100 5600
F 0 "OUT13" H 15055 5673 59  0000 L BNN
F 1 "SMD50X100" H 15055 5470 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 5600 50  0001 C CNN
F 3 "" H 15100 5600 50  0001 C CNN
	1    15100 5600
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT14
U 1 1 DB03A36D
P 15100 5900
F 0 "OUT14" H 15055 5973 59  0000 L BNN
F 1 "SMD50X100" H 15055 5770 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 5900 50  0001 C CNN
F 3 "" H 15100 5900 50  0001 C CNN
	1    15100 5900
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT15
U 1 1 C8234427
P 15100 6200
F 0 "OUT15" H 15055 6273 59  0000 L BNN
F 1 "SMD50X100" H 15055 6070 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 6200 50  0001 C CNN
F 3 "" H 15100 6200 50  0001 C CNN
	1    15100 6200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 LED+2
U 1 1 2268DE62
P 15100 6800
F 0 "LED+2" H 15055 6873 59  0000 L BNN
F 1 "SMD50X100" H 15055 6670 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 6800 50  0001 C CNN
F 3 "" H 15100 6800 50  0001 C CNN
	1    15100 6800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 LED+1
U 1 1 CFCDD87C
P 15100 6500
F 0 "LED+1" H 15055 6573 59  0000 L BNN
F 1 "SMD50X100" H 15055 6370 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 6500 50  0001 C CNN
F 3 "" H 15100 6500 50  0001 C CNN
	1    15100 6500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 30D010B6
P 4000 2800
F 0 "C1" H 4100 2900 59  0000 L BNN
F 1 "1u/16V" H 4060 2615 59  0000 L BNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4000 2800 50  0001 C CNN
F 3 "" H 4000 2800 50  0001 C CNN
	1    4000 2800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 PAD5
U 1 1 411DFD1A
P 1250 4000
F 0 "PAD5" H 1205 4073 59  0000 L BNN
F 1 "CH3" H 1205 3870 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1250 4000 50  0001 C CNN
F 3 "" H 1250 4000 50  0001 C CNN
	1    1250 4000
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 OUT15S1
U 1 1 1D8B1813
P 12150 8450
F 0 "OUT15S1" H 12105 8523 59  0000 L BNN
F 1 "SMD50X100" H 12105 8320 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 12150 8450 50  0001 C CNN
F 3 "" H 12150 8450 50  0001 C CNN
	1    12150 8450
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q2
U 1 1 8692C711
P 11050 8950
F 0 "Q2" H 11250 9000 59  0000 L BNN
F 1 "NMOSSOT23" H 11150 8950 59  0001 L BNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 11050 8950 50  0001 C CNN
F 3 "" H 11050 8950 50  0001 C CNN
	1    11050 8950
	1    0    0    -1  
$EndComp
Text Notes 3150 9850 0    56   ~ 0
Special pins:\nPIO0_0  (16)   ISP UART RX\nPIO0_4  ( 4)   ISP UART TX\nPIO0_5  ( 3)   RESET\nPIO0_10 ( 8)   Open drain\nPIO0_11 ( 7)   Open drain\nPIO0_2  ( 6)   SWDIO\nPIO0_3  ( 5)   SWCLK
Text Notes 1050 2000 0    85   ~ 0
Servo in/out
Text Notes 11200 1350 0    85   ~ 0
LED driver and outputs
Text Notes 4400 1600 0    85   ~ 0
Voltage regulator
Text Notes 4400 6350 0    85   ~ 0
Microcontroller
Text Notes 3650 3000 0    59   ~ 0
X7R
Text Notes 1900 2600 0    59   ~ 0
Input voltage range:\n4 ... 10V
Text Notes 4600 2100 0    59   ~ 0
LDO: \nMCP1702, MCP1703\nME6209A33M3G
Text Notes 3300 7250 0    59   ~ 0
X5R or X7R
Text Notes 5950 9100 0    59   ~ 0
NXP LPC832\nARM Cortex-M0\n16K Flash, 4K RAM\nTSSOP-20
Text Notes 10200 8250 0    85   ~ 0
Switched output\nfor light bar or similar
Text Notes 11550 9350 0    56   ~ 0
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
Wire Wire Line
	3350 8000 3950 8000
Wire Wire Line
	3350 7900 3950 7900
$Comp
L power:GND #PWR0101
U 1 1 5CCABD51
P 1550 4600
F 0 "#PWR0101" H 1550 4350 50  0001 C CNN
F 1 "GND" H 1555 4427 50  0000 C CNN
F 2 "" H 1550 4600 50  0001 C CNN
F 3 "" H 1550 4600 50  0001 C CNN
	1    1550 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5CCABE6A
P 3250 7300
F 0 "#PWR0102" H 3250 7050 50  0001 C CNN
F 1 "GND" H 3255 7127 50  0000 C CNN
F 2 "" H 3250 7300 50  0001 C CNN
F 3 "" H 3250 7300 50  0001 C CNN
	1    3250 7300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5CCAC0F7
P 4850 9200
F 0 "#PWR0103" H 4850 8950 50  0001 C CNN
F 1 "GND" H 4855 9027 50  0000 C CNN
F 2 "" H 4850 9200 50  0001 C CNN
F 3 "" H 4850 9200 50  0001 C CNN
	1    4850 9200
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
P 5100 3200
F 0 "#PWR0105" H 5100 2950 50  0001 C CNN
F 1 "GND" H 5105 3027 50  0000 C CNN
F 2 "" H 5100 3200 50  0001 C CNN
F 3 "" H 5100 3200 50  0001 C CNN
	1    5100 3200
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
$Comp
L power:GND #PWR0107
U 1 1 5CCACD7C
P 11150 9250
F 0 "#PWR0107" H 11150 9000 50  0001 C CNN
F 1 "GND" H 11155 9077 50  0000 C CNN
F 2 "" H 11150 9250 50  0001 C CNN
F 3 "" H 11150 9250 50  0001 C CNN
	1    11150 9250
	1    0    0    -1  
$EndComp
Wire Wire Line
	14400 6800 14400 6500
Connection ~ 14400 6500
Wire Wire Line
	14400 6500 13650 6500
$Comp
L power:GND #PWR0108
U 1 1 5CCBD906
P 10900 4250
F 0 "#PWR0108" H 10900 4000 50  0001 C CNN
F 1 "GND" H 10905 4077 50  0000 C CNN
F 2 "" H 10900 4250 50  0001 C CNN
F 3 "" H 10900 4250 50  0001 C CNN
	1    10900 4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 5CCBD9C4
P 12300 4200
F 0 "#PWR0110" H 12300 3950 50  0001 C CNN
F 1 "GND" H 12305 4027 50  0000 C CNN
F 2 "" H 12300 4200 50  0001 C CNN
F 3 "" H 12300 4200 50  0001 C CNN
	1    12300 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 6650 4850 6950
Connection ~ 4850 6950
Wire Wire Line
	3250 7250 3250 7300
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
Wire Wire Line
	14200 1700 14900 1700
$Comp
L Connector_Generic:Conn_01x01 OUT1
U 1 1 6BADA7B7
P 15100 2000
F 0 "OUT1" H 15055 2073 59  0000 L BNN
F 1 "SMD50X100" H 15055 1870 59  0001 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD50X100" H 15100 2000 50  0001 C CNN
F 3 "" H 15100 2000 50  0001 C CNN
	1    15100 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	12200 3900 12200 4050
Wire Wire Line
	12200 4050 12300 4050
Connection ~ 12300 4050
Wire Wire Line
	12300 4050 12300 4200
Wire Wire Line
	5400 2400 6000 2400
Wire Wire Line
	6000 2100 6000 2400
Wire Wire Line
	4000 2400 4800 2400
Wire Wire Line
	4850 6950 4850 7300
Wire Wire Line
	4850 8900 4850 9050
Wire Wire Line
	14100 3200 14900 3200
$Comp
L Connector_Generic:Conn_01x01 PAD1
U 1 1 1C475C6D
P 1250 2400
F 0 "PAD1" H 1205 2473 59  0000 L BNN
F 1 "+" H 1205 2270 59  0000 L BNN
F 2 "rc-light-controller-tlc5940-lpc812:SMD80X120" H 1250 2400 50  0001 C CNN
F 3 "" H 1250 2400 50  0001 C CNN
	1    1250 2400
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1450 2400 4000 2400
Wire Wire Line
	12300 1700 12300 1800
Wire Wire Line
	10900 2200 11600 2200
Wire Wire Line
	11300 2300 11300 2100
Text Label 9500 2600 0    70   ~ 0
BLANK
Wire Wire Line
	11300 1700 11300 2100
NoConn ~ 11600 3000
NoConn ~ 11600 3600
Wire Wire Line
	9500 3500 11600 3500
Wire Wire Line
	9500 2400 11600 2400
Wire Wire Line
	9500 3400 11600 3400
Wire Wire Line
	9500 2700 11600 2700
Wire Wire Line
	9500 2600 11600 2600
Wire Wire Line
	1450 4000 2600 4000
Wire Wire Line
	1450 3600 2800 3600
Wire Wire Line
	3050 3200 3050 4050
Wire Wire Line
	3050 4050 3650 4050
Wire Wire Line
	1450 3200 3050 3200
Wire Wire Line
	2800 4350 3650 4350
Wire Wire Line
	2800 3600 2800 4350
Wire Wire Line
	2600 4150 2600 4000
Wire Wire Line
	2600 4150 3650 4150
Wire Wire Line
	3650 4250 2600 4250
Wire Wire Line
	2600 4250 2600 4400
Wire Wire Line
	2600 4400 1450 4400
Text Label 10950 2200 0    50   ~ 0
IREF
Text Label 14400 1700 0    50   ~ 0
OUT0
Text Label 14400 2000 0    50   ~ 0
OUT1
Text Label 14400 2300 0    50   ~ 0
OUT2
Text Label 14400 2600 0    50   ~ 0
OUT3
Text Label 14400 2900 0    50   ~ 0
OUT4
Text Label 14400 3200 0    50   ~ 0
OUT5
Text Label 14400 3500 0    50   ~ 0
OUT6
Text Label 14400 3800 0    50   ~ 0
OUT7
Text Label 14400 4100 0    50   ~ 0
OUT8
Text Label 14400 4400 0    50   ~ 0
OUT9
Text Label 14400 4700 0    50   ~ 0
OUT10
Text Label 14400 5000 0    50   ~ 0
OUT11
Text Label 14400 5300 0    50   ~ 0
OUT12
Text Label 14400 5600 0    50   ~ 0
OUT13
Text Label 14400 5900 0    50   ~ 0
OUT14
Text Label 14400 6200 0    50   ~ 0
OUT15
Text Label 11400 8450 0    50   ~ 0
OUT15S
Text Label 1900 3200 0    50   ~ 0
ST-IN
Text Label 1900 3600 0    50   ~ 0
TH-IN
Text Label 1900 4000 0    50   ~ 0
AUX-IN
Text Label 1900 4400 0    50   ~ 0
OUT-ISP-OUT
$Comp
L power:+3V3 #PWR0109
U 1 1 60242FCF
P 6000 2100
F 0 "#PWR0109" H 6000 1950 50  0001 C CNN
F 1 "+3V3" H 6015 2273 50  0000 C CNN
F 2 "" H 6000 2100 50  0001 C CNN
F 3 "" H 6000 2100 50  0001 C CNN
	1    6000 2100
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0112
U 1 1 6024C267
P 11300 1700
F 0 "#PWR0112" H 11300 1550 50  0001 C CNN
F 1 "+3V3" H 11315 1873 50  0000 C CNN
F 2 "" H 11300 1700 50  0001 C CNN
F 3 "" H 11300 1700 50  0001 C CNN
	1    11300 1700
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0113
U 1 1 6024D34E
P 12300 1700
F 0 "#PWR0113" H 12300 1550 50  0001 C CNN
F 1 "+3V3" H 12315 1873 50  0000 C CNN
F 2 "" H 12300 1700 50  0001 C CNN
F 3 "" H 12300 1700 50  0001 C CNN
	1    12300 1700
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0114
U 1 1 6025D63E
P 4850 6650
F 0 "#PWR0114" H 4850 6500 50  0001 C CNN
F 1 "+3V3" H 4865 6823 50  0000 C CNN
F 2 "" H 4850 6650 50  0001 C CNN
F 3 "" H 4850 6650 50  0001 C CNN
	1    4850 6650
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 602CC5AA
P 4000 2300
F 0 "#FLG0102" H 4000 2375 50  0001 C CNN
F 1 "PWR_FLAG" H 4000 2474 50  0000 C CNN
F 2 "" H 4000 2300 50  0001 C CNN
F 3 "~" H 4000 2300 50  0001 C CNN
	1    4000 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 2300 4000 2400
$Comp
L MCU_NXP_LPC:LPC832M101FDH20 U2
U 1 1 609FA9C6
P 4750 8100
F 0 "U2" H 4200 8750 50  0000 C CNN
F 1 "LPC832M101FDH20" H 5300 8750 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 5800 8850 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/LPC83X.pdf" H 4900 8750 50  0001 L CNN
	1    4750 8100
	1    0    0    -1  
$EndComp
Text Notes 5950 9800 0    50   ~ 0
Pin assignment is similar to LPC812,\nwith the following exceptions:\nBLANK is PIO0_15 instesad of PIO0_6\nSIN is PIO0_23 instesad of PIO0_7
NoConn ~ 3950 8200
NoConn ~ 3950 8300
NoConn ~ 5550 7800
NoConn ~ 5550 8100
NoConn ~ 5550 8300
Wire Wire Line
	4650 7300 4650 6950
Wire Wire Line
	3250 6950 4650 6950
Connection ~ 4650 6950
Wire Wire Line
	4650 6950 4850 6950
Wire Wire Line
	4650 8900 4650 9050
Wire Wire Line
	4650 9050 4850 9050
Connection ~ 4850 9050
Wire Wire Line
	4850 9050 4850 9200
$Comp
L Transistor_FET:2N7002K Q1
U 1 1 6270D110
P 8350 6450
F 0 "Q1" H 8555 6496 50  0000 L CNN
F 1 "2N7002K" H 8555 6405 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8550 6375 50  0001 L CIN
F 3 "https://www.diodes.com/assets/Datasheets/ds30896.pdf" H 8350 6450 50  0001 L CNN
	1    8350 6450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9100 6450 8550 6450
$Comp
L power:GND #PWR02
U 1 1 62714706
P 8250 6850
F 0 "#PWR02" H 8250 6600 50  0001 C CNN
F 1 "GND" H 8255 6677 50  0000 C CNN
F 2 "" H 8250 6850 50  0001 C CNN
F 3 "" H 8250 6850 50  0001 C CNN
	1    8250 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 6650 8250 6850
Text Label 9100 6450 2    70   ~ 0
ST-RX
$Comp
L Device:R R2
U 1 1 6271B2F8
P 8250 5900
F 0 "R2" H 8100 5850 50  0000 C CNN
F 1 "10k" H 8100 6000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8180 5900 50  0001 C CNN
F 3 "~" H 8250 5900 50  0001 C CNN
	1    8250 5900
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR01
U 1 1 6271B9C4
P 8250 5600
F 0 "#PWR01" H 8250 5450 50  0001 C CNN
F 1 "+3V3" H 8265 5773 50  0000 C CNN
F 2 "" H 8250 5600 50  0001 C CNN
F 3 "" H 8250 5600 50  0001 C CNN
	1    8250 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 5600 8250 5750
Wire Wire Line
	8250 6050 8250 6150
Wire Wire Line
	6650 7700 6650 6150
Wire Wire Line
	6650 6150 8250 6150
Connection ~ 8250 6150
Wire Wire Line
	8250 6150 8250 6250
Text Label 6150 7700 2    50   ~ 0
SBUS
$Comp
L Device:R_Pack04 RN1
U 1 1 6273657D
P 3850 4250
F 0 "RN1" V 3433 4250 50  0000 C CNN
F 1 "1k x4" V 3524 4250 50  0000 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 4125 4250 50  0001 C CNN
F 3 "~" H 3850 4250 50  0001 C CNN
	1    3850 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	4050 4350 5050 4350
Wire Wire Line
	4050 4250 5050 4250
Wire Wire Line
	4050 4150 5050 4150
Wire Wire Line
	4050 4050 5050 4050
Text Notes 8700 6850 0    50   ~ 0
Important: this MOSFET needs to have\na low gate capacitance (~20pf), as otherwise\nit will distort the UART / Servo signal
Wire Wire Line
	5550 7700 6650 7700
Text Notes 7800 5250 0    85   ~ 0
S.BUS inverter
$EndSCHEMATC
