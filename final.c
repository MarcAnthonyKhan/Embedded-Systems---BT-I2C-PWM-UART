/*========================================================*
 * Final Project
 * Embedded Systems
 * Group 14
 * Jason Erickson, Joseph Flitz, Marc Khan, Tyler Luttmann
 *========================================================*/

        //Pinout:
        //  P6.0    =    H-Bridge Speed Control
        //  P1.2    =    SDA for LCD Display
        //  P1.3    =    SCL for LCD Display
        //  P1.6    =    UART for Bluetooth Connection


#include <msp430.h>
#include "LiquidCrystal_I2C.h"

//CONSTANTS

//flag is used to ensure a change in speed is only printed on the LCD once
int flag = 0;

//recieved_speed comes from the bluetooth module and is used to tell the motor how fast to run
int recieved_speed = 0;


//used to print the current speed as a percentage on the LCD
char message[10] = "50%";

//the current speed of the PWM signal as a % -- start at 50% power
float Speed = 50;

// MAIN
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;       	// Disable watchdog timer

    // Configure H-bridge Pin
    P6DIR   |=  BIT0;              			 //Set H-Bridge speed, pin 6.0, as output

    // Configure UART
    UCA0CTLW0 |= UCSWRST;           		// Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__ACLK;

    // Baud Rate calculation (values come from data sheet for 9600 baud rate)
    UCA0BR0 = 3;
    UCA0MCTLW = 0x9200;

    // Configure UART pins
    P1DIR &= ~BIT6;
    P1SEL1 &= ~BIT6;
    P1SEL0 |= BIT6;


    //can be used to blink on-board led for testing
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    //Initialize all pins
    P6OUT   |=  BIT0;               		    //Set P6.0 to 1 initially; to be disabled by PWM process
    PM5CTL0 &=  ~LOCKLPM5;               //Activate GPIO pins

    //LCD Setup
    I2C_Init(0x27);
    LCD_Setup();
    LCD_SetCursor(1, 0);
    LCD_Write ("Motor Speed: ");


    //Setup Timer for PWM
    TB0CTL  =   TBSSEL__ACLK + MC__UP + TBCLR;      //Set Timer B in ALCK Mode, count up mode and clear mode
    TB0CCR0 =   328-1;                              	//PWM Total Cycle/Period. Value of 328 found for this motor. Counter counts up to this value and resets to 0
    TB0CCR1 =   0;                                  	//Starting Duty Cycle, start completely disabled CCR1 does not reset timer to 0, unlike CCR0, but does trigger flag, like CCR0
    TB0CCTL0    |=  CCIE;                          	//enable interrupts for CCR0
    TB0CCTL1    |=  CCIE;                           	//enable interrupts for CCR1

    UCA0CTLW0 &= ~UCSWRST;              	// Initialize eUSCI
    UCA0IE |= UCRXIE;

    __enable_interrupt();                           	//Enable maskable interrupts for PWM Timer
    TB0CCR1 =   (Speed/100) * TB0CCR0;   	//Initial motor speed calculation

    while(1)
        {
            //only print message and change speed if a new speed came in from bluetooth
            if(flag){

                //multiply by 10 to make speed a %
                Speed = 10 * recieved_speed;

                //write new speed to LCD
                LCD_SetCursor(1, 1);
                LCD_Write(message);

                //reset our flag so that we only come here once for every new interrupt
                flag = 0;

                TB0CCR1 =   (Speed/100) * TB0CCR0;     //Set timer duty cycle to percentage of total cycle                                                        		
            }
        }


    return 0;
}




//INTERRUPTS
/*========================================================*/

#pragma vector = TIMER0_B0_VECTOR     	//CCR0 ISR, resets PWM to high at end of cycle
__interrupt void ISR_TB0_CCR0(void)
{
    P6OUT   |=BIT0;                         		//Set P6.0 to high
    TB0CCTL0 &= ~CCIFG;                     	//Clear flag
}

#pragma vector = TIMER0_B1_VECTOR     	//CCR1 ISR, sets PWM low once value reached
__interrupt void ISR_TB0_CCR1(void)
{
    P6OUT   &= ~BIT0;                       		//Set P6.0 to low
    TB0CCTL1 &= ~CCIFG;                     	//Clear flag
}

#pragma vector=USCI_A0_VECTOR          	//handles bluetooth communication
__interrupt void USCI_A0_ISR(void)
{
    //a-j represent the speeds 1-10 as a percentage
    //check what the received speed is and edit our message, trip flag, and save the speed
    if(UCA0RXBUF == 'a'){
        recieved_speed = 0;
        strcpy(message, "0%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'b'){
        recieved_speed = 1;
        strcpy(message, "10%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'c'){
        recieved_speed = 2;
        strcpy(message, "20%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'd'){
        recieved_speed = 3;
        strcpy(message, "30%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'e'){
        recieved_speed = 4;
        strcpy(message, "40%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'f'){
        recieved_speed = 5;
        strcpy(message, "50%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'g'){
        recieved_speed = 6;
        strcpy(message, "60%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'h'){
        recieved_speed = 7;
        strcpy(message, "70%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'i'){
        recieved_speed = 8;
        strcpy(message, "80%");
        flag = 1;
        return;
    }
    else if(UCA0RXBUF == 'j'){
        recieved_speed = 9;
        strcpy(message, "90%");
        flag = 1;
        return;
    }
    else{
        //if an invalid speed is received set motor back to default 50%
        recieved_speed = 5;
	        strcpy(message, "50%")
        flag = 1;
	        return;
    }

}
