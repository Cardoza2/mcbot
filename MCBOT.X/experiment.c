/*
 * File:   MS11.c
 * Author: jonah
 *
 * Created on December 1, 2018, 4:28 PM
 */

#pragma config FNOSC = FRC
#pragma config ICS = PGx3
#include "xc.h"
#define FCY 4000000UL       //needed for delay function
#include <libpic30.h>       //needed for delay function

int liftingCounter = 0;

void configPins() {
    _TRISA0 = 0; //pin 2 Lift Sleep
    _ANSA0 = 0; //pin 2 Disable analog (AN0)
    _TRISA1 = 0; //pin 3 Lift direction
    _ANSA1 = 0; //pin3 disable analog (AN1)
    _TRISB0 = 0; //pin 4 Lift PWM output (OC2)
    /// Does PWM need the analog disabled? 
    _TRISB1 = 0; //pin 5 Servo PWM (OC3)
    //_ANSB1 = 0;
    /// Does PWM need the analog disabled?
    _TRISB2 = 1; //Pin 6 An4 IR sensor  
    _ANSB2 = 1; //Pin 6 IR sensor 
    _TRISA2 = 0; // pin 7 QRD1114 led line
    _ANSA2 = 0; //pin 7 disable analog (AN13)
    //_TRISA3 = 1; //pin 8 QRD1114 analog line
    _ANSA3 = 1; //pin 8 enable analog (AN14)
    
    
    _TRISB7 = 0; //pin 11 debugging LED 
    _TRISB8 = 0; //pin 12 Drive Stepper direction 
    _TRISB9 = 0; //pin 13 Drive Stepper direction pin 13
    _TRISA6 = 0; //pin 14 Drive PWM Output pin 14
    //// Does the PWM need to have the analog disabled? no
    _TRISB12 = 1; //pin 15 Front Bumpers pin 15
    _ANSB12 = 0; // pin 15 disable analog
    _TRISB13 = 1; //pin 16 Rear bumpers pin 16
    _ANSB13 = 0; //pin 16 disable analog
    _TRISB14 = 0; // pin 17 Trigger LED
    _ANSB14 = 0; //pin 17 analog off 
    _TRISB15 = 0; // pin 18 Drive stepper sleep
    _ANSB15 = 0; //pin 18 disable analog
}

void _ISR _OC2Interrupt(void) {
    liftingCounter++;
   
    _OC2IF = 0; // eNABLES iNTERRUPT FLAG
}

void config_PWM_2() {
      //-----------------------------------------------------------
    // CONFIGURE PWM2 USING OC2 (on pin 4)
    
    // Clear control bits initially
    OC2CON1 = 0;
    OC2CON2 = 0;
    
  
    // Set period and duty cycle
    OC2R = 3990;                // Set Output Compare value to achieve
                                // desired duty cycle. This is the number
                                // of timer counts when the OC should send
                                // the PWM signal low. The duty cycle as a
                                // fraction is OC1R/OC1RS.
    OC2RS = 15000;               // Period of OC1 to achieve desired PWM 
                                // frequency, FPWM. See Equation 15-1
                                // in the datasheet. For example, for
                                // FPWM = 1 kHz, OC1RS = 3999. The OC1RS 
                                // register contains the period when the
                                // SYNCSEL bits are set to 0x1F (see FRM)
    
 
    
    // Configure OC2
    OC2CON1bits.OCTSEL = 0b111; // System (peripheral) clock as timing source
    OC2CON2bits.SYNCSEL = 0x1F; // Select OC1 as synchronization source
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
    OC2CON2bits.OCTRIG = 0;     // Synchronizes with OC1 source instead of
                                // triggering with the OC1 source
    OC2CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    
   
    
    _OC2IE = 1; //ENABLES YOUR INTERRUPT
    _OC2IF = 0; // eNABLES iNTERRUPT FLAG
    
    _LATA1 = 1;         //1 is up
}


int main(void) {
    
    configPins();
    config_PWM_2();

     _LATA0 = 1;     //disable sleep
     _LATA1 = 1;    //1 is up
    
     
    while(1) {
        _LATB7 = 1;     //testing LED
        if (liftingCounter > 350) {        //350 is a good number for the lifter
             _LATB7 = 0;     //testing LED
            _LATA0 = 0;     //sleep
            __delay_ms(2000);
            _LATA1 = 0;     //0 is down
            while(1) {
                //_LATB7 = 0;     //testing LED
                _LATA0 = 1;     //disables sleep
                liftingCounter = 0;
                if (liftingCounter > 350) {
                    _LATB7 = 1;     //testing LED
                    _LATA0 = 0;
                    while(1) {}
                }
            }
        }
    }

    return 0;
}
