/*
 * File:   interrupts.c
 * Author: MCBOT Team
 *
 * Created on November 1, 2018, 4:00 PM
 */


// Timer2 ISR -- This is the function that is automatically
// called each time the timer reaches the value specified
// in the register PR1. When that value is reached, the timer
// interrupt flag (T2IF) is set to 1, the while(1) loop in the
// main() function pauses, and the program branches to
// this function.
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {       //used for sorting servo
    // Clear Timer2 interrupt flag so that the program doesn't
    // just jump back to this function when it returns to the
    // while(1) loop.
    _T2IF = 0;

    TMR2 = 0;
    
    //A pwm period of 39999 is 50 MHz or 20 ms.
    //the servo goes from 0 to 180 degrees through
    //1 to 2 ms period.  This is a duty cycle of
    //2000 to 4000
    if (OC3R <= 1) { 
        OC3R = 8;
    }
    else {
        OC3R = 1;
    }
    
}

// Change Notification Interrupt Service Routine (ISR)
// This function executes every time the micro receives
// an interrupt originating from any of the CN pins. The
// micro knows the interrupt is from the one of the CN 
// when the change notification interrupt flag (CNIF)
// is set.
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {

    // Remember to clear the CN interrupt flag when
    // this ISR is entered.
    _CNIF = 0; // Clear interrupt flag

    // Place in this ISR whatever code should be executed
    // when a change in the button state is detected.
    // You will need to write code to distinguish between
    // a button press and a button release.
    if (_RB12 == 1) {       //Forward corner
        stopDriving();
    }
    else if (_RB13 == 1){   //Back corner
        stopDriving();
    }
    else {}                 //leaving corner
}

void _ISR _OC2Interrupt(void) {
    liftingCounter++;  
    _OC2IF = 0; // eNABLES iNTERRUPT FLAG
}

void _ISR _OC1Interrupt(void) {
    drivingCounter++;
    
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG
}
