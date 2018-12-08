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
#include <stdbool.h>  //need for bool




//Global Variables
//*************************************************
enum asdf {start, sort, score, scoring, end}; //lists the possible states
int numSorted = 0;
int IRthreshold = 3000;
bool driving = false;    //true means we are driving, false means we've stopped
int liftingCounter = 0;
int drivingCounter = 0;
int middle = 5000;
int turn180 = 1000;

//*************************************************

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

void configTimer1() {
    
    T1CONbits.TON = 1;      // turn on Timer1
    T1CONbits.TCS = 0;      // INTERNAL CLOCK
    T1CONbits.TCKPS = 0b11;     // 1:256 prescale. For more info on why its 0b11 check the data sheet
    PR1 = 15625;      // TIMER PERIOD OF 15625 is 2 seconds with the 2 MHz oscillator and 256 prescaler
    TMR1 = 0;     // RESET TIMER1 TO ZERO
    
    // Configure Timer1 interrupt
    _T1IP = 4;          // Select Timer1 interrupt priority
    _T1IE = 1;          // Enable Timer1 interrupt
    _T1IF = 0;          // Clear Timer1 interrupt flag
}

void configTimer2() {
    
    T2CONbits.TON = 1;      // turn on Timer1
    T2CONbits.TCS = 0;      // INTERNAL CLOCK
    T2CONbits.TCKPS = 0b11;     // 1:256 prescale. For more info on why its 0b11 check the data sheet
    PR2 = 1000;      // TIMER PERIOD OF 155 is 50 Hz
    TMR2 = 0;     // RESET TIMER1 TO ZERO
    
}

void configCNInterrupt() {
    // Configure Change Notification interrupt
    // Set CN interrupt priority to 6
    _CN5IE = 1; // Enable CN on pin 5 (CNEN1 register)
    _CN5PUE = 0; // Disable pull-up resistor (CNPU1 register)
    _CNIP = 6; // Set CN interrupt priority (IPC4 register)
    _CNIE = 1; // Enable CN interrupts (IEC1 register)

    // Clear Change Notification interrupt flag (IFS1 register)
    _CNIF = 0; // Clear interrupt flag (IFS1 register)
}

void _ISR _OC1Interrupt(void) {
    drivingCounter++;
    
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG
}

void _ISR _OC2Interrupt(void) {
    liftingCounter++;
//   if (counter > 600) {
//       counter = 0;
//   }
   
    _OC2IF = 0; // eNABLES iNTERRUPT FLAG
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {

    // Remember to clear the CN interrupt flag when
    // this ISR is entered.
    _CNIF = 0; // Clear interrupt flag

    // Place in this ISR whatever code should be executed
    // when a change in the button state is detected.
    // You will need to write code to distinguish between
    // a button press and a button release.
    if (_RB12 == 1) {       //Forward corner
        stopDriving();
    }
    else if (_RB13 == 1){   //Back corner
        stopDriving();
    }
    else {}                 //leaving corner
}

void configAtoD() {
        /*** Select Voltage Reference Source ***/
	// use AVdd for positive reference
	_PVCFG = 0b00;		// AD1CON2<15:14>, pg. 212-213 datasheet
	// use AVss for negative reference
	_NVCFG = 0;			// AD1CON2<13>


	/*** Select Analog Conversion Clock Rate ***/
	// make sure Tad is at least 600ns, see Table 29-41 datasheet
	_ADCS = 0b00000011;	// AD1CON3<7:0>, pg. 213 datasheet


	/*** Select Sample/Conversion Sequence ***/
	// use auto-convert
	_SSRC = 0b0111;		// AD1CON1<7:4>, pg. 211 datasheet
	// use auto-sample
	_ASAM = 1;			// AD1CON1<2>
	// choose a sample time >= 1 Tad, see Table 29-41 datasheet
	_SAMC = 0b00011;		// AD1CON3<12:8>


	/*** Choose Analog Channels to be Used ***/
	// scan inputs
	_CSCNA = 1;			// AD1CON2<10>
	// choose which channels to scan, e.g. for ch AN12, set _CSS12 = 1;
	_CSS14 = 1;			// AD1CSSH/L, pg. 217



	/*** Select How Results are Presented in Buffer ***/
	// set 12-bit resolution
	_MODE12 = 1;		// AD1CON1<10>
	// use absolute decimal format
	_FORM = 0b00;			// AD1CON1<9:8>
	// load results into buffer determined by converted channel, e.g. ch AN12 
    // results appear in ADC1BUF12
	_BUFREGEN = 1;		// AD1CON2<11>


	/*** Select Interrupt Rate ***/
	// interrupt rate should reflect number of analog channels used, e.g. if 
    // 5 channels, interrupt every 5th sample
	_SMPI = 0b00002;		// AD1CON2<6:2>


	/*** Turn on A/D Module ***/
	_ADON = 1;			// AD1CON1<15>
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
   // _OC1IF = 0; // eNABLES iNTERRUPT FLAG
    

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

void config_PWM_3() {
      //-----------------------------------------------------------
    // CONFIGURE PWM3 USING OC3 (on pin 5)
    
    // Clear control bits initially
    OC3CON1 = 0;
    OC3CON2 = 0;
    
  
    // Set period and duty cycle
    OC3R = 560;                // Set Output Compare value to achieve          //Set values between 400 and 700
                                // desired duty cycle. This is the number
                                // of timer counts when the OC should send
                                // the PWM signal low. The duty cycle as a
                                // fraction is OC1R/OC1RS.
    
    OC3RS = 9999;               // Period of OC1 to achieve desired PWM 
                                // frequency, FPWM. See Equation 15-1
                                // in the datasheet. For example, for
                                // FPWM = 1 kHz, OC1RS = 3999. The OC1RS 
                                // register contains the period when the
                                // SYNCSEL bits are set to 0x1F (see FRM)
    
 
    
    // Configure OC3
    OC3CON1bits.OCTSEL = 0b000; // System (peripheral) clock as timing source       //000 is Timer2
    OC3CON2bits.SYNCSEL = 0x1F; // Select OC1 as synchronization source
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
    OC3CON2bits.OCTRIG = 0;     // Synchronizes with OC1 source instead of
                                // triggering with the OC1 source
    OC3CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    
   
    
    //_OC3IE = 1; //ENABLES YOUR INTERRUPT
   // _OC3IF = 0; // eNABLES iNTERRUPT FLAG
    

}

void driveForward() {
    driving = true;    //sets boolean
    _LATB8 = 1;
    _LATB13 = 0;
    while(driving) {}   //switches in stopDriving function
}

void driveBackward() {
    drivingCounter = 0;
    driving = true;
    _LATB8 = 0;
    _LATB13 = 1;
    while(drivingCounter > middle) {}   //drives backward until the middle
}

void turn180() {
    drivingCounter = 0;
    _LATB8 = 1;
    _LATB13 = 1;
    while (drivingCounter > turn180) {} //turns a 180
}

void turnRight() {
    _LATB8 = 1;
    _LATB13 = 1;
}

void stopDriving() {
    driving = false;     //used in driveForward function
    OC1R = 0;            //Sets driving stepper duty cycle 
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

int main() {
    
    configPins();
    configTimer1();
    configTimer2();
    configCNInterrupt();
    configAtoD();
    config_PWM_1();
    config_PWM_2();
    config_PWM_3();
    
    
    asdf state = start;
    
    
    
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
                driveBackward();
                turn180();
                driveForward();
                raiseLift();
                state = end;
                break;
            case end:
                while(1) {}
                break;
        }
    }
    
    
    
    
    
//    while(1) {
//        if (counter > 350) {        //350 is a good number for the lifter
//            //_LATA0 = 0;
//            //OC2R = 0;
//            _LATA0 = 0;
//            //__delay_ms(1000); 
//            //_LATA0 = 1;
//            //counter = 0;
//            _LATA1 = 1 - _RA1;
//            //_LATA0 = 1;
//        }
//    }
   
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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