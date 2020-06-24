// Ethan Piper
// ME333
// isense.c

// This code is heavily based on Code Sample 10.1

#include "isense.h"
#include "NU32.h"

void ADC_init(void)	{
	AD1PCFGbits.PCFG0 = 0;					// AN0 is an ADC pin.
	AD1CON3bits.ADCS = 2;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
                                          //                           2*3*12.5ns = 75ns
	AD1CON1bits.ADON = 1;    				// turn on the analog to digital converter. 
}

int ADC_read(int pin) {
	int ADC_to_return = 0; 
	int i = 0;
	while (i < 10) {							//read the pin 10 times and return the average. 
										   
	
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) { 
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
		i++;
		ADC_to_return = ADC_to_return + ADC1BUF0;
	}
    return ADC_to_return/10;                      // read the buffer with the result, the averaged adc value. 
}
