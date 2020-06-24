// Ethan Piper
// HW 6
// ME433 - 2019
// Uses given sample code

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h> 	//for sine wave plotting
#include"ili9341_hw9.h" // separate I2C header file
#include<stdio.h>

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


//******************************************************************************//

int main(void) {
    
  __builtin_disable_interrupts();

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
  
  // make pin 12 (RA4) an output to power the LED. 
    TRISAbits.TRISA4 = 0; 
    
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
  
    float i = 0;
    char m[10];
    unsigned short x, y; 
    unsigned int z; 
    float z_old = 0;
    int x_old = 0;
    int y_old = 0;
    hw_9_boxes();
    int x_to_print;
    int y_to_print;
    float z_to_print;
  while(1) {
    _CP0_SET_COUNT(0); // set core timer to 0.
    
    LATAbits.LATA4 = !LATAbits.LATA4; // blink the LED
    
    sprintf(m, "i = %2.0f", i);
    draw_string(m, 162, 152, ILI9341_WHITE, BCKGND_COLOR);
    
    XPT2046_read(&x, &y, &z);
    
    // save the values right away because they seem to change.
    x_to_print = x;
    y_to_print = y;
    z_to_print = (float)z;
    
    // print the x, y, and z values.
    sprintf(m, "x_raw = %d", x_to_print);
    draw_string(m, 28, 100, ILI9341_GREEN, BCKGND_COLOR);
    sprintf(m, "y_raw = %d", y_to_print);
    draw_string(m, 28, 108, ILI9341_GREEN, BCKGND_COLOR);
    sprintf(m, "z = %.2f", z_to_print);
    draw_string(m, 28, 116, ILI9341_GREEN, BCKGND_COLOR);
    float x_remapped = ((((float) x_to_print) * 0.0002442) * 320.0);
    float y_remapped = 320 - ((((float) y_to_print) * 0.0002442) * 320.0);
    
    
    
    sprintf(m, "x = %.2f", x_remapped);
    draw_string(m, 28, 124, ILI9341_GREEN, BCKGND_COLOR);
    sprintf(m, "y = %.2f", y_remapped);
    draw_string(m, 28, 132, ILI9341_GREEN, BCKGND_COLOR);
    
    //sprintf(m, "test %d", (((z_to_print < 400)& (z_old > 400))));
    //draw_string(m, 28, 140, ILI9341_GREEN, BCKGND_COLOR);
    
    //sprintf(m, "test2 %d", (x_old > 149 & x_old < 201));
    //draw_string(m, 28, 148, ILI9341_GREEN, BCKGND_COLOR);
    
    //sprintf(m, "test3 %d", (y_old > 63 & y_old < 113));
    //draw_string(m, 28, 156, ILI9341_GREEN, BCKGND_COLOR);
   
    
    // increment if the top box is pressed and released.
    if (((z_to_print < 400)& (z_old > 400)) & (x_old > 149 + 40 & x_old < 201 + 60) & (y_old > 63 & y_old < 113)) {
        i++;
    }
    
    // decrement if the bottom box is pressed and released.
    if (((z_to_print < 400) & (z_old > 400)) & (x_old > 149 + 40 & x_old < 201 + 60) & (y_old > 191  & y_old < 241)) {
        i--;
    }
    
    z_old = z_to_print; // update z so that we can determine if a button is both pressed and released. 
    x_old = x_remapped;
    y_old = y_remapped;
    
    
    float fps = (24000000.0 / _CP0_GET_COUNT()); // 24,000,000 ticks per second. Find the current number of ticks and divide by 24 M
    sprintf(m, "FPS: %.2f", fps);
    draw_string(m, 28, 168, ILI9341_GREEN, BCKGND_COLOR);
    
    // core timer will run at 24 MHz - half of the sysclk. 
        // 24,000,000 ticks per second -> 24,000 ticks per ms. 
    
    while (_CP0_GET_COUNT() < 2400000) { ; } // delay for 100 ms
    ;
  }
  return 0;
}