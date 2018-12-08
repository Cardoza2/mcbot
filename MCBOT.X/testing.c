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
#define FCY 4000000UL       //needed for delay function
#include <libpic30.h>       //needed for delay function




//Global Variables
//*************************************************
int counter = 0;
int IRthreshold = 3000;
int next = 0;
int blue = 0; 

//*************************************************



int main() {
    
    configPins();
    config_PWM_2();
    _LATA0 = 1;         //disables sleep
    _LATA1 = 1;         //1 is up
    
    
    
    
    while(1)
    {
        switch (state) {
            case start:
                findDispenser();
                state = sort;
                break;
            case sort:
                _LATA2 = 1;         //turn on IR LED
                while(numSorted < 4) {
                    _LATB14 = 1;     //turn on trigger LED
                    sort(senseColor());     //determines color and moves sorting arm
                    _LATB14 = 0;    //Turn LED off
                    OC3R = 560;     //Return sorting arm to middle
                }
                _LATA2 = 0;         //turn off IR LED
                state = end;
                break;
            case scoring:
                driveForward();
                break;
            case end:
                return 0;
                break;
        }
    }
    
    
    
    
    
    while(1) {
        if (counter > 350) {        //350 is a good number for the lifter
            //_LATA0 = 0;
            //OC2R = 0;
            _LATA0 = 0;
            //__delay_ms(1000); 
            //_LATA0 = 1;
            //counter = 0;
            _LATA1 = 1 - _RA1;
            //_LATA0 = 1;
        }
    }
   
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //Milestone 10
//    configPins();
//    config_PWM_1();
//    configCNInterrupt();
//    configAtoD();
//    
//    driveForward();
//    while(next == 0) {}
//    blue = 1;
//    next = 0;
//    findGoal();
//    //while(next == 0) {}
//    next == 0;
//    counter = 0;
//    blue = 0;
//    driveForward();
//    while(1){}
//    while(next == 0) {}
    

    //_LATB7 = 0;
//    while(1) {
//        if (ADC1BUF14 > IRthreshold) {
//            _LATB7 = 1;
//        }
//        else {
//            _LATB7 = 0;
//        }
        
//        if (_RB12 = 1) {  // We ran into an issue where the switches aren't going low.
//            _LATB7 = 0;
//        }
//        else {
//            _LATB7 = 1;
//        }
    //}
    
   
    return 0;
}