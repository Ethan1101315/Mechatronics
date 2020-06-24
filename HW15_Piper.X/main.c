// Ethan Piper
// HW 15
// ME433 - 2019

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h> 	//for sine wave plotting
#include<stdio.h>
#include"hw15.h"
#include"lcd_hw15.h"

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

char going_down = 0;
char going_up = 1;
int duty_cycle_percentage = 0;// TIMER 3 INTERRUPT

    void __ISR(12, IPL5SOFT) Timer3_ISR(void) { //timer3

    IFS0bits.T3IF = 0;
    
    OC1RS = duty_cycle_percentage * (PR2) / (100); //update the duty cycle. 
    
    if ((going_up) & (duty_cycle_percentage < 100)) {
        duty_cycle_percentage = duty_cycle_percentage + 1;
    }
    else if ((going_up) & (duty_cycle_percentage == 100)) {
        going_up = 0;
        going_down = 1;
        duty_cycle_percentage = duty_cycle_percentage - 1;
    }
    else if ((going_down) & (duty_cycle_percentage > 0)) {
        duty_cycle_percentage = duty_cycle_percentage - 1;
    }
    else if ((going_down) & (duty_cycle_percentage == 0)) {
        going_up = 1;
        going_down = 0;
        duty_cycle_percentage = duty_cycle_percentage + 1;
    }

    // how many times has the interrupt occurred?

    // set the duty cycle and direction pin
    LATBbits.LATB2 = going_up; // flip the LED depending on the direction.
    }
    
    /*
    // TIMER 2 INTERRUPT
    void __ISR(8, IPL5SOFT) Timer2ISR(void) { //timer 2
        IFS0bits.T2IF = 0;
    }
    */

//******************************************************************************//

int main(void) {

  __builtin_disable_interrupts();
  
  motor_pwm_init(); // initialize timer 2 interrupt.
  timer3_init();
  
  // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
  __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

  // 0 data RAM access wait states
  BMXCONbits.BMXWSDRM = 0x0;

  // enable multi vector interrupts
  INTCONbits.MVEC = 0x1;

  // disable JTAG to get pins back
  DDPCONbits.JTAGEN = 0;

  __builtin_enable_interrupts();

  // PIN INITIALIZATIONS SPECIFIC TO THIS CODE.

    ANSELA = 0;
    ANSELB = 0;
    
    RPB3Rbits.RPB3R = 0b0101; // map the RPB3 pin (pin 7) to OC1. Connect this to the top right red LED. 

  // make pin 12 (RA4) an output to power the LED.
    TRISAbits.TRISA4 = 0;
    
  // make pin 6 (RB2) an output to power the top left green LED 
    TRISBbits.TRISB2 = 0;
    LATBbits.LATB2 = 1; //set it to initially be high. 
    
    // set RA4 to initially be high.
    LATAbits.LATA4 = 1;

    // Configure RB2 (SDA2) and RB3 (SCL2) as outputs.
    TRISAbits.TRISA1 = 0; // make A1 an output for the LCD's SDI pin
    TRISBbits.TRISB14 = 0; // make B14 an output for the LCD's SCK pin
    TRISBbits.TRISB15 = 0; // make B15 an output for the LCD's DC pin
    TRISBbits.TRISB8 = 1; // make B8 an input for the LCD's SDO pin
    TRISBbits.TRISB7 = 0; // CS is B7
    TRISBbits.TRISB9 = 0; // CS2 is B9

    SPI1_init(); // turn on spi
    LCD_init(); // initialize the LCD
    LCD_clearScreen(BCKGND_COLOR); // clear the screen

    int i = 0;
    unsigned char random_array[240];
    while (i < 240) { // just use a random array for the LCD plotting function this time. 
        random_array[i] = i;
        i++;
    }
    
    draw_three_axes(); // draw the 3 axes for plotting the RGB values. 

  while(1) {
    _CP0_SET_COUNT(0); // set core timer to 0.

    LATAbits.LATA4 = !LATAbits.LATA4; // blink the LED

    plot_to_LCD(random_array, 1);
    plot_to_LCD(random_array, 2);
    plot_to_LCD(random_array, 3);
    
    
    // core timer will run at 24 MHz - half of the sysclk.
        // 24,000,000 ticks per second -> 24,000 ticks per ms.

    while (_CP0_GET_COUNT() < 2400000) { ; } // delay for 100 ms
    ;
  }
  return 0;
}