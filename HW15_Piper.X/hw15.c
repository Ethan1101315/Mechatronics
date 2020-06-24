// Ethan Piper
// HW 15
// ME433 - 2019

#include <xc.h>
#include "hw15.h"
#include<stdio.h>

void motor_pwm_init(void) { // initializes timer 2
    //IFS0bits.T2IF = 0;
    T2CONbits.TCKPS = 0; // Timer2 pre-scaler N=1 (1:1)

    PR2 = 2399; // PR = PBCLK / N / desiredF - 1

    TMR2 = 0; // initial TMR2 count is 0

    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults

    OC1RS = 0; // duty cycle

    OC1R = 0; // initialize before turning OC1 on; afterward it is read-only

    T2CONbits.ON = 1; // turn on Timer2

    OC1CONbits.ON = 1; // turn on OC1   
}

void timer3_init(void) {
    IFS0bits.T3IF = 0;
    T3CONbits.TCKPS = 0b111; // Timer3 pre-scaler N=256 (1:64)

    PR3 = 1874; // PR = PBCLK / N / desiredF - 1 // 100 Hz

    
    TMR3 = 0; // initial TMR3 count is 0
    IEC0bits.T3IE = 1; // enable timer3 interrupt. 
    IPC3bits.T3IP = 5; // interrupt priority of 5. 

    T3CONbits.ON = 1; // turn on Timer3  
}