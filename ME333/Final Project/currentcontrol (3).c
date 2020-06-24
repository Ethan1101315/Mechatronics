// Ethan Piper
// ME333
// currentcontrol.c

#include "currentcontrol.h"
#include "NU32.h"


void currentcontrol_init(void)	{
// CONFIGURE THE TIMER 2 INTERRUPT
									
									// step 3: configure the peripheral, Timer2. It will roll over 5000 times per second, triggering the ISR each time. 
										
	T2CONbits.TCKPS = 1; 			// Set up Timer 2 with a prescaler value of 2. 
	PR2 = 7999;					// T = (PR2+1) * prescaler * 12.5 ns. 
	TMR2 = 0;
	T2CONbits.ON = 1;
	
	
	
	IPC2bits.T2IP = 5;            	// step 4: interrupt priority 5
	IFS0bits.T2IF = 0;          	// step 5: clear the int flag
	IEC0bits.T2IE = 1;            	// step 6: enable Timer2 by setting IEC0<8>
									

// CONFIGURE TIMER 3 FOR USE WITH OUTPUT COMPARE. 
	T3CONbits.TCKPS = 0; // Set up Timer 3 with a prescaler value of 1. 

	PR3 = 3999; // Contains the maximum value of the count of TMR3 before resetting to 0 on the next count. 
			// Calculated as PR = (80 MHz) / (prescaler) / (desired frequency) the desired frequency is 20 kHz. 
	TMR3 = 0; // Initialize the count of TMR3 to 0. 

	OC1CONbits.OCTSEL = 1;	// Sets Timer3 as the clock source for OC1. (Timer2 is the default).	OC1 = D0. 
	OC1CONbits.OCM = 0b110; // Sets the operating mode of OC1 to PWM mode with no fault pin. 
	OC1RS = 1000;
	OC1R = 1000; // duty cycle = 25% = OC1RS / (PR3+1).

	T3CONbits.ON = 1; // Turning on Timer 3. 

	OC1CONbits.ON = 1; // Turning on OC1. 
	
// SET THE DIGITAL OUTPUT PIN RD6. 
	TRISDbits.TRISD6 = 0;		// sets RD6 to be an output (it is an input by default). 

}

int PI_Controller(int adcval, int Waveform) {
	static int e;
	static int u;
	/* static int unew; */
	volatile extern float Ki, Kp;		// define as extern so that this function can receive these variables from main.c
	volatile extern int	eint;
	
	e = adcval - Waveform; // e = sensed - ref
	eint = eint + e;
	if (eint > EINTMAX)		// anti-integrator windup 
	{
		eint = EINTMAX;
	}
	else if (eint < -EINTMAX)
	{
		eint = -EINTMAX;
	}
	u = Kp*e + Ki*eint; 
	return u;
}