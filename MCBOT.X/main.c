/*
 * File:   main.c
 * Author: MCBOT Team
 *
 * Created on October 29, 2018, 3:45 PM
 * 
 * Globals:
 * numSorted - number of balls sorted in current round
 * state - current state
 * 
 * Functions:
 *  Forward() - Drive forward until interruption
 *  Reverse() - Drive backwards until interruption 
 *  Right() - Turn right. 
 *      Input is in degrees.
 *  Left()
 *  Dump()
 *  Sort()
 *  Trigger()
 *  
 * 
 * States:
 * 
 * 
 * Interrupts:
 *  Timer1 - Main timer that keeps track of the parts of the round
 *  Front_Bumpers
 *  Back_Bumpers
 *  IR_Sensor
 * 
 */




#include "xc.h"

// Select oscillator
#pragma config FNOSC = FRC       // 8 MHz FRC oscillator



enum asdf {start, sort, score, end}; //lists the possible states
int numSorted = 0;
int IRthreshold = 3000;

int main()
{
    configPins();
    config_PWM_3();
    configAtoD();
    configTimer2();
    asdf state = start; //possibly start in the findDispenser state?
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
                state = end;
                break;
            case end:
                return 0;
                break;
        }
        // State 1
        
        // State 2
        
        // State 3
        
        // State 4
    }
 
    return 0;
}