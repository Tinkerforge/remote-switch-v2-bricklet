EESchema Schematic File Version 2
LIBS:tinkerforge
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:remote-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Remote Bricklet"
Date "2017-09-13"
Rev "2.0"
Comp "Tinkerforge GmbH"
Comment1 "Licensed under CERN OHL v.1.1"
Comment2 "Copyright (©) 2017, B.Nordmeyer <bastian@tinkerforge.com>"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L VCC #PWR02
U 1 1 509A86E5
P 2500 3100
F 0 "#PWR02" H 2500 3200 30  0001 C CNN
F 1 "VCC" H 2500 3200 30  0000 C CNN
F 2 "" H 2500 3100 60  0001 C CNN
F 3 "" H 2500 3100 60  0001 C CNN
	1    2500 3100
	1    0    0    -1  
$EndComp
Text Notes 600  7650 0    40   ~ 0
Copyright Tinkerforge GmbH 2017.\nThis documentation describes Open Hardware and is licensed under the\nCERN OHL v. 1.1.\nYou may redistribute and modify this documentation under the terms of the\nCERN OHL v.1.1. (http://ohwr.org/cernohl). This documentation is distributed\nWITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF\nMERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A\nPARTICULAR PURPOSE. Please see the CERN OHL v.1.1 for applicable\nconditions\n
NoConn ~ 1700 3550
$Comp
L DRILL U3
U 1 1 4C6050A5
P 10700 6200
F 0 "U3" H 10750 6250 60  0001 C CNN
F 1 "DRILL" H 10700 6200 60  0000 C CNN
F 2 "DRILL_NP" H 10700 6200 60  0001 C CNN
F 3 "" H 10700 6200 60  0001 C CNN
	1    10700 6200
	1    0    0    -1  
$EndComp
$Comp
L DRILL U4
U 1 1 4C6050A2
P 10700 6400
F 0 "U4" H 10750 6450 60  0001 C CNN
F 1 "DRILL" H 10700 6400 60  0000 C CNN
F 2 "DRILL_NP" H 10700 6400 60  0001 C CNN
F 3 "" H 10700 6400 60  0001 C CNN
	1    10700 6400
	1    0    0    -1  
$EndComp
$Comp
L DRILL U6
U 1 1 4C60509F
P 11050 6400
F 0 "U6" H 11100 6450 60  0001 C CNN
F 1 "DRILL" H 11050 6400 60  0000 C CNN
F 2 "DRILL_NP" H 11050 6400 60  0001 C CNN
F 3 "" H 11050 6400 60  0001 C CNN
	1    11050 6400
	1    0    0    -1  
$EndComp
$Comp
L DRILL U5
U 1 1 4C605099
P 11050 6200
F 0 "U5" H 11100 6250 60  0001 C CNN
F 1 "DRILL" H 11050 6200 60  0000 C CNN
F 2 "DRILL_NP" H 11050 6200 60  0001 C CNN
F 3 "" H 11050 6200 60  0001 C CNN
	1    11050 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 4C5FCF5E
P 1950 3650
F 0 "#PWR05" H 1950 3650 30  0001 C CNN
F 1 "GND" H 1950 3580 30  0001 C CNN
F 2 "" H 1950 3650 60  0001 C CNN
F 3 "" H 1950 3650 60  0001 C CNN
	1    1950 3650
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR06
U 1 1 4C5FCF4F
P 1350 4400
F 0 "#PWR06" H 1350 4400 30  0001 C CNN
F 1 "GND" H 1350 4330 30  0001 C CNN
F 2 "" H 1350 4400 60  0001 C CNN
F 3 "" H 1350 4400 60  0001 C CNN
	1    1350 4400
	1    0    0    -1  
$EndComp
$Comp
L CON-SENSOR2 P1
U 1 1 4C5FCF27
P 1350 3850
F 0 "P1" H 1100 4400 60  0000 C CNN
F 1 "CON-SENSOR2" V 1500 3850 60  0000 C CNN
F 2 "CON-SENSOR2" H 1350 3850 60  0001 C CNN
F 3 "" H 1350 3850 60  0001 C CNN
	1    1350 3850
	-1   0    0    -1  
$EndComp
$Comp
L R_PACK4 RP1
U 1 1 51657BFE
P 2000 4200
F 0 "RP1" H 2000 4650 40  0000 C CNN
F 1 "82" H 2000 4150 40  0000 C CNN
F 2 "4X0402" H 2000 4200 60  0001 C CNN
F 3 "" H 2000 4200 60  0001 C CNN
	1    2000 4200
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1850 3150 1850 3750
Wire Wire Line
	1850 3750 1700 3750
Wire Wire Line
	1350 4400 1350 4300
Wire Wire Line
	1700 3650 1950 3650
$Comp
L RFM69HW U2
U 1 1 51E3E3FD
P 8450 4450
F 0 "U2" H 8450 4450 60  0000 C CNN
F 1 "RFM69HW" H 8750 3950 60  0000 C CNN
F 2 "RFM69HW" H 8450 4450 60  0001 C CNN
F 3 "" H 8450 4450 60  0000 C CNN
	1    8450 4450
	1    0    0    -1  
$EndComp
NoConn ~ 7900 4100
$Comp
L R R1
U 1 1 51E3EE32
P 7550 4800
F 0 "R1" V 7630 4800 50  0000 C CNN
F 1 "10k" V 7550 4800 50  0000 C CNN
F 2 "R0603F" H 7550 4800 60  0001 C CNN
F 3 "" H 7550 4800 60  0000 C CNN
	1    7550 4800
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR07
U 1 1 51E3EE3F
P 7300 4850
F 0 "#PWR07" H 7300 4850 30  0001 C CNN
F 1 "GND" H 7300 4780 30  0001 C CNN
F 2 "" H 7300 4850 60  0001 C CNN
F 3 "" H 7300 4850 60  0001 C CNN
	1    7300 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 4850 7300 4800
Wire Wire Line
	7800 4800 7900 4800
$Comp
L VCC #PWR08
U 1 1 51E3EEA8
P 8450 3400
F 0 "#PWR08" H 8450 3500 30  0001 C CNN
F 1 "VCC" H 8450 3500 30  0000 C CNN
F 2 "" H 8450 3400 60  0001 C CNN
F 3 "" H 8450 3400 60  0001 C CNN
	1    8450 3400
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 51E3EEAE
P 8650 3650
F 0 "C2" V 8800 3650 50  0000 L CNN
F 1 "1µF" V 8500 3550 50  0000 L CNN
F 2 "C0603F" H 8650 3650 60  0001 C CNN
F 3 "" H 8650 3650 60  0001 C CNN
	1    8650 3650
	-1   0    0    1   
$EndComp
$Comp
L C C3
U 1 1 51E3EEC8
P 9050 3650
F 0 "C3" V 9200 3650 50  0000 L CNN
F 1 "100nF" V 8900 3550 50  0000 L CNN
F 2 "C0603F" H 9050 3650 60  0001 C CNN
F 3 "" H 9050 3650 60  0001 C CNN
	1    9050 3650
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR09
U 1 1 51E3EECE
P 8650 3900
F 0 "#PWR09" H 8650 3900 30  0001 C CNN
F 1 "GND" H 8650 3830 30  0001 C CNN
F 2 "" H 8650 3900 60  0001 C CNN
F 3 "" H 8650 3900 60  0001 C CNN
	1    8650 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 51E3EEF2
P 8450 5200
F 0 "#PWR010" H 8450 5200 30  0001 C CNN
F 1 "GND" H 8450 5130 30  0001 C CNN
F 2 "" H 8450 5200 60  0001 C CNN
F 3 "" H 8450 5200 60  0001 C CNN
	1    8450 5200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR011
U 1 1 51E3EEF8
P 9050 3900
F 0 "#PWR011" H 9050 3900 30  0001 C CNN
F 1 "GND" H 9050 3830 30  0001 C CNN
F 2 "" H 9050 3900 60  0001 C CNN
F 3 "" H 9050 3900 60  0001 C CNN
	1    9050 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 3450 9050 3450
Wire Wire Line
	8450 3400 8450 3850
Connection ~ 8650 3450
Connection ~ 8450 3450
Wire Wire Line
	8650 3900 8650 3850
Wire Wire Line
	9050 3900 9050 3850
Wire Wire Line
	8400 5050 8500 5050
Wire Wire Line
	8450 5050 8450 5200
Connection ~ 8450 5050
Wire Wire Line
	9000 4800 9800 4800
NoConn ~ 9000 4600
NoConn ~ 9000 4500
NoConn ~ 9000 4400
NoConn ~ 9000 4100
$Comp
L GND #PWR012
U 1 1 522D91CB
P 9800 5100
F 0 "#PWR012" H 9800 5100 30  0001 C CNN
F 1 "GND" H 9800 5030 30  0001 C CNN
F 2 "" H 9800 5100 60  0001 C CNN
F 3 "" H 9800 5100 60  0001 C CNN
	1    9800 5100
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 59B94840
P 1750 4400
F 0 "C?" H 1800 4500 50  0000 L CNN
F 1 "220pF" H 1800 4300 50  0000 L CNN
F 2 "C0402E" H 1750 4400 60  0001 C CNN
F 3 "" H 1750 4400 60  0001 C CNN
	1    1750 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 3850 1800 3850
Wire Wire Line
	1800 3950 1700 3950
Wire Wire Line
	1700 4050 1800 4050
Wire Wire Line
	1800 4150 1700 4150
Text GLabel 2300 3850 2    60   Output ~ 0
S-CS
Text GLabel 2300 3950 2    60   Output ~ 0
S-CLK
Text GLabel 2300 4050 2    60   Output ~ 0
S-MOSI
Text GLabel 2300 4150 2    60   Input ~ 0
S-MISO
Wire Wire Line
	2200 3850 2300 3850
Wire Wire Line
	2300 3950 2200 3950
Wire Wire Line
	2200 4050 2300 4050
Wire Wire Line
	2300 4150 2200 4150
$Comp
L GND #PWR?
U 1 1 59B94CE1
P 1750 4650
F 0 "#PWR?" H 1750 4650 30  0001 C CNN
F 1 "GND" H 1750 4580 30  0001 C CNN
F 2 "" H 1750 4650 60  0001 C CNN
F 3 "" H 1750 4650 60  0001 C CNN
	1    1750 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 4200 1750 4150
Connection ~ 1750 4150
Wire Wire Line
	1750 4650 1750 4600
$Comp
L C C?
U 1 1 59B94E36
P 2100 3350
F 0 "C?" V 2250 3350 50  0000 L CNN
F 1 "1uF" V 1950 3250 50  0000 L CNN
F 2 "C0603F" H 2100 3350 60  0001 C CNN
F 3 "" H 2100 3350 60  0001 C CNN
	1    2100 3350
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 59B95013
P 2500 3350
F 0 "C?" V 2650 3350 50  0000 L CNN
F 1 "10uF" V 2350 3250 50  0000 L CNN
F 2 "C0603F" H 2500 3350 60  0001 C CNN
F 3 "" H 2500 3350 60  0001 C CNN
	1    2500 3350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59B950AB
P 2100 3600
F 0 "#PWR?" H 2100 3600 30  0001 C CNN
F 1 "GND" H 2100 3530 30  0001 C CNN
F 2 "" H 2100 3600 60  0001 C CNN
F 3 "" H 2100 3600 60  0001 C CNN
	1    2100 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59B95107
P 2500 3600
F 0 "#PWR?" H 2500 3600 30  0001 C CNN
F 1 "GND" H 2500 3530 30  0001 C CNN
F 2 "" H 2500 3600 60  0001 C CNN
F 3 "" H 2500 3600 60  0001 C CNN
	1    2500 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 3150 2500 3150
Connection ~ 2100 3150
Wire Wire Line
	2100 3600 2100 3550
Wire Wire Line
	2500 3600 2500 3550
Wire Wire Line
	2500 3150 2500 3100
Connection ~ 2500 3150
$Comp
L XMC1XXX24 U?
U 1 1 59B95367
P 5300 2250
F 0 "U?" H 5150 2650 60  0000 C CNN
F 1 "XMC1XXX24" H 5300 1850 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5450 3000 60  0001 C CNN
F 3 "" H 5450 3000 60  0000 C CNN
	1    5300 2250
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 59B95622
P 4800 2200
F 0 "C?" V 4950 2200 50  0000 L CNN
F 1 "100nF" V 4650 2100 50  0000 L CNN
F 2 "C0603F" H 4800 2200 60  0001 C CNN
F 3 "" H 4800 2200 60  0001 C CNN
	1    4800 2200
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR?
U 1 1 59B95816
P 4800 1900
F 0 "#PWR?" H 4800 2000 30  0001 C CNN
F 1 "VCC" H 4800 2000 30  0000 C CNN
F 2 "" H 4800 1900 60  0001 C CNN
F 3 "" H 4800 1900 60  0001 C CNN
	1    4800 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59B958BD
P 4800 2600
F 0 "#PWR?" H 4800 2600 30  0001 C CNN
F 1 "GND" H 4800 2530 30  0001 C CNN
F 2 "" H 4800 2600 60  0001 C CNN
F 3 "" H 4800 2600 60  0001 C CNN
	1    4800 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 2600 4800 2400
Wire Wire Line
	4950 2550 4800 2550
Connection ~ 4800 2550
Wire Wire Line
	4950 2450 4800 2450
Connection ~ 4800 2450
Wire Wire Line
	4950 1950 4800 1950
Wire Wire Line
	4800 1900 4800 2000
Connection ~ 4800 1950
$Comp
L XMC1XXX24 U?
U 2 1 59B95B57
P 5300 3500
F 0 "U?" H 5150 4100 60  0000 C CNN
F 1 "XMC1XXX24" H 5300 2950 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5450 4250 60  0001 C CNN
F 3 "" H 5450 4250 60  0000 C CNN
	2    5300 3500
	1    0    0    -1  
$EndComp
$Comp
L XMC1XXX24 U?
U 3 1 59B95D82
P 5300 4550
F 0 "U?" H 5150 4800 60  0000 C CNN
F 1 "XMC1XXX24" H 5300 4250 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5450 5300 60  0001 C CNN
F 3 "" H 5450 5300 60  0000 C CNN
	3    5300 4550
	1    0    0    -1  
$EndComp
$Comp
L XMC1XXX24 U?
U 4 1 59B95FC8
P 5300 5650
F 0 "U?" H 5150 6100 60  0000 C CNN
F 1 "XMC1XXX24" H 5300 5200 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5450 6400 60  0001 C CNN
F 3 "" H 5450 6400 60  0000 C CNN
	4    5300 5650
	1    0    0    -1  
$EndComp
Text GLabel 4700 3750 0    60   Input ~ 0
S-CS
Text GLabel 4700 3850 0    60   Input ~ 0
S-CLK
Text GLabel 4700 3950 0    60   Input ~ 0
S-MOSI
Text GLabel 4700 5300 0    60   Output ~ 0
S-MISO
$Comp
L CONN_01X02 P?
U 1 1 59B968E9
P 3500 4900
F 0 "P?" H 3500 5050 50  0000 C CNN
F 1 "CONN_01X02" V 3600 4800 50  0000 C CNN
F 2 "kicad-libraries:SolderJumper" H 3500 4800 50  0001 C CNN
F 3 "" H 3500 4800 50  0000 C CNN
	1    3500 4900
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 59B96AA0
P 3450 5150
F 0 "#PWR?" H 3450 5150 30  0001 C CNN
F 1 "GND" H 3450 5080 30  0001 C CNN
F 2 "" H 3450 5150 60  0001 C CNN
F 3 "" H 3450 5150 60  0001 C CNN
	1    3450 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 5150 3450 5100
Wire Wire Line
	4700 5300 4950 5300
Wire Wire Line
	4700 3950 4950 3950
Wire Wire Line
	4950 3850 4700 3850
Wire Wire Line
	4700 3750 4950 3750
$Comp
L R R?
U 1 1 59B96DBE
P 4250 5800
F 0 "R?" V 4330 5800 50  0000 C CNN
F 1 "1k" V 4250 5800 50  0000 C CNN
F 2 "R0603F" H 4250 5800 60  0001 C CNN
F 3 "" H 4250 5800 60  0000 C CNN
	1    4250 5800
	0    -1   -1   0   
$EndComp
$Comp
L LED D?
U 1 1 59B9705B
P 3750 5800
F 0 "D?" H 3750 5900 50  0000 C CNN
F 1 "LED" H 3750 5700 50  0000 C CNN
F 2 "kicad-libraries:D0603F" H 3750 5800 50  0001 C CNN
F 3 "" H 3750 5800 50  0000 C CNN
	1    3750 5800
	1    0    0    -1  
$EndComp
Text Notes 5650 3950 0    39   ~ 0
SPI Slave\nP0.13 : CH0-DX2 : SEL\nP0.14 : CH0-DX1 : CLK\nP0.15 : CH0-DX0 : MOSI
Text Notes 5600 5400 0    39   ~ 0
SPI Slave\nP2.0 :  CH0-DOUT0 : MISO
$Comp
L VCC #PWR?
U 1 1 59B9820B
P 3550 5700
F 0 "#PWR?" H 3550 5800 30  0001 C CNN
F 1 "VCC" H 3550 5800 30  0000 C CNN
F 2 "" H 3550 5700 60  0001 C CNN
F 3 "" H 3550 5700 60  0001 C CNN
	1    3550 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 5800 3550 5700
Wire Wire Line
	3950 5800 4000 5800
Text GLabel 7100 4300 0    60   Input ~ 0
M-CS
$Comp
L R_PACK4 RP?
U 1 1 59B99DC7
P 7400 4650
F 0 "RP?" H 7400 5100 40  0000 C CNN
F 1 "220" H 7400 4600 40  0000 C CNN
F 2 "4X0603" H 7400 4650 60  0001 C CNN
F 3 "" H 7400 4650 60  0001 C CNN
	1    7400 4650
	-1   0    0    -1  
$EndComp
Text GLabel 7100 4400 0    60   Input ~ 0
M-CLK
Text GLabel 7100 4500 0    60   Input ~ 0
M-MOSI
Text GLabel 7100 4600 0    60   Output ~ 0
M-MISO
Wire Wire Line
	7100 4300 7200 4300
Wire Wire Line
	7200 4400 7100 4400
Wire Wire Line
	7100 4500 7200 4500
Wire Wire Line
	7200 4600 7100 4600
Wire Wire Line
	7600 4600 7900 4600
Wire Wire Line
	7900 4500 7600 4500
Wire Wire Line
	7600 4400 7900 4400
Wire Wire Line
	7900 4300 7600 4300
Text GLabel 4700 3250 0    60   Input ~ 0
M-MISO
Text GLabel 4700 3350 0    60   Output ~ 0
M-MOSI
Text GLabel 4700 3450 0    60   Output ~ 0
M-CLK
Text GLabel 4700 3550 0    60   Output ~ 0
M-CS
$Comp
L ANT2 ANT?
U 1 1 59B9AB09
P 10050 4800
F 0 "ANT?" H 9800 4950 60  0000 C CNN
F 1 "ANT2" H 10050 5050 60  0000 C CNN
F 2 "FT-004B" H 10000 4700 60  0001 C CNN
F 3 "" H 10000 4700 60  0000 C CNN
	1    10050 4800
	0    1    1    0   
$EndComp
Wire Wire Line
	9800 5100 9800 4900
Wire Wire Line
	4700 3250 4950 3250
Wire Wire Line
	4950 3350 4700 3350
Wire Wire Line
	4700 3450 4950 3450
Wire Wire Line
	4950 3550 4700 3550
Text Notes 5650 3500 0    39   ~ 0
SPI Master\nP0.6 : CH1-DX0 : MISO\nP0.7 : CH1-DOUT0 : MOSI\nP0.8 : CH1-SCLK_OUT : CLK\nP0.9 : CH1-SEL0 : CS
Text GLabel 9750 4200 2    60   Input ~ 0
DIO1
Text GLabel 9750 4300 2    60   Input ~ 0
DIO2
Wire Wire Line
	9000 4200 9100 4200
Wire Wire Line
	9000 4300 9100 4300
Text GLabel 4700 4700 0    60   Input ~ 0
DIO1
Text GLabel 4700 4600 0    60   Input ~ 0
DIO2
$Comp
L R_PACK4 RP?
U 1 1 59B9C5C1
P 9400 4450
F 0 "RP?" H 9400 4900 40  0000 C CNN
F 1 "220" H 9400 4400 40  0000 C CNN
F 2 "4X0603" H 9400 4450 60  0001 C CNN
F 3 "" H 9400 4450 60  0001 C CNN
	1    9400 4450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9200 4100 9100 4100
Wire Wire Line
	9100 4100 9100 4200
Wire Wire Line
	9200 4400 9100 4400
Wire Wire Line
	9100 4400 9100 4300
Wire Wire Line
	9600 4400 9700 4400
Wire Wire Line
	9700 4400 9700 4300
Wire Wire Line
	9700 4300 9750 4300
Wire Wire Line
	9600 4100 9700 4100
Wire Wire Line
	9700 4100 9700 4200
Wire Wire Line
	9700 4200 9750 4200
NoConn ~ 9200 4200
NoConn ~ 9200 4300
NoConn ~ 9600 4300
NoConn ~ 9600 4200
Wire Wire Line
	4700 4600 4950 4600
Wire Wire Line
	4950 4700 4700 4700
$EndSCHEMATC
