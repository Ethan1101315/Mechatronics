#ifndef I2C2_H__
#define I2C2_H__

#include<xc.h>
#include<sys/attribs.h>  // __ISR macro

// Ethan Piper
// ME433 - 2019
// HW5
// Uses given sample code


// Header file for I2C.c
// helps implement use I2C2 as a master without using interrupts

void initExpander(void);
void setExpander(char, char);
char getExpander();

void i2c_master_setup(void);              // set up I2C 2 as a master, at 100 kHz

void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

#endif