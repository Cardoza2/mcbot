/*
 * File:   main.c
 * Author: MCBOT Team
 *
 * Created on October 29, 2018, 3:45 PM
 * 
 * Globals:
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

void config_ad()
{

	/*** Configure Desired Port Pins as Analog Inputs ***/
	_TRISB4 = 1;		// TRISA/B, pg. 45 datasheet
	_ANSB4 = 1;			// ANSA/B, pg. 136-137


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
	_CSS2 = 1;			// AD1CSSH/L, pg. 217 FIGURE ME OIT
 


	/*** Select How Results are Presented in Buffer ***/
	// set 12-bit resolution
	_MODE12 = 1;		// AD1CON1<10>
	// use absolute decimal format
	_FORM = 0b00;			// AD1CON1<9:8>
	// load results into buffer determined by converted channel, e.g. ch AN12 
    // results appear in ADC1BUF12
	_BUFREGEN = 1;		// AD1CON2<11>


	/*** Select Interrupt Rate ***/ // FIGURE ME OUT
	// interrupt rate should reflect number of analog channels used, e.g. if 
    // 5 channels, interrupt every 5th sample

     _SMPI = 0b00001;		// AD1CON2<6:2>


	/*** Turn on A/D Module ***/
	_ADON = 1;			// AD1CON1<15>
}

// Drive forward Function


// Turn right Function

// 

int main()
{
    _TRISA2 = 0; //PIN 7
    
    
    // Clear control bits initially
    OC3CON1 = 0;
    OC3CON2 = 0;
   
    // Set period and duty cycle
    OC3R = 800;                // Set Output Compare value to achieve
                                // desired duty cycle. This is the number
                                // of timer counts when the OC should send
                                // the PWM signal low. The duty cycle as a
                                // fraction is OC1R/OC1RS.
    OC3RS = 3999;               // Period of OC1 to achieve desired PWM 
                                // frequency, FPWM. See Equation 15-1
                                // in the data sheet. For example, for
                                // FPWM = 1 kHz, OC1RS = 3999. The OC1RS 
                                // register contains the period when the
                                // SYNCSEL bits are set to 0x1F (see FRM)
    
    // Configure OC1
    OC3CON1bits.OCTSEL = 0b111; // System (peripheral) clock as timing source
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
    
    
    //-----------------------------------------------------------
    // RUN
    config_ad();

    // Wait and let the PWM do its job behind the scenes
    while(1)
    {
        // State 1
        
        // State 2
        
        // State 3
        
        // State 4
    }
 
    return 0;
}