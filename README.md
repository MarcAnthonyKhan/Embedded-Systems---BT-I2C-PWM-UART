CIS 4930 Embedded Systems, Summer 2022
Final Project
Jason Erickson, Joseph Flitz, Marc Khan, Tyler Luttmann





Problem Description


The objective of the Embedded Systems final project is to control the speed of a motor using a bluetooth module connected to the TI-MSP430FR2355 Launchpad and display the speed, in percent, on an i2c LCD display.

A more in-depth description of our implementation is as follows: 

A character is sent from the bluetooth terminal app on a smartphone to the HC-05 bluetooth module. 

The character read from the bluetooth module will correspond to an integer between 0 and 9, which when multiplied by 10, represents the amount of power being sent to the DC motor as a percentage. 

This integer is used in calculating the duty cycle of the PWM signal that powers the DC motor. 

A higher percentage, or duty cycle, means that the PWM signal is “high” for more of the overall cycle, which results in more power being sent and the motor moving faster. 

The PWM signal is sent to the H-Bridge, which sends power to the motor from a separate power source, depending on the interpreted PWM input. 

The percentage is also sent to the I2C-connected LCD display, showing the percentage of power that the motor is receiving.
