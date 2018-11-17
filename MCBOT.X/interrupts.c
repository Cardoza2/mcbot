/*
 * File:   interrupts.c
 * Author: MCBOT Team
 *
 * Created on November 1, 2018, 4:00 PM
 */


// Timer1 ISR -- This is the function that is automatically
// called each time the timer reaches the value specified
// in the register PR1. When that value is reached, the timer
// interrupt flag (T1IF) is set to 1, the while(1) loop in the
// main() function pauses, and the program branches to
// this function.
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {       //used for sorting servo
    // Clear Timer1 interrupt flag so that the program doesn't
    // just jump back to this function when it returns to the
    // while(1) loop.
    _T1IF = 0;

    TMR1 = 0;
    
    //A pwm period of 39999 is 50 MHz or 20 ms.
    //the servo goes from 0 to 180 degrees through
    //1 to 2 ms period.  This is a duty cycle of
    //2000 to 4000
    if (OC1R <= 1) { 
        OC1R = 8;
    }
    else {
        OC1R = 1;
    }
    
    if (_RB8 == 0) {
        _RB8 = 1;
    }
    else {
        _RB8 = 0;
    }
    
}



