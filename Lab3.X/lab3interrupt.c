/*
 * File:   lab3interrupt.c
 * Author: stalbot8
 *
 * Created on September 27, 2018, 3:20 PM
 */


// This program lights an LED on pin 2 for 1 second,
// followed by an LED on pin 6 for 0.5 seconds. It 
// repeats as long as a switch connected to pin 5 is
// closed. The program uses a timer interrupt to handle
// the timing and a change notification interrupt to respond
// to closing the switch.

#include<xc.h>

//-----------------------------------------------------
// Select oscillator
// PLACE CODE TO SELECT OSCILLATOR HERE
#pragma config FNOSC = LPRC
int state = 1;

//-----------------------------------------------------
// Change Notification Interrupt Service Routine (ISR)
// This function executes every time the micro receives
// an interrupt originating from any of the CN pins. The
// micro knows the interrupt is from the one of the CN 
// when the change notification interrupt flag (CNIF)
// is set.

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{

    // Remember to clear the CN interrupt flag when
    // this ISR is entered.
    // PLACE CODE TO CLEAR THE CN INTERRUPT FLAG HERE
    _CNIF = 0; // Clear interrupt flag

    // Place in this ISR whatever code should be executed
    // when a change in the button state is detected.
    // You will need to write code to distinguish between
    // a button press and a button release.
    // PLACE CUSTOM CODE HERE
    if (_RB1 == 1)
    {
        _LATA0 = 1;
        _LATB2 = 0;
        TMR1 = 0;   
        state = 1;
    }
    else
    {
        _LATA0 = 0;
        _LATB2 = 0;
        state = 0;
    }
}


//-----------------------------------------------------
// Timer1 Interrupt Service Routine (ISR)
// This function executes every time the micro receives
// an interrupt originating from Timer1. The micro knows
// the interrupt is from Timer1 when the Timer1 interrupt
// flag (T1IF) is set.

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{

    // Remember to clear the Timer1 interrupt flag when
    // this ISR is entered.
    // PLACE CODE TO CLEAR THE TIMER1 INTERRUPT FLAG HERE
    _T1IF = 0; // Clear interrupt flag


    // Place in this ISR whatever code should be executed
    // when the timer reaches the period (PR1) that you
    // specify
    // PLACE CUSTOM CODE HERE
    if (state == 1)
    {
        _LATA0 = 0;
        _LATB2 = 1;
        PR1 = 7750;
        TMR1 = 0;
        state = 2;
    }
    else if (state == 2)
    {
        _LATA0 = 1;
        _LATB2 = 0;
        PR1 = 15500;
        TMR1 = 0;
        state = 1;
    }
}


//-----------------------------------------------------
// Main Function

int main()
{

    // Configure the digital I/O ports
    // PLACE CODE TO CONFIGURE THE DIGITAL I/O PORTS HERE
    _ANSB1 = 0;         // setting to digital for the next 2 lines
    _ANSA0 = 0;
    _ANSB2 = 0;
    
    _TRISA0 = 0;        // configuring digital I/O for next 2 lines. 0 is output, 1 is input.
    _TRISB1 = 1;
    _TRISB2 = 0;

    // Configure Timer1 using T1CON register
    // Remember to enable Timer1, select its prescale
    // value, and set the timer source to the internal
    // clock (FOSC/2)
    // PLACE CODE TO CONFIGURE TIMER1 HERE
    T1CONbits.TON = 1;      // turn on Timer1
    T1CONbits.TCS = 0;      // INTERNAL CLOCK
    //T1CONbits.TCKPS = 0b11;     // 1:256 prescale. For more info on why its 0b10, check the data sheet

    // Configure Timer1 interrupt
    // Remember to enable the Timer1 interrupt source
    // (IEC0 register) and set the Timer1 interrupt
    // priority to 4 (IPC0 register)
    // PLACE CODE TO CONFIGURE TIMER INTERRUPT HERE
    _T1IP = 4; // Select interrupt priority
    _T1IE = 1; // Enable interrupt
    PR1 = 15500; // Set period for interrupt to occur (this value depends on
                       // the time that you want to elapse between interrupts)

    // Clear Timer1 interrupt flag (IFS0 register)
    // PLACE CODE TO CLEAR THE TIMER1 INTERRUPT FLAG HERE
    _T1IF = 0; // Clear interrupt flag


    // Configure Change Notification interrupt
    // Set CN interrupt priority to 6
    // PLACE CODE TO CONFIGURE CN INTERRUPT HERE
    _CN5IE = 1; // Enable CN on pin 5 (CNEN1 register)
    _CN5PUE = 0; // Disable pull-up resistor (CNPU1 register)
    _CNIP = 6; // Set CN interrupt priority (IPC4 register)
    _CNIE = 1; // Enable CN interrupts (IEC1 register)

    // Clear Change Notification interrupt flag (IFS1 register)
    // PLACE CODE TO CLEAR THE CN INTERRUPT FLAG HERE
    _CNIF = 0; // Clear interrupt flag (IFS1 register)


    // Other initialization stuff
    // PLACE OTHER INITIALIZATION CODE HERE


    // Loop and wait - Note that it's empty because the only
    // time anything is done is when the interrupts occur,
    // sending the code to the appropriate ISR (see above)
    // where all the action happens

    while(1)
    {
    }

    return 0;
}


// THIS WAS FOR THE FIRST TASK OF THE LAB

/*
 * File:   Lab 3.c
 * Author: stalbot8
 *
 * Created on September 26, 2018, 3:25 PM
 


#include "xc.h"
#pragma config FNOSC = FRC

int main(void) 
{
    _ANSB1 = 0;         // setting to digital for the next 2 lines
    _ANSA0 = 0;
    _ANSB2 = 0;
    
    _TRISA0 = 0;        // configuring digital I/O for next 2 lines. 0 is output, 1 is input.
    _TRISB1 = 1;
    _TRISB2 = 0;
    
    T1CONbits.TON = 1;      // turn on Timer1
    T1CONbits.TCS = 0;      // INTERNAL CLOCK
    T1CONbits.TCKPS = 0b11;     // 1:256 prescale. For more info on why its 0b10, check the data sheet
    
    // int counts = 62500;
    PR1 = 23438;      // TIMER PERIOD OF 62500
    TMR1 = 0;     // RESET TIMER1 TO ZERO
    while(1)
    {
        if(_RB1 == 0)
            {
                _LATA0 = 0;
                _LATB2 = 0;
            }
        else{
            TMR1 = 0;
            while(TMR1 < .66666 * PR1)
            {   
                if(_RB1 == 0)
                {
                    _LATA0 = 0;
                    _LATB2 = 0;
                    break;
                } 

                _LATA0 = 1;       // TURN ON LED AT PIN 2
                _LATB2 = 0;     // MAKE SURE THAT PIN 6 LED IS OFF
            }



            while(TMR1 > .66666 * PR1)
            {
                 if(_RB1 == 0)
                {
                    _LATA0 = 0;
                    _LATB2 = 0;
                    break;
                }
                _LATA0 = 0;
                _LATB2 = 1;        
            }
        }
       
        
    }
    return 0;
}*/
