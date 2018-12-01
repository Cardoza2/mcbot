/*
 * File:   config.c
 * Author: MCBOT Team
 *
 * Created on November 1, 2018, 3:58 PM
 */

void configTimer2() {
    
    T2CONbits.TON = 1;      // turn on Timer1
    T2CONbits.TCS = 0;      // INTERNAL CLOCK
    T2CONbits.TCKPS = 0b11;     // 1:256 prescale. For more info on why its 0b11 check the data sheet
    PR2 = 1000;      // TIMER PERIOD OF 155 is 50 Hz
    TMR2 = 0;     // RESET TIMER1 TO ZERO
    
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