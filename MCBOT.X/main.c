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
#include <stdbool.h>  //need for bool
#include "motions.c"
#include "config.c"
#include "interrupts.c"

// Select oscillator
#pragma config FNOSC = FRC       // 8 MHz FRC oscillator
#pragma config OSCIOFNC = OFF



enum asdf {start, sort, score, scoring, end}; //lists the possible states
int numSorted = 0;
int IRthreshold = 3000;
bool driving = false;    //true means we are driving, false means we've stopped

int main()
{
    configPins();
    config_PWM_3();     //sorting servo
    configAtoD();       //both color and IR sensors
    configTimer2();     //sorting servo
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
        // State 1
        
        // State 2
        
        // State 3
        
        // State 4
    }
 
    return 0;
}