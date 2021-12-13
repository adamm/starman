EESchema Schematic File Version 4
EELAYER 30 0
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
L Device:R R5
U 1 1 61B56CE7
P 6650 2650
F 0 "R5" H 6720 2696 50  0000 L CNN
F 1 "100" H 6720 2605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6580 2650 50  0001 C CNN
F 3 "~" H 6650 2650 50  0001 C CNN
	1    6650 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 61B5804B
P 6650 2150
F 0 "D3" V 6689 2033 50  0000 R CNN
F 1 "LED" V 6598 2033 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 6650 2150 50  0001 C CNN
F 3 "~" H 6650 2150 50  0001 C CNN
	1    6650 2150
	0    -1   -1   0   
$EndComp
$Comp
L Device:Speaker LS1
U 1 1 61B5B45D
P 8200 1750
F 0 "LS1" H 8370 1746 50  0000 L CNN
F 1 "Speaker" H 8370 1655 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 8200 1550 50  0001 C CNN
F 3 "~" H 8190 1700 50  0001 C CNN
	1    8200 1750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 61B5D969
P 6650 1750
F 0 "R2" H 6580 1796 50  0000 R CNN
F 1 "75" H 6580 1705 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6580 1750 50  0001 C CNN
F 3 "~" H 6650 1750 50  0001 C CNN
	1    6650 1750
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6650 1600 6650 1500
Wire Wire Line
	6650 1500 6950 1500
Wire Wire Line
	6650 1900 6650 2000
$Comp
L Device:LED D4
U 1 1 61B61BCE
P 6950 2150
F 0 "D4" V 6989 2033 50  0000 R CNN
F 1 "LED" V 6898 2033 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 6950 2150 50  0001 C CNN
F 3 "~" H 6950 2150 50  0001 C CNN
	1    6950 2150
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 61B61BD4
P 6950 1750
F 0 "R3" H 6880 1796 50  0000 R CNN
F 1 "75" H 6880 1705 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6880 1750 50  0001 C CNN
F 3 "~" H 6950 1750 50  0001 C CNN
	1    6950 1750
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6950 1900 6950 2000
$Comp
L Device:LED D5
U 1 1 61B627D1
P 7250 2150
F 0 "D5" V 7289 2033 50  0000 R CNN
F 1 "LED" V 7198 2033 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 7250 2150 50  0001 C CNN
F 3 "~" H 7250 2150 50  0001 C CNN
	1    7250 2150
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R4
U 1 1 61B627D7
P 7250 1750
F 0 "R4" H 7180 1704 50  0000 R CNN
F 1 "75" H 7180 1795 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 7180 1750 50  0001 C CNN
F 3 "~" H 7250 1750 50  0001 C CNN
	1    7250 1750
	-1   0    0    1   
$EndComp
Wire Wire Line
	7250 1900 7250 2000
$Comp
L Device:LED D2
U 1 1 61B643C0
P 6350 2150
F 0 "D2" V 6389 2033 50  0000 R CNN
F 1 "LED" V 6298 2033 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 6350 2150 50  0001 C CNN
F 3 "~" H 6350 2150 50  0001 C CNN
	1    6350 2150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6350 1900 6350 2000
Wire Wire Line
	7250 1600 7250 1500
Wire Wire Line
	6950 1600 6950 1500
Connection ~ 6950 1500
Wire Wire Line
	6950 1500 7250 1500
Wire Wire Line
	6350 1600 6350 1500
Wire Wire Line
	6350 1500 6650 1500
Connection ~ 6650 1500
Wire Wire Line
	6350 2300 6350 2400
Wire Wire Line
	6350 2400 6650 2400
Wire Wire Line
	6650 2300 6650 2400
Connection ~ 6650 2400
Wire Wire Line
	6950 2400 6950 2300
Wire Wire Line
	7250 2300 7250 2400
Wire Wire Line
	7250 2400 6950 2400
Connection ~ 6950 2400
$Comp
L power:GND #PWR09
U 1 1 61B67D27
P 6650 2900
F 0 "#PWR09" H 6650 2650 50  0001 C CNN
F 1 "GND" H 6655 2727 50  0000 C CNN
F 2 "" H 6650 2900 50  0001 C CNN
F 3 "" H 6650 2900 50  0001 C CNN
	1    6650 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 61B67F6A
P 7900 1950
F 0 "#PWR06" H 7900 1700 50  0001 C CNN
F 1 "GND" H 7905 1777 50  0000 C CNN
F 2 "" H 7900 1950 50  0001 C CNN
F 3 "" H 7900 1950 50  0001 C CNN
	1    7900 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 1950 7900 1850
Wire Wire Line
	7900 1850 8000 1850
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J4
U 1 1 61B6A402
P 5600 3350
F 0 "J4" H 5650 3667 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 5650 3576 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 5600 3350 50  0001 C CNN
F 3 "~" H 5600 3350 50  0001 C CNN
	1    5600 3350
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:LF50_TO220 U1
U 1 1 61B6EF8A
P 3700 1400
F 0 "U1" H 3700 1642 50  0000 C CNN
F 1 "LF50_TO220" H 3700 1551 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 3700 1625 50  0001 C CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/c4/0e/7e/2a/be/bc/4c/bd/CD00000546.pdf/files/CD00000546.pdf/jcr:content/translations/en.CD00000546.pdf" H 3700 1350 50  0001 C CNN
	1    3700 1400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 61B73277
P 4050 1350
F 0 "#PWR01" H 4050 1200 50  0001 C CNN
F 1 "+5V" H 4065 1523 50  0000 C CNN
F 2 "" H 4050 1350 50  0001 C CNN
F 3 "" H 4050 1350 50  0001 C CNN
	1    4050 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 1400 4050 1400
$Comp
L power:GND #PWR04
U 1 1 61B7A7C8
P 3700 1900
F 0 "#PWR04" H 3700 1650 50  0001 C CNN
F 1 "GND" H 3705 1727 50  0000 C CNN
F 2 "" H 3700 1900 50  0001 C CNN
F 3 "" H 3700 1900 50  0001 C CNN
	1    3700 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Barrel_Jack_Switch J1
U 1 1 61B7B05D
P 2450 1500
F 0 "J1" H 2507 1817 50  0000 C CNN
F 1 "Barrel_Jack_MountingPin" H 2507 1726 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 2500 1460 50  0001 C CNN
F 3 "~" H 2500 1460 50  0001 C CNN
	1    2450 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 61B7DB5C
P 2800 1700
F 0 "#PWR03" H 2800 1450 50  0001 C CNN
F 1 "GND" H 2805 1527 50  0000 C CNN
F 2 "" H 2800 1700 50  0001 C CNN
F 3 "" H 2800 1700 50  0001 C CNN
	1    2800 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 1600 2800 1600
Wire Wire Line
	2800 1600 2800 1700
$Comp
L Device:R_POT RV1
U 1 1 61B8AD6A
P 7750 1750
F 0 "RV1" H 7680 1796 50  0000 R CNN
F 1 "10k" H 7680 1705 50  0000 R CNN
F 2 "Potentiometer_THT:Potentiometer_Vishay_T73YP_Vertical" H 7750 1750 50  0001 C CNN
F 3 "~" H 7750 1750 50  0001 C CNN
	1    7750 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 1750 8000 1750
Wire Wire Line
	7250 1500 7750 1500
Wire Wire Line
	7750 1500 7750 1600
Connection ~ 7250 1500
NoConn ~ 7750 1900
$Comp
L Device:R R1
U 1 1 61B643C6
P 6350 1750
F 0 "R1" H 6280 1796 50  0000 R CNN
F 1 "75" H 6280 1705 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 6280 1750 50  0001 C CNN
F 3 "~" H 6350 1750 50  0001 C CNN
	1    6350 1750
	-1   0    0    -1  
$EndComp
Text Label 6350 2000 2    50   ~ 0
CHAN1
Text Label 6650 2000 2    50   ~ 0
CHAN2
Text Label 6950 2000 2    50   ~ 0
CHAN3
Text Label 7250 2000 2    50   ~ 0
CHAN4
Text Label 4150 3300 0    50   ~ 0
CHAN4
$Comp
L Connector_Generic:Conn_01x06 J5
U 1 1 61B94F2D
P 5650 4050
F 0 "J5" H 5730 4042 50  0000 L CNN
F 1 "Conn_01x06" H 5730 3951 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 5650 4050 50  0001 C CNN
F 3 "~" H 5650 4050 50  0001 C CNN
	1    5650 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 61B97343
P 6050 2150
F 0 "D1" V 6089 2033 50  0000 R CNN
F 1 "LED" V 5998 2033 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 6050 2150 50  0001 C CNN
F 3 "~" H 6050 2150 50  0001 C CNN
	1    6050 2150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6650 2400 6950 2400
Wire Wire Line
	6650 2800 6650 2850
Wire Wire Line
	6650 2400 6650 2500
Text Label 6050 2000 2    50   ~ 0
STATUS
Text Label 4150 2900 0    50   ~ 0
TXD
Text Label 4150 3000 0    50   ~ 0
RXD
Text Label 4150 3200 0    50   ~ 0
STATUS
Text Label 2900 2700 2    50   ~ 0
EN
Text Label 4150 3900 0    50   ~ 0
BOOT
Text Label 4150 3400 0    50   ~ 0
CHAN3
Text Label 4150 3500 0    50   ~ 0
CHAN2
Text Label 4150 3700 0    50   ~ 0
BUTTON
Text Label 2900 3500 2    50   ~ 0
DATA
Text Label 2900 3400 2    50   ~ 0
CLOCK
Text Label 2900 3300 2    50   ~ 0
DISABLE
Text Label 2900 3200 2    50   ~ 0
LATCH
Wire Wire Line
	4050 1350 4050 1400
Text Label 5450 4350 2    50   ~ 0
LATCH
Text Label 5450 4250 2    50   ~ 0
DISABLE
Text Label 5450 4150 2    50   ~ 0
CLOCK
Text Label 5450 4050 2    50   ~ 0
DATA
$Comp
L power:GND #PWR013
U 1 1 61BAB308
P 5450 3950
F 0 "#PWR013" H 5450 3700 50  0001 C CNN
F 1 "GND" V 5455 3822 50  0000 R CNN
F 2 "" H 5450 3950 50  0001 C CNN
F 3 "" H 5450 3950 50  0001 C CNN
	1    5450 3950
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR012
U 1 1 61BABCFB
P 5450 3850
F 0 "#PWR012" H 5450 3700 50  0001 C CNN
F 1 "+5V" V 5465 3978 50  0000 L CNN
F 2 "" H 5450 3850 50  0001 C CNN
F 3 "" H 5450 3850 50  0001 C CNN
	1    5450 3850
	0    -1   -1   0   
$EndComp
NoConn ~ 2750 1500
$Comp
L Switch:SW_Push SW2
U 1 1 61BAD73C
P 4150 5000
F 0 "SW2" H 4150 5285 50  0000 C CNN
F 1 "SW_Push" H 4150 5194 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 4150 5200 50  0001 C CNN
F 3 "~" H 4150 5200 50  0001 C CNN
	1    4150 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 61BAE9AD
P 4350 5000
F 0 "#PWR014" H 4350 4750 50  0001 C CNN
F 1 "GND" V 4355 4872 50  0000 R CNN
F 2 "" H 4350 5000 50  0001 C CNN
F 3 "" H 4350 5000 50  0001 C CNN
	1    4350 5000
	0    -1   -1   0   
$EndComp
Text Label 3700 5000 2    50   ~ 0
BUTTON
$Comp
L Switch:SW_SPST SW1
U 1 1 61BAF525
P 3150 1400
F 0 "SW1" H 3150 1635 50  0000 C CNN
F 1 "SW_SPST" H 3150 1544 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3150 1400 50  0001 C CNN
F 3 "~" H 3150 1400 50  0001 C CNN
	1    3150 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 1400 2950 1400
Wire Wire Line
	3350 1400 3400 1400
$Comp
L power:PWR_FLAG #FLG02
U 1 1 61BB6D54
P 3350 1900
F 0 "#FLG02" H 3350 1975 50  0001 C CNN
F 1 "PWR_FLAG" H 3350 2073 50  0000 C CNN
F 2 "" H 3350 1900 50  0001 C CNN
F 3 "~" H 3350 1900 50  0001 C CNN
	1    3350 1900
	-1   0    0    1   
$EndComp
Connection ~ 3350 1400
Text Label 5400 3250 2    50   ~ 0
EN
Text Label 5400 3350 2    50   ~ 0
TXD
Text Label 5400 3450 2    50   ~ 0
RXD
$Comp
L power:GND #PWR011
U 1 1 61BBAEE0
P 5900 3350
F 0 "#PWR011" H 5900 3100 50  0001 C CNN
F 1 "GND" V 5905 3222 50  0000 R CNN
F 2 "" H 5900 3350 50  0001 C CNN
F 3 "" H 5900 3350 50  0001 C CNN
	1    5900 3350
	0    -1   -1   0   
$EndComp
Text Label 5900 3450 0    50   ~ 0
BOOT
NoConn ~ 5900 3250
$Comp
L power:PWR_FLAG #FLG01
U 1 1 61BBD081
P 2800 1600
F 0 "#FLG01" H 2800 1675 50  0001 C CNN
F 1 "PWR_FLAG" V 2800 1728 50  0000 L CNN
F 2 "" H 2800 1600 50  0001 C CNN
F 3 "~" H 2800 1600 50  0001 C CNN
	1    2800 1600
	0    1    1    0   
$EndComp
Connection ~ 2800 1600
Connection ~ 4050 1400
$Comp
L Device:C C1
U 1 1 61C49718
P 3550 1800
F 0 "C1" V 3802 1800 50  0000 C CNN
F 1 "0.33" V 3711 1800 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 3588 1650 50  0001 C CNN
F 3 "~" H 3550 1800 50  0001 C CNN
	1    3550 1800
	0    1    -1   0   
$EndComp
$Comp
L Device:C C2
U 1 1 61C4ED2E
P 3850 1800
F 0 "C2" V 4102 1800 50  0000 C CNN
F 1 "0.1" V 4011 1800 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 3888 1650 50  0001 C CNN
F 3 "~" H 3850 1800 50  0001 C CNN
	1    3850 1800
	0    1    -1   0   
$EndComp
Wire Wire Line
	3350 1400 3350 1800
Wire Wire Line
	3400 1800 3350 1800
Connection ~ 3350 1800
Wire Wire Line
	3350 1800 3350 1900
Wire Wire Line
	3700 1700 3700 1800
Connection ~ 3700 1800
Wire Wire Line
	3700 1800 3700 1900
Wire Wire Line
	4050 1400 4050 1800
Wire Wire Line
	4050 1800 4000 1800
$Comp
L Device:R R6
U 1 1 61C9944C
P 6050 2650
F 0 "R6" H 6120 2696 50  0000 L CNN
F 1 "330" H 6120 2605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 5980 2650 50  0001 C CNN
F 3 "~" H 6050 2650 50  0001 C CNN
	1    6050 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 2300 6050 2500
Wire Wire Line
	6050 2800 6050 2850
Wire Wire Line
	6050 2850 6650 2850
Connection ~ 6650 2850
Wire Wire Line
	6650 2850 6650 2900
$Comp
L Connector:Conn_01x15_Female J2
U 1 1 61B7BEB9
P 3100 3400
F 0 "J2" H 2950 4300 50  0000 L CNN
F 1 "Conn_01x15_Female" H 2650 4200 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x15_P2.54mm_Vertical" H 3100 3400 50  0001 C CNN
F 3 "~" H 3100 3400 50  0001 C CNN
	1    3100 3400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x15_Female J3
U 1 1 61B7CECF
P 3950 3400
F 0 "J3" H 3842 4285 50  0000 C CNN
F 1 "Conn_01x15_Female" H 3842 4194 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x15_P2.54mm_Vertical" H 3950 3400 50  0001 C CNN
F 3 "~" H 3950 3400 50  0001 C CNN
	1    3950 3400
	-1   0    0    -1  
$EndComp
NoConn ~ 2900 2800
NoConn ~ 2900 2900
NoConn ~ 2900 3000
NoConn ~ 2900 3100
NoConn ~ 2900 3900
NoConn ~ 2900 3600
NoConn ~ 2900 3700
NoConn ~ 2900 3800
$Comp
L power:+5V #PWR07
U 1 1 61B91D58
P 2700 4000
F 0 "#PWR07" H 2700 3850 50  0001 C CNN
F 1 "+5V" H 2715 4173 50  0000 C CNN
F 2 "" H 2700 4000 50  0001 C CNN
F 3 "" H 2700 4000 50  0001 C CNN
	1    2700 4000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 61B95544
P 2800 4200
F 0 "#PWR08" H 2800 3950 50  0001 C CNN
F 1 "GND" H 2805 4027 50  0000 C CNN
F 2 "" H 2800 4200 50  0001 C CNN
F 3 "" H 2800 4200 50  0001 C CNN
	1    2800 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 4200 2800 4000
Wire Wire Line
	2800 4000 2900 4000
Wire Wire Line
	2900 4100 2700 4100
Wire Wire Line
	2700 4100 2700 4000
$Comp
L power:GND #PWR010
U 1 1 61B98C96
P 4250 4200
F 0 "#PWR010" H 4250 3950 50  0001 C CNN
F 1 "GND" H 4255 4027 50  0000 C CNN
F 2 "" H 4250 4200 50  0001 C CNN
F 3 "" H 4250 4200 50  0001 C CNN
	1    4250 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 4000 4250 4000
Wire Wire Line
	4250 4000 4250 4200
NoConn ~ 4150 3800
Text Label 4150 3600 0    50   ~ 0
CHAN1
NoConn ~ 4150 3100
NoConn ~ 4150 2800
NoConn ~ 4150 2700
$Comp
L power:+3V3 #PWR05
U 1 1 61BD958E
P 4450 4000
F 0 "#PWR05" H 4450 3850 50  0001 C CNN
F 1 "+3V3" H 4465 4173 50  0000 C CNN
F 2 "" H 4450 4000 50  0001 C CNN
F 3 "" H 4450 4000 50  0001 C CNN
	1    4450 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 4100 4450 4100
Wire Wire Line
	4450 4100 4450 4000
$Comp
L Device:R R7
U 1 1 61BDCA44
P 3850 4800
F 0 "R7" H 3920 4846 50  0000 L CNN
F 1 "1k" H 3920 4755 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3780 4800 50  0001 C CNN
F 3 "~" H 3850 4800 50  0001 C CNN
	1    3850 4800
	-1   0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR02
U 1 1 61BDDB25
P 3850 4650
F 0 "#PWR02" H 3850 4500 50  0001 C CNN
F 1 "+3V3" H 3865 4823 50  0000 C CNN
F 2 "" H 3850 4650 50  0001 C CNN
F 3 "" H 3850 4650 50  0001 C CNN
	1    3850 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 5000 3850 5000
Wire Wire Line
	3850 4950 3850 5000
Connection ~ 3850 5000
Wire Wire Line
	3850 5000 3950 5000
$EndSCHEMATC
