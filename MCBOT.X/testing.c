/*
 * File:   testing.c
 * Author: MCBOT Team
 * 
 * Created on November 3, 2018, 10:34 AM
 * 
 * This file is for testing functions before adding them to the project
 */

#pragma config FNOSC = FRCDIV
#include "xc.h"

#define pin12 = _RB8
#define pin16 = _RB13

int counter = 0;

void _ISR _OC1Interrupt(void)
{
    counter++;
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG
}

void config_PWM_1() {
      //-----------------------------------------------------------
    // CONFIGURE PWM1 USING OC1 (on pin 14)
    
    // Clear control bits initially
    OC1CON1 = 0;
    OC1CON2 = 0;
   
    // Set period and duty cycle
    OC1R = 1;                // Set Output Compare value to achieve
                                // desired duty cycle. This is the number
                                // of timer counts when the OC should send
                                // the PWM signal low. The duty cycle as a
                                // fraction is OC1R/OC1RS.
    OC1RS = 78;               // Period of OC1 to achieve desired PWM    //39999 should give 50Hz
                                // frequency, FPWM. See Equation 15-1
                                // in the datasheet. For example, for
                                // FPWM = 1 kHz, OC1RS = 3999. The OC1RS 
                                // register contains the period when the
                                // SYNCSEL bits are set to 0x1F (see FRM)
    
    // Configure OC1
    OC1CON1bits.OCTSEL = 0b000; // System (peripheral) clock as timing source
    OC1CON2bits.SYNCSEL = 0x1F; // Select OC1 as synchronization source
                                // (self synchronization) -- Although we
                                // selected the system clock to determine
                                // the rate at which the PWM timer increments,
                                // we could have selected a different source
                                // to determine when each PWM cycle initiates.
                                // From the FRM: When the SYNCSEL<4:0> bits
                                // (OCxCON2<4:0>) = 0b11111, they make the
                                // timer reset when it reaches the value of
                                // OCxRS, making the OCx module use its
                                // own Sync signal.
    OC1CON2bits.OCTRIG = 0;     // Synchronizes with OC1 source instead of
                                // triggering with the OC1 source
    OC1CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    
}

void driveForward() {
    _LATB8 = 1;
    _LATB13 = 0;
}

void turnRight() {
    _LATB8 = 1;
    _LATB13 = 1;
}

// Timer1 ISR -- This is the function that is automatically
// called each time the timer reaches the value specified
// in the register PR1. When that value is reached, the timer
// interrupt flag (T1IF) is set to 1, the while(1) loop in the
// main() function pauses, and the program branches to
// this function.
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
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

void configTimer2() {
    
    T2CONbits.TON = 1;      // turn on Timer1
    T2CONbits.TCS = 0;      // INTERNAL CLOCK
    T2CONbits.TCKPS = 0b11;     // 1:256 prescale. For more info on why its 0b11 check the data sheet
    PR2 = 1000;      // TIMER PERIOD OF 155 is 50 Hz
    TMR2 = 0;     // RESET TIMER1 TO ZERO
    
}

void configTimer1() {
    
    T1CONbits.TON = 1;      // turn on Timer1
    T1CONbits.TCS = 0;      // INTERNAL CLOCK
    T1CONbits.TCKPS = 0b11;     // 1:256 prescale. For more info on why its 0b11 check the data sheet
    PR1 = 15625;      // TIMER PERIOD OF 15625 is 2 seconds with the 2 MHz oscillator and 256 prescaler
    TMR1 = 0;     // RESET TIMER1 TO ZERO
    
    // Configure Timer1 interrupt
    _T1IP = 4;          // Select Timer1 interrupt priority
    _T1IE = 1;          // Enable Timer1 interrupt
    _T1IF = 0;          // Clear Timer1 interrupt flag
}

int main() {
    _TRISB8 = 0;
    
    _RCDIV = 0b010;   //Configures postscaler on oscillator to Fcy = 2 MHz
    
    
    config_PWM_1();
    configTimer1();
    configTimer2();
    
    _RB8 = 1;
    
    while(1) {
        
    }
    
    return 0;
}