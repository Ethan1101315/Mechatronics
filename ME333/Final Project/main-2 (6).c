// Ethan Piper
// ME333
// main.c - motor control project

#include "NU32.h"          // config bits, constants, funcs for startup and UART
// include other header files here
#include "encoder.h" // encoder module
#include "utilities.h" // utilities module
#include "currentcontrol.h" // current control module
#include "isense.h"	// current sensor module
#include "positioncontrol.h" // position control module

#define BUF_SIZE 200

volatile float PWM_duty_cycle = 0;
static int counter = 0;		// counter for the ITEST case in the ISR. 
volatile int Waveform[100];		// reference current waveform itself
static volatile int current = 200;
volatile int eint = 0;				
volatile float Kp = 0, Ki = 0;
volatile int unew;
volatile int actual_current_waveform[100];
volatile float Kp_position=0, Kd_position=0, Ki_position=0;
volatile float desired_angle = 0;
volatile float eint_position = 0;		// for the position PID controller. 
volatile float eprev = 0;
volatile float u_position;
volatile int num_steps;
volatile float reference_position[2000];
static int trajectory_counter = 0;
volatile float actual_position_waveform[2000];

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) CurrentController(void) {	//ISR called by Timer2 at a frequency of 5 kHz. 
	switch (get_mode()) {		//set the duty cycle and direction bit depending on the mode. 
								//the digital output controls the phase of the H-bridge and the PWM controls the ENABLE pin. 
	case IDLE:									
	{
		OC1RS = 0; 	// for braking, AENABLE = 0 and APHASE = anything. 
		break;									
	}		
	case PWM:									
	{
		if (PWM_duty_cycle>0) {
			LATDCLR = 0b1000000;		// set the H-bridge for forward motion if the PWM value is positive. 
			OC1RS = (PWM_duty_cycle * (PR3+1)) / (100); 
		}
		else if (PWM_duty_cycle<0) {
			LATDSET = 0b1000000;		// set the H-bridge for backwards motion if the PWM value is negative.
			OC1RS = ((PWM_duty_cycle * (PR3+1)) / (100)); 
		}			
		else if (PWM_duty_cycle==0) {
			OC1RS = 0;
		}
		break;		
	}
	case ITEST:
	{
		Waveform[counter] = current;
		unew = PI_Controller((1.443*(ADC_read(0))-733.2), Waveform[counter]);			// PI controller using actual current in mA and desired waveform. 
		if (unew < -100)		// make sure the control effort is positive and does not exceed 100 (it will be used to give a percent control effort)
		{
			unew = -100;
		}
		else if (unew > 100)
		{
			unew = 100;
		}
		if (unew < 0)
		{	unew = -unew;
			LATDCLR = 0b1000000;
		}
		else if (unew > 0)
		{
			LATDSET = 0b1000000;
		}
		OC1RS = ((unew/100.0) * PR3); 	// Updates the duty cycle based on PI controller. 
		actual_current_waveform[counter] = (1.443*(ADC_read(0))-733.2);
		if (counter == 25 || counter == 50 || counter == 75)
		{
			current = -current;
			/* if (current > 0)
			{
			current = -200;
			LATDSET = 0b1000000;		// set the direction bit to 1 when current is set to be negative. 
			}
			else if (current < 0)
			{
			current = 200;
			LATDCLR = 0b1000000;		// set the direction bit to 0 when current is set to be positive. 
			} */
		}
		if (counter == 99)
		{
			counter = 0;
			set_mode(IDLE);				// Set the mode to IDLE at the end of the current loop test. 
		}
		counter++;
		break;
	}
	case HOLD:
	{ 
		unew = PI_Controller((1.443*(ADC_read(0))-733.2), u_position);			// PI controller using actual current in mA and commanded current from the position control loop.  
		if (unew < -100)		// make sure the control effort is positive and does not exceed 100 (it will be used to give a percent control effort)
		{
			unew = -100;
		}
		else if (unew > 100)
		{
			unew = 100;
		}
		
		if (unew < 0)
		{	
			unew = -unew;
			LATDCLR = 0b1000000;
		}
		else if (unew > 0)
		{
			LATDSET = 0b1000000;
		}
		OC1RS = ((unew/100.0) * PR3); 	// Updates the duty cycle based on PI controller. 
		
		break;
	}	
	case TRACK: 
	{
		unew = PI_Controller((1.443*(ADC_read(0))-733.2), u_position);			// PI controller using actual current in mA and commanded current from the position control loop.  
		if (unew < -100)		// make sure the control effort is positive and does not exceed 100 (it will be used to give a percent control effort)
		{
			unew = -100;
		}
		else if (unew > 100)
		{
			unew = 100;
		}
		
		if (unew < 0)
		{	
			unew = -unew;
			LATDCLR = 0b1000000;
		}
		else if (unew > 0)
		{
			LATDSET = 0b1000000;
		}
		OC1RS = ((unew/100.0) * PR3); 	// Updates the duty cycle based on PI controller. 
		
		break;
	}
	}
	IFS0bits.T2IF = 0; 				// clears the interrupt flag. 

}

void __ISR(_TIMER_4_VECTOR, IPL4SOFT) PositionController(void) {		// 200 Hz position control ISR.
	LATDINV = 0b10000000000;		// invert the digital output pin RD10. 
	if (get_mode()==HOLD)
	{	
		u_position = PID_Controller(((encoder_counts()-32768.0) / (1792.0)) * (360.0), desired_angle); // takes as input the actual encoder value in degrees and the desired angle. 
	}
	if (get_mode()==TRACK)
	{
		u_position = PID_Controller(((encoder_counts()-32768.0) / (1792.0)) * (360.0), reference_position[trajectory_counter]);
		actual_position_waveform[trajectory_counter] = (((encoder_counts()-32768.0) / (1792.0)) * (360.0));
		if (trajectory_counter==num_steps)
		{
			desired_angle = reference_position[trajectory_counter];
			set_mode(HOLD);
		}
		trajectory_counter++;
	}
	IFS0bits.T4IF = 0; 				// clears the interrupt flag. 
} 
	
int main() 
{
  char buffer[BUF_SIZE];
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;        
  set_mode(IDLE); // set the mode to IDLE.
  __builtin_disable_interrupts();
  encoder_init(); // Initializes SPI4
  ADC_init();		// Initializes the ADC. 
  currentcontrol_init();	// Initializes timer 2 for an ISR, timer 3 for output compare, and digital output pin RD6 for controlling the motor. 
  positioncontrol_init();   // Initializes timer 4 for an ISR. 
  __builtin_enable_interrupts();
  volatile float ADC_milliamps;

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
	case 'a':						// READ CURRENT SENSOR (ADC COUNTS)				
		sprintf(buffer, "%d\n\r", ADC_read(0));	// reads ADC value from pin AN0. 
		NU32_WriteUART3(buffer); // send ADC value to MATLAB
		break;
	case 'b':					// READ CURRENT SENSOR (mA)
		ADC_milliamps = 1.443*(ADC_read(0))-733.2;		// used linear fit equation to convert counts to milliamps. 
		sprintf(buffer, "%f\n\r", ADC_milliamps);
		NU32_WriteUART3(buffer); // send ADC value to MATLAB
		break;
	case 'c':                      // READ ENCODER (COUNTS)
      {
        sprintf(buffer, "%d\n\r", encoder_counts());
		NU32_WriteUART3(buffer); // send encoder count to MATLAB
        break;
      }
      case 'd':                      // READ ENCODER (DEG)
      {
		float degrees;
		degrees = ((encoder_counts()-32768.0) / (1792.0)) * (360.0); // there are 1792 encoder counts per revolution. 
        sprintf(buffer, "%f\n\r", degrees);
		NU32_WriteUART3(buffer); // send encoder count to MATLAB
		break;
      }
	  case 'e':                      // RESET ENCODER
      {
        encoder_reset();
        break;
	  }
	  case 'f':				// SET PWM (-100 to 100)
	  {
		set_mode(PWM);			// set the mode to PWM mode. 
		NU32_ReadUART3(buffer,BUF_SIZE); 
		sscanf(buffer, "%f", &PWM_duty_cycle);
		
		if (PWM_duty_cycle < -100) {			// saturate the PWM duty cycle. 
			PWM_duty_cycle = -100;
		}
		else if (PWM_duty_cycle > 100) {
			PWM_duty_cycle = 100;	
		}		
		break;
      }
	  case 'g':				// SET CURRENT GAINS
	  {
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f", &Kp, &Ki);
		  break;
	  }
	  case 'h':				// GET CURRENT GAINS
	  {
		sprintf(buffer,"%f %f\r\n", Kp, Ki); // return the proportional and integral gains. 
        NU32_WriteUART3(buffer);
		  break;
	  }
	  case 'i':				// SET POSITION GAINS
	  {
		NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f %f", &Kp_position, &Kd_position, &Ki_position); 	// retrieve position gains from MATLAB. 
		break;
	  }
	  case 'j': 			// GET POSITION GAINS
	  {
		sprintf(buffer,"%f %f %f\r\n", Kp_position, Kd_position, Ki_position); // return the position gains. 
        NU32_WriteUART3(buffer);
		  break;
	  }
	  case 'k':				// TEST CURRENT GAINS
	  {
		  static int i = 0;
		  if (i == 100)
		  {
			  i = 0;
		  }
		  set_mode(ITEST);
		  while (get_mode() == ITEST) {;}	// Infinite while loop until mode is set back to IDLE. 
		  sprintf(buffer,"%d\r\n", 100);	// send the number of samples (100)
		  NU32_WriteUART3(buffer);
		  while (i < 100)
		  {
		  sprintf(buffer,"%d %d\r\n", Waveform[i], actual_current_waveform[i]); // send the actual and reference waveform. 
																				// send using a while loop because the MATLAB code uses a 
																				// for loop when reading the sent values. 
		  NU32_WriteUART3(buffer);
		  i++;
		  }
		  break;
	  }
		case 'l':				// GO TO ANGLE (DEG)
		{
			NU32_ReadUART3(buffer,BUF_SIZE);
			sscanf(buffer, "%f", &desired_angle); 
			set_mode(HOLD);				// Set to HOLD mode
			break;
		}
		case 'm': 			// LOAD STEP TRAJECTORY
		{
			static int counter2 = 0;
			NU32_ReadUART3(buffer,BUF_SIZE);
			sscanf(buffer, "%d", &num_steps); 		// receive the number of steps from MATLAB.
			while (counter2 < num_steps)
			{
			NU32_ReadUART3(buffer,BUF_SIZE);
			sscanf(buffer, "%f", &reference_position[counter2]); 	
			counter2++;			
			}
			counter2 = 0;		// reset the counter.
			break;
		}			
		case 'n': 			// LOAD CUBIC TRAJECTORY
		{
			static int counter3 = 0;
			NU32_ReadUART3(buffer,BUF_SIZE);
			sscanf(buffer, "%d", &num_steps); 		// receive the number of steps from MATLAB.
			while (counter3 < num_steps)
			{
			NU32_ReadUART3(buffer,BUF_SIZE);
			sscanf(buffer, "%f", &reference_position[counter3]);
			counter3++;			
			}
			counter3 = 0;		// reset the counter.
			break;
		}			
		case 'o':
		{
			trajectory_counter = 0;
			set_mode(TRACK);
			static int case_o_counter = 0;
			if (case_o_counter == num_steps)
			{
			  case_o_counter = 0;
			}
			while (get_mode() == TRACK) {;}	// Infinite while loop while in TRACK mode.  
			sprintf(buffer,"%d\r\n", num_steps);	// send the number of samples. 
			NU32_WriteUART3(buffer);
			while (case_o_counter < num_steps)
			{
			sprintf(buffer,"%f %f\r\n", reference_position[case_o_counter], actual_position_waveform[case_o_counter]); // send the actual and reference position. 
																				// send using a while loop because the MATLAB code uses a 
																				// for loop when reading the sent values. 
			NU32_WriteUART3(buffer);
			case_o_counter++;
			}
			break;
		}
		case 'q':
      {
        // handle q for quit. Later you may want to return to IDLE mode here.
		set_mode(IDLE);			// set the mode to IDLE. 		
        break;
      }
	  case 'r': 					// GET MODE
	  {
		int n = get_mode();
		sprintf(buffer, "%d\n\r", n);
		NU32_WriteUART3(buffer);
		break;
	  }
	  case 'p':					// UNPOWER THE MOTOR 
	  {
		set_mode(IDLE);
		PWM_duty_cycle=0 ; 		//reset the duty cycle to 0. 
		break;
	  }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
