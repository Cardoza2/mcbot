/*
 * File:   motions.c
 * Author: jonah
 *
 * Created on November 9, 2018, 3:06 PM
 */

void driveForward() {
    _LATB8 = 1;
    _LATB13 = 0;
}

void turnRight() {
    _LATB8 = 1;
    _LATB13 = 1;
}

void raiseLift() {
    while(1) {
         if (counter > 500 ) {
             _LATB13 = 0;
         }
    }
}