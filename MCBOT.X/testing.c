/*
 * File:   testing.c
 * Author: MCBOT Team
 * 
 * Created on November 3, 2018, 10:34 AM
 * 
 * This file is for testing functions before adding them to the project
 */

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
    OC1R = 3999;                // Set Output Compare value to achieve
                                // desired duty cycle. This is the number
                                // of timer counts when the OC should send
                                // the PWM signal low. The duty cycle as a
                                // fraction is OC1R/OC1RS.
    OC1RS = 3999;               // Period of OC1 to achieve desired PWM 
                                // frequency, FPWM. See Equation 15-1
                                // in the datasheet. For example, for
                                // FPWM = 1 kHz, OC1RS = 3999. The OC1RS 
                                // register contains the period when the
                                // SYNCSEL bits are set to 0x1F (see FRM)
    
    // Configure OC1
    OC1CON1bits.OCTSEL = 0b111; // System (peripheral) clock as timing source
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
    _LATB13 = 1;
}

void turnRight() {
    _LATB8 = 1;
    _LATB13 = 0;
}

int main() {
    
    _TRISB8 = 0;
    _TRISB13 = 0;
    //ANSB8 = 0;
    //_ANSB13 = 0;
   
    config_PWM_1();
    
    _OC1IE = 1; //ENABLES YOUR INTERRUPT
    
    while(1) {
        if (counter <= 100) {
            driveForward();
        }
        else if (counter <= 150) {
            turnRight();
        }
        else {
            counter = 0;
        }
    }
    
    return 0;
}