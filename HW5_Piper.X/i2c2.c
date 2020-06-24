// Ethan Piper
// ME433 - 2019
// HW5
// Uses given sample code

#include"i2c2.h"

// I2C Master utilities, 400 kHz, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

void i2c_master_setup(void) {
  ANSELBbits.ANSB2 = 0;
  ANSELBbits.ANSB3 = 0;
  // pgd = 104 ns, Fsck = 400 kHz, Pbclk = 48 MHz.
  I2C2BRG = 53;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void initExpander() {
    
    i2c_master_setup();
    
// set pins as inputs or outputs
    i2c_master_start();
    i2c_master_send((0b0100000<<1)|0); // chip address (A0, A1, and A2 are set to GND)
    i2c_master_send(0x0); //address of the IODIR register 
    i2c_master_send(0b11110000); // the value to put in the register
    i2c_master_stop(); // make the stop bit
    
   
// INTCON register
    i2c_master_start();
    i2c_master_send((0b0100000<<1)|0); // chip address (A0, A1, and A2 are set to GND)
    i2c_master_send(0x05); //address of the IODIR register 
    i2c_master_send(0b00100000); // the value to put in the register
    i2c_master_stop(); // make the stop bit
    
// OLAT register
    i2c_master_start();
    i2c_master_send((0b0100000<<1)|0); // chip address (A0, A1, and A2 are set to GND)
    i2c_master_send(0x0A); //address of the OLAT register 
    i2c_master_send(0b00001111); // the value to put in the register
    i2c_master_stop(); // make the stop bit
  
//     
}

void setExpander(char pin, char level) {
    // the only pin we will need to flip is GP0. Set it low when pushbutton is pushed (when GP7 is high)
    i2c_master_start();
    i2c_master_send((0b0100000<<1)|0); // chip address (A0, A1, and A2 are set to GND)
    i2c_master_send(0x0A); //address of the OLAT register 
    
    unsigned char command;
    if (level == 1) { // if we want to make GP0 high
        command = 0b1>>pin; // bit shift depending on which pin you want to modify. 
        i2c_master_send(command); // the value to put in the register
    }
    else { // if we want to make GP0 low
        command = 0b0>>pin; // bit shift depending on which pin you want to modify. 
        i2c_master_send(command); // the value to put in the register
    }
    
    //i2c_master_send(0b1);
    i2c_master_stop(); // make the stop bit
}

char getExpander(address) {
    i2c_master_start();
    i2c_master_send((0b0100000<<1)|0); // chip address (A0, A1, and A2 are set to GND)
    i2c_master_send(address); // read from the GPIO register
    i2c_master_restart(); // make the restart bit
    i2c_master_send((0b0100000<<1)|1); // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    char r = i2c_master_recv(); // save the value returned
    i2c_master_ack(1); // make the ack so the slave knows we got it
    i2c_master_stop(); // make the stop bit
    return r;
}