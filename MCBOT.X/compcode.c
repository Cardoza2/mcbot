/*
 * File:   compcode.c
 * Author: AdamC
 *
 * Created on December 10, 2018, 10:18 PM
 */


#pragma config FNOSC = FRC
#pragma config ICS = PGx3
#include "xc.h"
#define FCY 4000000UL       //needed for delay function
#include <libpic30.h>       //needed for delay function
#include <stdbool.h>        //need for bool
#pragma config OSCIOFNC = OFF   //I'm assuming this is what allows pin 8 to work

int numSorted = 0;
int blkSorted = 0;
int whtSorted = 0;
int ballLimit = 8;
int IRthreshold = 2450;     //3000 in actual arena 2450 with clicker
bool driving = false;    //true means we are driving, false means we've stopped
bool creeping = false;  //creeping is designed to ignore the interrupt when we want to wedge into a corner
int drivingCounter = 0;
int liftingCounter = 0;
int middle = 530;
int clicksTo180 = 340;
int black = 620;
int white = 3700;
enum asdf {start, sorting, scoring, end}; //lists the possible states
char scoreColor = 0;
int timer = 0;


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
     _TRISB8 = 0; //pin 12 Drive Stepper direction (YELLOW)) 0 is forward
    _TRISB9 = 0; //pin 13 Drive Stepper direction pin 13 (ORANGE)) 1 is forward
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
    PR1 = 15625;      // TIMER PERIOD OF 15625 is 1 second with the 4 MHz oscillator and 256 prescaler
    TMR1 = 0;     // RESET TIMER1 TO ZERO
    
    // Configure Timer1 interrupt
    _T1IP = 6;          // Select Timer1 interrupt priority
    _T1IE = 1;          // Enable Timer1 interrupt
    _T1IF = 0;          // Clear Timer1 interrupt flag
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {       //used for round timer
    // Clear Timer1 interrupt flag so that the program doesn't
    // just jump back to this function when it returns to the
    // while(1) loop.
    _T1IF = 0;

    TMR1 = 0;
    
    timer++;
    if (timer >= 105) {
        while(1) {}
    }
}

void configTimer2() {
    
    T2CONbits.TON = 1;      // turn on Timer1
    T2CONbits.TCS = 0;      // INTERNAL CLOCK
    T2CONbits.TCKPS = 0b01;     // 1:256 prescale. For more info on why its 0b11 check the data sheet
    PR2 = 1000;      // TIMER PERIOD OF 155 is 50 Hz
    TMR2 = 0;     // RESET TIMER1 TO ZERO
    
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
	_CSS14 = 1;			// AD1CSSH/L, pg. 217   //QRD sensor
    _CSS4 = 1;          //IR sensor



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
	_SMPI = 0b00010;		// AD1CON2<6:2>


	/*** Turn on A/D Module ***/
	_ADON = 1;			// AD1CON1<15>
}

void config_PWM_3() {
      //-----------------------------------------------------------
    // CONFIGURE PWM3 USING OC3 (on pin 5) servo
    
    // Clear control bits initially
    OC3CON1 = 0;
    OC3CON2 = 0;
    
  
    // Set period and duty cycle
    OC3R = 560;                // Set Output Compare value to achieve          //Set values between 500 and 1000
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

void configCNInterrupt() {
    // Configure Change Notification interrupt
    // Set CN interrupt priority to 6
    _CN14IE = 1; // Enable CN on pin 5 (CNEN1 register)
    _CN5PUE = 0; // Disable pull-up resistor (CNPU1 register)
    _CNIP = 5; // Set CN interrupt priority (IPC4 register)
    _CNIE = 1; // Enable CN interrupts (IEC1 register)

    // Clear Change Notification interrupt flag (IFS1 register)
    _CNIF = 0; // Clear interrupt flag (IFS1 register)
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
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG
    
    _LATB15 = 0;    //sleep
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
    

}

void __attribute__((interrupt, auto_psv)) _OC1Interrupt(void) {
    drivingCounter++;
    
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG
}

void __attribute__((interrupt, auto_psv)) _OC2Interrupt(void) {
    liftingCounter++;  
    _OC2IF = 0; // eNABLES iNTERRUPT FLAG
}

void turnRight() {
    _LATB15 = 1;    //disable sleep
    _LATB8 = 1;
    _LATB9 = 1;
}

void driveForward() {
    _LATB15 = 1;    //disables sleep
    driving = true;    //sets boolean
    _LATB8 = 0;
    _LATB9 = 1;
    while(driving) {}   //switches in stopDriving function
}

void creepForward() {
    _LATB7 = 0; //test led
    creeping = true;
    OC1RS = 25000;
    _LATB15 = 1;    //disables sleep
    driving = true;    //sets boolean
    _LATB8 = 0;
    _LATB9 = 1;
    drivingCounter = 0;
    while(drivingCounter < 200) {}   //switches in stopDriving function
    OC1RS = 15000;
    creeping = false;
}

void stopDriving() {
    driving = false;     //used in driveForward function
    _LATB15 = 0;    //sleep
}

void driveBackward() {
    _LATB15 = 1;    //disables sleep
    drivingCounter = 0;
    _LATB8 = 1;
    _LATB9 = 0;
    while(drivingCounter < middle) {}   //drives backward until the middle
    _LATB15 = 0;    //sleep
}

void findActive() {
    _LATB7 = 1; //test led
    turnRight();
    while(ADC1BUF4 < IRthreshold) {}
    stopDriving();
}

void turn180() {
    _LATB15 = 1;    //disables sleep
    drivingCounter = 0;
    _LATB8 = 1;
    _LATB9 = 1;
    while (drivingCounter < clicksTo180) {} //turns a 180 to the right
    _LATB15 = 0;    //sleep
}

void turnRight90() {
    _LATB15 = 1;    //disables sleep
    drivingCounter = 0;
    _LATB8 = 1;
    _LATB9 = 1;
    while (drivingCounter < clicksTo180/2) {} //turns a 90 to the right
    _LATB15 = 0;    //sleep
}

void turnLeft90() {
    _LATB15 = 1;    //disables sleep
    drivingCounter = 0;
    _LATB8 = 0;
    _LATB9 = 0;
    while (drivingCounter < clicksTo180/2) {} //turns a 90 to the left
    _LATB15 = 0;    //sleep
}

void sort(char color) {
    
    if (color == 'w') {
        OC3R = 700;
        numSorted++;
        whtSorted++;
    }
    else if (color == 'b') {
        OC3R = 400;
        numSorted++;
        blkSorted++;
    }
    __delay_ms(300);
    return;
}

char senseColor() {
    char color;
    while(1) {
        if (ADC1BUF14 > white) {      //3700 is approx 3V
            color = 'w';
            return color;
        }
        else if (ADC1BUF14 > black) {     //1700 is approx 1.4V
            color = 'b';
            return color;
        }
        else {
            color = 'n';            //means there is nothing there
            _LATB14 = 0;    //trigger LED
            __delay_ms(300);
            _LATB14 = 1;    //trigger LED
        }
    }
    
    return color;
}

void score() {
    /// scoring based on color determined in deciding function
    driveForward();
    _LATB15 = 0;    //sleep wheels
    
    if (scoreColor == 'b') {    //move servo depending on color
        OC3R = 400;
        blkSorted = 0;
    } else {
        OC3R = 760;
        whtSorted = 0;
    }
    
    _TRISA0 = 1;    //unsleep lift
    liftingCounter = 0;
    while (1) {
        if (liftingCounter > 590) { //350 is a good number for the lifter, or 590 in the latest tests 
            _LATA0 = 0; //sleep lift
            __delay_ms(2000);
            _LATA1 = 0; //0 is down
            _LATA0 = 1; //disables sleep on lift
            liftingCounter = 0;
            while (1) {
                if (liftingCounter > 590) {
                    _LATA0 = 0; //sleep lift
                }
            }
        }
    }
    _LATB15 = 1;    //unsleep wheels
    driveBackward();
}

void decide() {
    turnRight90();
    if (whtSorted >= 4 && blkSorted >= 4) {     //lots of white and black
        if (ADC1BUF4 >= IRthreshold) {      //black first
            scoreColor = 'b';
            score();                    //score black
            turnRight90();
            scoreColor = 'w';       
            score();                    //score white
            turn180();
        }
        else {                              //white first
            scoreColor = 'w';
            score();                    //score white
            turnRight90();
            if (ADC1BUF4 >= IRthreshold) { //find black goal
                scoreColor = 'b';
                score();                    //score black
                turn180();
            }
            else {
                turnRight90();
                if (ADC1BUF4 >= IRthreshold) { //find black goal
                    scoreColor = 'b';
                    score();               //score black
                }
                turnRight90();
            }
        }
        return;
    }
    
    else if(whtSorted >= 4 && blkSorted <= 4) {     //more white
        scoreColor = 'w';
        if (ADC1BUF4 < IRthreshold) {   //white goal
            score();                    //score white
            turnLeft90();
        }   
        else {                          //saw black goal first
            turnRight90();
            score();                    //score white
            turn180();
        }
        return;
    }
    
    else if (whtSorted <= 4 && blkSorted >= 4) {    //more black
        scoreColor = 'b';
        if (ADC1BUF4 >= IRthreshold) { //find black goal
                score();                    //score black
                turnLeft90();
        }
        else {
            turnRight90();
            if (ADC1BUF4 >= IRthreshold) { //find black goal
                score();                    //score black
                turn180();
            }
            else {
                turnRight90();
                score();                    //score Black
                turnRight90();
            }
        }
        return;
    }
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {

    // Remember to clear the CN interrupt flag when
    // this ISR is entered.
    
    // Place in this ISR whatever code should be executed
    // when a change in the button state is detected.
    // You will need to write code to distinguish between
    // a button press and a button release.

    //__delay_ms(250);
    _CNIF = 0; // Clear interrupt flag
    
    if (creeping) {
        return;
    }
    
    if (_RB12 == 1) {       //Forward corner
        stopDriving();
    }
    else if (_RB13 == 1){   //Back corner
        stopDriving();
    }
    else {}                 //leaving corner
}

enum asdf state = start;

int main() {
    
    configPins();
    configTimer1();
    configTimer2();
    config_PWM_1();
    config_PWM_2();
    config_PWM_3();
    configCNInterrupt();
    configAtoD();
    
    
    
    
    _LATB14 = 1;
    while(1) {}
    
    
    
    //Delay to get your hands out of the way
    __delay_ms(500);
    
    //enum asdf {start, sorting, scoring, end}; //lists the possible states
            //Note, when switching states, the state doesn't switch until it has finished the case, then it changes to the case that matches the most 
                //recent value for state.
    while(1){
        switch(state){
            case start:
                /// Starting the round and getting us to the corner ///
                findActive();
                //__delay_ms(100);
                driveForward();
                //_LATB7 = 0; //test led
                creepForward();
                stopDriving();
                //Switch state to sorting       
                state = sorting;
                break;
                
            case sorting:
                //_LATB7 = 0;     //test Led
                
                /// Sorting balls and counting how many we have ///
                
                // set servo to center
                 OC3R = 760;
                __delay_ms(300);
                OC3R = 400;
                __delay_ms(300);
                OC3R = 560;
                __delay_ms(300);
                
                                
                //While loop to collect balls
                while(whtSorted <= ballLimit && blkSorted <= ballLimit) {            
                    _LATB14 = 1;     //turn on trigger LED
                    _LATA2 = 1; //turn on QRD LED
                    sort(senseColor());     //determines color and moves sorting arm
                    
                    OC3R = 560;     //Return sorting arm to middle
                    _LATB14 = 0;    //Turn trigger LED off
                    _LATA2 = 0; //turn off QRD LED
                    __delay_ms(300);
                }
                state = scoring;
                break;
                
            case scoring:
                
                driveBackward();
                decide();
                driveForward();
                
                state = sorting;
                break;
                
            case end:
                //turn everything off and lock in a loop    //although with the interrupt loop lock we don't need this.
                _LATA0 = 0;     //sleep lift
                _LATB15 = 0;    //sleep wheels
                while(1){}
                break;
        }
    }
    
    
    
    
    //precautionary while loop to stop the bot
    while(1){}
    return 0;
}
