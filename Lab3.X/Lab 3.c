/*
 * File:   Lab 3.c
 * Author: stalbot8
 *
 * Created on September 26, 2018, 3:25 PM
 */


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
}
