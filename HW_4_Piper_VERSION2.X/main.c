// Ethan Piper
// ME433 - 2019
// Uses given sample code

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h> 	//for sine wave plotting

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

#define CS LATAbits.LATA0       // chip select pin

//******************************************************************************//

// FUNCTION DEFINITIONS

void initSPI1() {              // SPI initialization function
  //#define CS LATAbits.LATA0 ;      // chip select pin

  RPA0Rbits.RPA0R = 0b0011; // map the RPA0 pin to SS1. 
     
  RPA1Rbits.RPA1R = 0b0011; // assign SD01 to pin RPA1. 
  SDI1Rbits.SDI1R = 0b0100; // assign SD1R to pin RPB8
          
  // set up the chip select pin as an output
  // the chip select pin is used to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISAbits.TRISA0 = 0;
  CS = 1;

  // Master - SPI1, pins are: SDI4(F4), SDO4(F5), SCK4(F13).  
  // we manually control SS4 as a digital output (F12)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup spi1 , all bits must be changed for SPI1 from SPI4
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 1;            // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1CONbits.MODE32 = 0;
  SPI1CONbits.MODE16 = 0;
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.MSSEN=0;
  SPI1CONbits.ON = 1;       // turn on spi 1
  
}
    
// send a byte via spi and return the response
unsigned char spi1_io(unsigned char o) {
  //SPI1BUF = o;
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  //SPI1BUF; // throwaway info
  return SPI1BUF;
}

void setVoltage(unsigned char channel, unsigned int v) {

	unsigned short t = 0;
    unsigned short t2 = 0;
	t = channel << 15; //channel name is at the very end of the data transfer
	t = t | 0b0111000000000000; // sets BUF =1 (buffered), GA = 1 (gain of 1), and SHDN = 1 (active mode)
	t = t | ((v&0b111111111111)); //rejecting excessive bits (above 12 because I have the 12 bit DAC)
     
    
    //char BGS = 0b111;
    //char c1 = BGS<<4;
    //c1 = c1 | channel << 7;
    //c1 = c1 | v >> 6;
    //char c2 = v&0b000011111111;
    
    
    //t = t | 0b0111000000000000;
	//t = t | ((v&0b1111111111) <<2); //rejecting excessive bits (above 10)
	
	CS = 0;
	spi1_io(t>>8); // spi_io only sends 8 bits at a time, but we need to send 16 bits. 
    spi1_io(t);
	CS = 1;
}

//******************************************************************************//

int main(void) {
    
   //ANSELA = 0;
   //ANSELB = 0;
  
  // comment out NU32_Startup because the Pragmas take care of this. 
    
  __builtin_disable_interrupts();

  // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
  __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

  // 0 data RAM access wait states
  BMXCONbits.BMXWSDRM = 0x0;

  // enable multi vector interrupts
  INTCONbits.MVEC = 0x1;

  // disable JTAG to get pins back
  DDPCONbits.JTAGEN = 0;

  // make pin 2 (RA0) an output for the SS function. 
  TRISAbits.TRISA0 = 0; 
    
  // make pin 3 (RA1) an output for the SD01 function. 
  TRISAbits.TRISA1 = 0;
  
  // make pin 25 (RB14) an output for the SCK1 function. 
  TRISBbits.TRISB14 = 0;
    
    
  __builtin_enable_interrupts();

  initSPI1(); 
  
  int i = 0;
  int j = 0;
 
  unsigned int v_a;
  unsigned int v_b;
  int going_up = 1;
  int going_down = 0; // for the triangle wave
  
  
  while(1) {
    _CP0_SET_COUNT(0); // set core timer to 0. 
        
        // core timer will run at 24 MHz - half of the sysclk. 
        // 24,000,000 ticks per second -> 24,000 ticks per ms. 
    while (_CP0_GET_COUNT() < 24000) { ; } // delay for 1 ms.
    
    v_a = 2048 + 2047*sin(i*2*3.1415/1000*10);  //should make a 10Hz sin wave)
    
    // max value of 4095
    // 5 full triangles a second
    // half a triangle per 100 ms. 
    // max ADC of 4000
    // if we increment by 40 every time, we will have a frequency of 5 Hz. 
    
    //TRIANGLE WAVE
    
    if ((j < 100) & (going_up ==1)) {
        v_b = 40 * j;
    }
    else if (j == 100 & (going_up ==1)) {
        j = 0; // reset the increment variable
        going_up = 0;
        going_down = 1;
    }
    else if ((j < 100 & j > 0) & (going_down ==1)) {
        v_b = 4000 + 40 * -j;
    }
    else if (v_b == 0 & going_down == 1) {
        going_down = 0;
        going_up = 1;
        j = 0;
    }
    else if (j == 100 & (going_down ==1)) {
        j = 0; // reset the increment variable
        going_up = 1;
        going_down = 0;
    }
	i++;
    j++;
    
    
	setVoltage(0b0, v_a);	
	setVoltage(0b1, v_b);

    ;
  }
  return 0;
}