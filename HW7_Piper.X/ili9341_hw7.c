    
#include <xc.h>
#include "ili9341_hw7.h"

void LCD_init() {
    int time = 0;
    
    CS = 0; // CS
   
    LCD_command(ILI9341_SWRESET);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 7200000) {} // 300ms

    LCD_command(0xEF);
  	LCD_data(0x03);
	LCD_data(0x80);
	LCD_data(0x02);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xCF);
  	LCD_data(0x00);
	LCD_data(0xC1);
	LCD_data(0x30);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xED);
  	LCD_data(0x64);
	LCD_data(0x03);
	LCD_data(0x12);
    LCD_data(0x81);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xE8);
  	LCD_data(0x85);
	LCD_data(0x00);
	LCD_data(0x78);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xCB);
  	LCD_data(0x39);
	LCD_data(0x2C);
	LCD_data(0x00);
    LCD_data(0x34);
    LCD_data(0x02);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xF7);
  	LCD_data(0x20);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xEA);
  	LCD_data(0x00);
	LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_PWCTR1);
  	LCD_data(0x23);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_PWCTR2);
  	LCD_data(0x10);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_VMCTR1 );
  	LCD_data(0x3e);
    LCD_data(0x28);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_VMCTR2);
  	LCD_data(0x86);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_MADCTL);
  	LCD_data(0x48);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
/*    
    LCD_command(ILI9341_VSCRSADD);
  	LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
 */   
    LCD_command(ILI9341_PIXFMT);
  	LCD_data(0x55);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_FRMCTR1);
  	LCD_data(0x00);
    LCD_data(0x18);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command( ILI9341_DFUNCTR);
  	LCD_data(0x08);
    LCD_data(0x82);
    LCD_data(0x27);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xF2);
  	LCD_data(0); // 1
    LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_GAMMASET);
  	LCD_data(0x01);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_GMCTRP1);
  	LCD_data(0x0F);
    LCD_data(0x31);
    LCD_data(0x2B);
    LCD_data(0x0C);
    LCD_data(0x0E);
    LCD_data(0x08);
    LCD_data(0x4E);
    LCD_data(0xF1);
    LCD_data(0x37);
    LCD_data(0x07);
    LCD_data(0x10);
    LCD_data(0x03);
    LCD_data(0x0E);
    LCD_data(0x09);
    LCD_data(0x00);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_GMCTRN1);
  	LCD_data(0x00);
    LCD_data(0x0E);
    LCD_data(0x14);
    LCD_data(0x03);
    LCD_data(0x11);
    LCD_data(0x07);
    LCD_data(0x31);
    LCD_data(0xC1);
    LCD_data(0x48);
    LCD_data(0x08);
    LCD_data(0x0F);
    LCD_data(0x0C);
    LCD_data(0x31);
    LCD_data(0x36);
    LCD_data(0x0F);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(0xB1);
  	LCD_data(0x00);
    LCD_data(0x10);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_SLPOUT);
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    LCD_command(ILI9341_DISPON);
    
    CS = 1; // CS
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    
    CS = 0; // CS
    
    LCD_command(ILI9341_MADCTL);
    LCD_data(MADCTL_MX | MADCTL_BGR); // rotation
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 3600000) {} // 150ms
    
    CS = 1; // CS
}

void SPI1_init() {
  SDI1Rbits.SDI1R = 0b0100; // B8 is SDI1
  RPA1Rbits.RPA1R = 0b0011; // A1 is SDO1
  TRISBbits.TRISB7 = 0; // CS is B7
  CS = 1; // CS starts high

  // DC pin
  TRISBbits.TRISB15 = 0;
  DC = 1;
  
  SPI1CON = 0; // turn off the spi module and reset it
  SPI1BUF; // clear the rx buffer by reading from it
  SPI1BRG = 0; // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0; // clear the overflow bit
  SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1; // master operation
  SPI1CONbits.ON = 1; // turn on spi1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    DC = 0; // DC
    spi_io(com);
    DC = 1; // DC
}

void LCD_data(unsigned char dat) {
    spi_io(dat);
}

void LCD_data16(unsigned short dat) {
    spi_io(dat>>8);
    spi_io(dat);
}

void LCD_setAddr(unsigned short x, unsigned short y, unsigned short w, unsigned short h) {
    LCD_command(ILI9341_CASET); // Column
    LCD_data16(x);
	LCD_data16(x+w-1);

	LCD_command(ILI9341_PASET); // Page
	LCD_data16(y);
	LCD_data16(y+h-1);

	LCD_command(ILI9341_RAMWR); // Into RAM
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // check boundary
    
    CS = 0; // CS
    
    LCD_setAddr(x,y,1,1);
    LCD_data16(color);
    
    CS = 1; // CS
}

void LCD_clearScreen(unsigned short color) {
    int i;
    
    CS = 0; // CS
    
    LCD_setAddr(0,0,ILI9341_TFTWIDTH,ILI9341_TFTHEIGHT);
	for (i = 0;i < ILI9341_TFTWIDTH*ILI9341_TFTHEIGHT; i++){
		LCD_data16(color);
	}
    
    CS = 1; // CS
}

void draw_letter(char letter, int x, int y, int c1, int c2) {
    int i; // i increments in the x direction
    int j; // j increments in the y direction
    for (i = 0; i<5; i++) { // each row is 5 pixels wide
        char col = ASCII[letter-0x20][i]; // shift by hex 20 because the first 20 ASCII characters cannot be displayed(?)
        for (j = 7; j>=0; j--) { // each column is 8 pixels tall. Start at the bottom and move up. 
            char bit = (col >> j)&1;
            if (bit==1) {
                LCD_drawPixel(x+i, y+j, c1);
            }
            else {
                LCD_drawPixel(x+i, y+j, c2); // c2 is the background color
            }
        }
    }
}

void draw_string(char* m, int x, int y, int c1, int c2) {
    int t = 0;
    while(m[t]) { // the last character will be a 0, indicating that the string has ended. 
        draw_letter(m[t], x + 5*t, y, c1, c2); // move over 5 bits each time because each character is that wide. 
        t++;
    }
}
void progress_bar(int x, int y, int length, int increment) { 
    float i;
    int j;
    float k;
    char l;
    
   
    for (k = 0; k<((increment*length)/100); k++) {
        for (l = 0; l<7; l++) { // make the bar 8 bits tall 
            LCD_drawPixel(x + k, y+l, ILI9341_WHITE); 
        }
    }
    
    // draw the background to the bar. 
    for (i = k; i<length; i++) { 
        for (j = 0; j<7; j++) { // make the bar 8 bits tall 
            LCD_drawPixel(x + i, y+j, ILI9341_BLACK); 
        }
    }
}

void IMU_bar(int accel_x, int accel_y) {
    
    // BACKGROUND BARS 
    
    /*
    // HORIZONTAL BAR
    int i = 0;
    int j = 0;
    for (i = 20; i<220; i++) { 
        for (j = 0; j<7; j++) { // start at center of LCD and make 8 bits tall
            LCD_drawPixel(i, (320/2)-4+j, ILI9341_BLACK); 
        }
    }
    
    // VERTICAL BAR
    int a;
    int b;
    for (b = (120-4); b < (120+4); b++) { // start at center of LCD and make 8 bits wide
        for (a = 60; a < 260; a++) { // make the bar 8 bits tall 
            LCD_drawPixel(b, a, ILI9341_BLACK); 
        }
    }
    */
     
    // DRAW MOVING ACCELERATION BARS
    
    float accel_x_ratio = (float) accel_x / (float) 16000; // normalize the accelerations
    float accel_y_ratio = (float) accel_y / (float) 16000;
    
    float x_bar = accel_x_ratio * 100.0; // how far from the center to move
    float y_bar = accel_y_ratio * 100.0;
    
    if (x_bar > 100) {
        x_bar = 100; // because the acceleration values sometimes go above 16000, 
                     //ensure the x_bar is not plotted past the background bars. 
    }
    else if (x_bar < -100) {
        x_bar = -100;
    }
    
    if (y_bar > 100) {
        y_bar = 100; // because the acceleration values sometimes go above 16000, 
                     //ensure the x_bar is not plotted past the background bars. 
    }
    else if (y_bar < -100) {
        y_bar = -100;
    }
    
    int i_x = 0;
    int j_x = 0;
    
    // X ACCELERATION BAR
    
    if (x_bar < 0) {
        for (i_x = 0; i_x>x_bar; i_x--) { 
            for (j_x = 0; j_x<7; j_x++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(120 + i_x, (320/2)-4+j_x, ILI9341_WHITE); 
            }
        }
    }
    else {
        for (i_x = 0; i_x<x_bar; i_x++) { 
            for (j_x = 0; j_x<7; j_x++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(120 + i_x, (320/2)-4+j_x, ILI9341_WHITE); 
            }
        }
    }
    
    // Y ACCELERATION BAR
    
    int i_y = 0;
    int j_y = 0;
    
    if (y_bar < 0) {
        for (i_y = 0; i_y>y_bar; i_y--) { 
            for (j_y = 0; j_y<8; j_y++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(116 + j_y, (320/2)+i_y, ILI9341_WHITE); 
            }
        }
    }
    else {
        for (i_y = 0; i_y<y_bar; i_y++) { 
            for (j_y = 0; j_y<8; j_y++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(116 + j_y, (320/2)+i_y, ILI9341_WHITE); 
            }
        }
    }
    
    // DRAW BACKGROUND BARS. MAKE SURE THEY DON'T COVER THE MOVING BARS. 
    
    // HORIZONTAL BAR
    int i = 0;
    int j = 0;
    if (x_bar < 0) {
        for (i = 20; i<(120 + x_bar); i++) { 
            for (j = 0; j<7; j++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(i, (320/2)-4+j, ILI9341_BLACK); 
            }
        }
        for (i = 120; i < 220; i++) {
            for (j = 0; j<7; j++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(i, (320/2)-4+j, ILI9341_BLACK); 
            }
        }
    }
    else {
        for (i = 20; i<(120); i++) { 
            for (j = 0; j<7; j++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(i, (320/2)-4+j, ILI9341_BLACK); 
            }
        }
        for (i = 220; i > 120 + x_bar; i--) {
            for (j = 0; j<7; j++) { // start at center of LCD and make 8 bits tall
                LCD_drawPixel(i, (320/2)-4+j, ILI9341_BLACK); 
            }
        }
    }
    
    // VERTICAL BAR
    int a;
    int b;
    if (y_bar < 0) {
        for (b = (120-4); b < (120+4); b++) { // start at center of LCD and make 8 bits wide
            for (a = 60; a < 160 + y_bar; a++) { // make the bar 8 bits tall 
                LCD_drawPixel(b, a, ILI9341_BLACK); 
            }
        }
        for (b = (120-4); b < (120+4); b++) { // start at center of LCD and make 8 bits wide
            for (a = 260; a < 160; a--) { // make the bar 8 bits tall 
                LCD_drawPixel(b, a, ILI9341_BLACK); 
            }
        }
    }
    else {
        for (b = (120-4); b < (120+4); b++) { // start at center of LCD and make 8 bits wide
            for (a = 60; a < 160; a++) { // make the bar 8 bits tall 
                LCD_drawPixel(b, a, ILI9341_BLACK); 
            }
        }
        for (b = (120-4); b < (120+4); b++) { // start at center of LCD and make 8 bits wide
            for (a = 260; a > 160 + y_bar; a--) { // make the bar 8 bits tall 
                LCD_drawPixel(b, a, ILI9341_BLACK); 
            }
        }
    }
}