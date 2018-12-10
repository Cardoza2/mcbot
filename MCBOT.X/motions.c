/*
 * File:   motions.c
 * Author: jonah
 *
 * Created on November 9, 2018, 3:06 PM
 */

void driveForward() {
    _LATB15 = 1;    //disables sleep
    driving = true;    //sets boolean
    _LATB8 = 0;
    _LATB13 = 0;
    while(driving) {}   //switches in stopDriving function
}

void driveBackward() {
    //_LATB7 = 1;     //testing LED
    drivingCounter = 0;
    //driving = true;
    _LATB8 = 1;
    _LATB9 = 0;
    while(drivingCounter < middle) {}   //drives backward until the middle
    //_LATB7 = 0;     //testing LED
    _LATB15 = 0;    //sleep
}

void turn180() {
    drivingCounter = 0;
    _LATB8 = 1;
    _LATB9 = 1;
    while (drivingCounter < clicksTo180) {} //turns a 180 to the right
    _LATB15 = 0;    //sleep
}

void turnRight() {
    _LATB15 = 1;    //disable sleep
    _LATB8 = 1;
    _LATB9 = 1;
}

void stopDriving() {
    driving = false;     //used in driveForward function
    //OC1R = 0;            //Sets driving stepper duty cycle 
    _LATB15 = 0;    //sleep
}

void raiseLift() {
    while(1) {
         if (liftingCounter > 500 ) {
             _LATB13 = 0;
         }
    }
}

void findDispenser() {
    turnRight();
    while(ADC1BUF4 < IRthreshold) {}
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
    if (ADC1BUF14 > 3700) {      //3700 is approx 3V
        color = 'w';
    }
    else if (ADC1BUF14 > 1700) {     //1700 is approx 1.4V
        color = 'b';
    }
    else {
        color = 'n';            //means there is nothing there
    }
    
    return color;
}