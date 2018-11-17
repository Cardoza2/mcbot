/*
 * File:   testing.c
 * Author: MCBOT Team
 * 
 * Created on November 3, 2018, 10:34 AM
 * 
 * This file is for testing functions before adding them to the project
 */

#pragma config FNOSC = FRC
//#pragma config ICS = PGx3
#include "xc.h"




//Global Variables
//*************************************************
int counter = 0;
int IRthreshold = 3000;

//*************************************************

void configPins() {
    _TRISB7 = 0;       //LED pin 11 temp 3
    _TRISB8 = 0;        //Stepper direction pin 12
    _TRISB9 = 0;        //Stepper direction pin 13
    _TRISA6 = 0;        // PWM Output
    _TRISB13 = 0;       //Rear bumpers pin 16
    _TRISB12 = 1;       //Front Bumpers pin 15
    _TRISA3 = 1;    
    _ANSA3 = 1;        //IR sensor pin 8
}

void _ISR _OC1Interrupt(void)
{
    counter++;
//    if (counter > 100) {
//        stopDriving();
//    }
   
    _OC1IF = 0; // eNABLES iNTERRUPT FLAG
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

configCNInterrupt() {
    // Configure Change Notification interrupt
    // Set CN interrupt priority to 6
    // PLACE CODE TO CONFIGURE CN INTERRUPT HERE
    _CN5IE = 1; // Enable CN on pin 5 (CNEN1 register)
    _CN5PUE = 0; // Disable pull-up resistor (CNPU1 register)
    _CNIP = 6; // Set CN interrupt priority (IPC4 register)
    _CNIE = 1; // Enable CN interrupts (IEC1 register)

    // Clear Change Notification interrupt flag (IFS1 register)
    // PLACE CODE TO CLEAR THE CN INTERRUPT FLAG HERE
    _CNIF = 0; // Clear interrupt flag (IFS1 register)
}

// Change Notification Interrupt Service Routine (ISR)
// This function executes every time the micro receives
// an interrupt originating from any of the CN pins. The
// micro knows the interrupt is from the one of the CN 
// when the change notification interrupt flag (CNIF)
// is set.

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{

    // Remember to clear the CN interrupt flag when
    // this ISR is entered.
    // PLACE CODE TO CLEAR THE CN INTERRUPT FLAG HERE
    _CNIF = 0; // Clear interrupt flag

    // Place in this ISR whatever code should be executed
    // when a change in the button state is detected.
    // You will need to write code to distinguish between
    // a button press and a button release.
    // PLACE CUSTOM CODE HERE
    if (_RB12 == 1)     //Forward corner
    {
        stopDriving();
    }
    else if (_RB13 == 1){   //Back corner
        stopDriving();
    }
    else                    //out of corner
    {
        
    }
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
	_SMPI = 0b00001;		// AD1CON2<6:2>


	/*** Turn on A/D Module ***/
	_ADON = 1;			// AD1CON1<15>
}

void stopDriving() {
    OC1R = 0;            //Sets driving stepper duty cycle 
}

void driveForward() {
    OC1R = 3990;
    _LATB8 = 1;
    _LATB9 = 0;
}

void turnRight() {
    OC1R = 3990;
    _LATB8 = 1;
    _LATB9 = 1;
}

void findGoal() {
    turnRight();
    while(ADC1BUF14 < IRthreshold) {}
}

int main() {
    
    configPins();
    config_PWM_1();
    //configCNInterrupt();
    //configAtoD();
    
//    
//    driveForward();
//    findGoal();
//    driveForward();
    while(1){
        driveforward();
    }
 
    

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
    }
    
   
    return 0;
}