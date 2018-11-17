/*
 * File:   testing.c
 * Author: MCBOT Team
 * 
 * Created on November 3, 2018, 10:34 AM
 * 
 * This file is for testing functions before adding them to the project
 */

#pragma config FNOSC = FRC
#pragma config ICS = PGx3
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
    OC1R = 3990;                // Set Output Compare value to achieve
                                // desired duty cycle. This is the number
                                // of timer counts when the OC should send
                                // the PWM signal low. The duty cycle as a
                                // fraction is OC1R/OC1RS.
    OC1RS = 15000;               // Period of OC1 to achieve desired PWM    //39999 should give 50Hz
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
    
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG

}

configCNInterrupt() {
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
}

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
    if (_RB12 == 1)     //Forward corner
    {
        stopDriving();
    }
    else if (_RB13 == 1){   //Back corner
        stopDriving();
    }
    else                    //out of corner
    {
        _LATA0 = 0;
        _LATB2 = 0;
        state = 0;
    }
}

void stopDriving() {
    
}

int main() {
    _TRISB8 = 0;
    _TRISB13 = 0;
    _ANSB13 = 0;
    
    
    _OC1IE = 1;     //Enables the Interrupt
    config_PWM_1();
    configCNInterrupt();
    
    _LATB8 = 1;
    _LATB13 = 1;
    
    
     
    
    _LATB13 = 0;
    return 0;
}