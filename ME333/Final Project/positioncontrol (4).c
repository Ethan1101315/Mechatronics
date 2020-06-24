// Ethan Piper
// ME333
// positioncontrol.c

#include "positioncontrol.h"
#include "NU32.h"

void positioncontrol_init(void)	{
	// CONFIGURE THE TIMER 4 INTERRUPT
									
									// step 3: configure the peripheral, Timer4. It will roll over 200 times per second, triggering the ISR each time. 
										
	T4CONbits.TCKPS = 0b100; 		// Set up Timer 4 with a prescaler value of 16. 
	PR4 = 24999;					// T = (PR4+1) * prescaler * 12.5 ns. 
	TMR4 = 0;						// initialize the timer to 0. 
	T4CONbits.ON = 1;				// turns on the timer. 
	
	
	
	IPC4bits.T4IP = 4;            	// step 4: interrupt priority 4
	IFS0bits.T4IF = 0;          	// step 5: clear the int flag
	IEC0bits.T4IE = 1;            	// step 6: enable Timer4. 
	
	// SET THE DIGITAL OUTPUT PIN RD6. 
	TRISDbits.TRISD10 = 0;		// sets RD10 to be an output (it is an input by default). 
}

float PID_Controller(float actual_position, float desired_position) {		// PID controller for position controller. 
	static float e_position;
	static float u_position;
	volatile static float edot;
	volatile extern float Ki_position, Kd_position, Kp_position;		// define as extern so that this function can receive these variables from main.c
	volatile extern float eint_position;
	volatile extern float eprev;
	
	e_position = desired_position - actual_position; // e = ref - sensed
	eint_position = eint_position + e_position; // integral error term
	edot = e_position - eprev;	// derivative error term
	eprev = e_position;
	if (eint_position > EINTMAX_POSITION)		// anti-integrator windup 
	{
		eint_position = EINTMAX_POSITION;
	}
	else if (eint_position < -EINTMAX_POSITION)
	{
		eint_position = -EINTMAX_POSITION;
	}
	u_position = Kp_position*e_position + Kd_position*edot + Ki_position*eint_position; 
	
	return u_position;
}

