/*
 * File:   Lab7.c
 * Author: stalbot8
 *
 * Created on October 24, 2018, 4:05 PM
 */


#include "xc.h"
// Select oscillator
#pragma config FNOSC = FRC       // 8 MHz FRC oscillator
int counter = 0;
void _ISR _OC1Interrupt(void)
{
    counter++;
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG
}

int main()
{
    
    _TRISB0 = 0;
    _TRISA6 = 0;
    
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
    OC1RS = 15000;               // Period of OC1 to achieve desired PWM 
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
    
   
    
    _OC1IE = 1; //ENABLES YOUR INTERRUPT
    int switchy = 200;
    while(1)
    {
        if (counter >= switchy)
        {
            counter = 0;
            switchy  = switchy + 200;                    
            _LATB0 = _LATB0^1;
        }
        if (switchy > 800)
        {
            _LATB0 = _LATB0^1;
            switchy = 0;
        }
    }
    
    return 0;
}
