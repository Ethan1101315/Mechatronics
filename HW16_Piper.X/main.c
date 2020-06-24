#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>

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
#pragma config PMDL1WAY = ON // allow multiple reconfigurations
#pragma config IOL1WAY = ON // allow multiple reconfigurations
#pragma config FUSBIDIO = OFF // USB pins controlled by USB module
#pragma config FVBUSONIO = OFF // USB BUSON controlled by USB module


#include "ili9341_final.h"
#include "i2c_master_noint.h"
#include "ov7670.h"
#include "motor.h"

#define DIR1 LATAbits.LATA10
#define DIR2 LATAbits.LATA7
#define USER PORTBbits.RB4



void startup() {
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // pin definitions
    ANSELA = 0;
    ANSELB = 0;
    TRISAbits.TRISA7 = 0; // DIR2
    DIR2 = 1;
    TRISAbits.TRISA10 = 0; // DIR1
    DIR1 = 1;
    TRISBbits.TRISB4 = 1; // USER
    
    // OC1 is B15, goes with DIR1
    RPB15Rbits.RPB15R = 0b0101; // map the RPB3 pin (pin 7) to OC1. Connect this to the top right red LED. 
    
    // OC4 is A4, goes with DIR2
    
    // LCD uses SPI1: A0 is SDO, A1 is SDI, B5 is CST, B14 is SCK1, A9 is DC, B7 is CS
    SPI1_init();
    LCD_init();
    LCD_clearScreen(ILI9341_DARKGREEN);
    
    // Camera uses C0-7, C8 as OC2, A8 as INT3/PCLK, B13 as INT2/HREF, C9 as INT1/VSYNC, and I2C1
    i2c_master_setup();
    ov7670_setup();
    
    
    // B3 is available as SCL2, B2 is available as SDA2
    
    
    
    
}

// INTERRUPTS //

char going_down = 0;
char going_up = 1;
int duty_cycle_percentage = 0;// TIMER 3 INTERRUPT

    void __ISR(12, IPL5SOFT) Timer3_ISR(void) { //timer3

    IFS0bits.T3IF = 0;
    
    OC1RS = PR2;
    
    //OC1RS = duty_cycle_percentage * (PR2) / (100); //update the duty cycle. 
    
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
    }

int main() {

    __builtin_disable_interrupts();

    motor_pwm_init(); // initialize timer 2 interrupt.
    timer3_init();
    
    startup();

    __builtin_enable_interrupts();
    
    int I = 0;
    char message[100];
    
    while(1) {

        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<48000000/2/2){
            while(USER == 0){}
        }
        DIR1 = 0;
        DIR2 = 1;
        
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<48000000/2/2){
            while(USER == 0){}
        }
        DIR1 = 1;
        DIR2 = 0;
        
        //I++;
        //sprintf(message,"I = %d   ", I);
        //drawString(140,92,message);
        
        unsigned char d[2000];
        
        int c = ov7670_count(d);
        sprintf(message, "c = %d   ",c);
        drawString(140,92,message); // there are 290 rows
        /*
        int i = 0;
        int t = 0;
        for (i=0;i<30;i++){
            sprintf(message, "%d    %d    %d    %d  ",d[t],d[t+1],d[t+2],d[t+3]);
            t=t+4;
            drawString(1,1+i*10,message);
        }
         * */
        
        int x = 0, x2 = 1;
        int y = 0;
        int dim = 0;
        for(x = 0; x < 310; x++, x2=x2+2){
            dim = d[x2]>>3;
            for(y=0;y<32;y++){
                if (y == dim){
                    LCD_drawPixel(y+30,x,ILI9341_BLACK);
                }
                else {
                    LCD_drawPixel(y+30,x,ILI9341_WHITE);
                }
            }
        }
        
        

    }
}
