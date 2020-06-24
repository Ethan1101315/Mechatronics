// Ethan Piper
// HW 7
// ME433 - 2019
// Uses given sample code

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h> 	//for sine wave plotting
#include"ili9341_hw7.h" // separate I2C header file
#include"imu_i2c.h"
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
  
  // make pin 12 (RA4) an output to power the LED. 
    TRISAbits.TRISA4 = 0; 
    
    // set RA4 to initially be high. 
    LATAbits.LATA4 = 1;
    
    // make pin 11 (RB4) an input so that information is taken in when the pushbutton is used. 
    // TRISBbits.TRISB4 = 1;
    
    // Configure RB2 (SDA2) and RB3 (SCL2) as outputs.
    TRISAbits.TRISA1 = 0; // make A1 an output for the LCD's SDI pin
    TRISBbits.TRISB14 = 0; // make B14 an output for the LCD's SCK pin
    TRISBbits.TRISB15 = 0; // make B15 an output for the LCD's DC pin
    TRISBbits.TRISB7 = 0; // make B15 an output for the LCD's CS pin
    TRISBbits.TRISB8 = 0; // make B15 an output for the LCD's SDO pin
    
    initIMU(); // enable I2C communication with the IMU chip. 

    SPI1_init(); // turn on spi
    LCD_init(); // initialize the LCD
    LCD_clearScreen(BCKGND_COLOR); // clear the screen
  
    char m[10];
  while(1) {
    _CP0_SET_COUNT(0); // set core timer to 0.
    
    LATAbits.LATA4 = !LATAbits.LATA4; // blink the LED
    
   
    
    //char who_am_i = getIMU(0x0F); //Check the WHO_AM_I REGISTER
    //sprintf(m, "%d", who_am_i); // debugging
    //draw_string(m, 28, 32, ILI9341_MAGENTA, BCKGND_COLOR);
    
   // Save IMU data to a char array
    
    unsigned char data[14]; // make a char array to put data into. 
    
    
    I2C_read_multiple(IMU_ADDRESS, 0x20, data, 14); // start at OUT_TEMP_L register. Length of 14.
    
    // Convert the char array into 7 different shorts
    signed short temperature =  (data[1]<<8)|(data[0]); // shift the high 8 bits, then or with the low 8 bits. Create a 16 bit value. 
    signed short gyro_x = (data[3]<<8)|(data[2]);
    signed short gyro_y = (data[5]<<8)|(data[4]);
    signed short gyro_z = (data[7]<<8)|(data[6]);
    signed short accel_x = (data[9]<<8)|(data[8]);
    signed short accel_y = (data[11]<<8)|(data[10]);
    signed short accel_z = (data[13]<<8)|(data[12]);
    
    /*
    sprintf(m, "temperature: %d", temperature);
    draw_string(m, 28, 40, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "gyroX: %d", gyro_x);
    draw_string(m, 28, 48, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "gyroY: %d", gyro_y);
    draw_string(m, 28, 56, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "gyroZ: %d", gyro_z);
    draw_string(m, 28, 64, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "accelX: %d", accel_x);
    draw_string(m, 28, 72, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "accelY: %d", accel_y);
    draw_string(m, 28, 80, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "accelZ: %d", accel_z);
    draw_string(m, 28, 88, ILI9341_MAGENTA, BCKGND_COLOR);
    */
   
    IMU_bar(accel_x, accel_y);
    
    // print out relevant values. 
    
    char who_am_i = getIMU(0x0F); //Check the WHO_AM_I REGISTER
    sprintf(m, "WHO_AM_I = %d", who_am_i); // debugging
    draw_string(m, 28, 32, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "accelX: %d", accel_x);
    draw_string(m, 28, 40, ILI9341_MAGENTA, BCKGND_COLOR);
    sprintf(m, "accelY: %d", accel_y);
    draw_string(m, 28, 48, ILI9341_MAGENTA, BCKGND_COLOR);
    
    // core timer will run at 24 MHz - half of the sysclk. 
        // 24,000,000 ticks per second -> 24,000 ticks per ms. 
    
    while (_CP0_GET_COUNT() < 2400000/2) { ; } // delay for 50 ms - 20 Hz
    ;
  }
  return 0;
}