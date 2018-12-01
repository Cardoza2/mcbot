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

void findDispenser() {
    turnRight();
    while(ADC1BUF14 < IRthreshold) {}
    //_LATB7 = 1;   //Test LED
    stopDriving();
    driveForward();
}

void sort(char color) {
    if (color == 'b') {
        OC3R = 400;
        numSorted++;
    }
    else if (color == 'w') {
        OC3R = 700;
        numSorted++;
    }
    else {
        OC3R = 560;
    }
}

char senseColor() {
    char color;
    if (ADCBUF14 > 3700) {      //3700 is approx 3V
        color = 'w';
    }
    else if (ADCBUF14 > 1700) {     //1700 is approx 1.4V
        color = 'b';
    }
    else {
        color = 'n';            //means there is nothing there
    }
    
    return color;
}