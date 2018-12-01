/*
 * File:   Lab8.c
 * Author: stalbot8
 *
 * Created on October 31, 2018, 3:20 PM
 */


#include "xc.h"
#pragma config FNOSC = FRC       // 8 MHz FRC oscillator

double angle = 0;
int A = 0;
int B = 0;
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
    
    // CODE FOR METHOD 2
    // _RA2 is always what _RB2 was previously if it is going clockwise
    // The opposite is true if it is going clockwise
    if (_RA2 == A)
    {
        if (angle > 180)
            {  
                angle = 180;
            }
            else
            {
                angle += .75;
            }
    }
    else
    {
        if (angle < 3)
            {  
                angle = 0;
            }
            else
            {
                angle -= .75;
            }
    }
    A = _RB2;
    
    //CODE FOR METHOD 1
    /*if (_RB2 == 0)
    {
        if (_RA2 == 1)
        {
            
            if (angle > 180)
            {  
                angle = 180;
            }
            else
            {
                angle += 3;
            }
        }
        else
        {
            
            if (angle < 3)
            {  
                angle = 0;
            }
            else
            {
                angle -= 3;
            }
        }
    }
   */
}

int main(void) {
    A = _RB2;
    B = _RA2;

    _TRISB2 = 1;
    _TRISA2 = 1;

    _ANSB2 = 0;
    _ANSA2 = 0;
    
    
    //-----------------------------------------------------------
    // CONFIGURE PWM1 USING OC1 (on pin 14)
    
    // Clear control bits initially
    OC1CON1 = 0;
    OC1CON2 = 0;
    
    // Set period and duty cycle
    OC1R = 2000;                // Set Output Compare value to achieve
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
    
    
     // Configure Change Notification interrupt
   
    _CN6IE = 1; // Enable CN on pin 6 (CNEN1 register)
    _CN6PUE = 0; // Disable pull-up resistor (CNPU1 register)
    _CNIP = 5; // Set CN interrupt priority (IPC4 register)
    _CNIE = 1; // Enable CN interrupts (IEC1 register)

    // Clear Change Notification interrupt flag (IFS1 register)
    // PLACE CODE TO CLEAR THE CN INTERRUPT FLAG HERE
    _CNIF = 0; // Clear interrupt flag (IFS1 register)
    
    while(1)
    {
        OC1R = (3999/180)*angle;
    }
    
    return 0;
}
