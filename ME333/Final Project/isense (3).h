// Ethan Piper
// ME333
// isense.h

#ifndef isense__H__
#define isense__H__

void ADC_init(void); // function prototypes

int ADC_read(int);	//reads the ADC and returns a value from 0-1023. 

#define VOLTS_PER_COUNT (3.3/1024)
#define CORE_TICK_TIME 25    // nanoseconds between core ticks
#define SAMPLE_TIME 10       // 10 core timer ticks = 250 ns
#define DELAY_TICKS 20000000 // delay 1/2 sec, 20 M core ticks, between messages

#endif