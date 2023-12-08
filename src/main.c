#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************************************************************************
* Title:    I2C master library using hardware TWI interface
* Author:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
* File:     $Id: twimaster.c,v 1.3 2005/07/02 11:14:21 Peter Exp $
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   any AVR device with hardware TWI 
* Usage:    API compatible with I2C Software Library i2cmaster.h
**************************************************************************/
#include <inttypes.h>
#include <compat/twi.h>


/* define CPU frequency in Mhz here if not defined in Makefile */
//#ifndef F_CPU
//#define F_CPU 16000000UL
//#endif
 
/* I2C clock in Hz */
#define SCL_CLOCK 50000L


/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void i2c_init(void)
{
  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
  
  TWSR = 0;                         /* no prescaler */
  TWBR = ((F_CPU/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */

}/* i2c_init */


/*************************************************************************	
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
unsigned char i2c_start(unsigned char address)
{
    uint8_t   twst;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;

	// send device address
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;

	return 0;

}/* i2c_start */


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready
 
 Input:   address and transfer direction of I2C device
*************************************************************************/
void i2c_start_wait(unsigned char address)
{
    uint8_t   twst;


    while ( 1 )
    {
	    // send START condition
	    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    
    	// wait until transmission completed
    	while(!(TWCR & (1<<TWINT)));
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
    
    	// send device address
    	TWDR = address;
    	TWCR = (1<<TWINT) | (1<<TWEN);
    
    	// wail until transmission completed
    	while(!(TWCR & (1<<TWINT)));
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) ) 
    	{    	    
    	    /* device busy, send stop condition to terminate write operation */
	        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	        
	        // wait until stop condition is executed and bus released
	        while(TWCR & (1<<TWSTO));
	        
    	    continue;
    	}
    	//if( twst != TW_MT_SLA_ACK) return 1;
    	break;
     }

}/* i2c_start_wait */


/*************************************************************************
 Issues a repeated start condition and sends address and transfer direction 

 Input:   address and transfer direction of I2C device
 
 Return:  0 device accessible
          1 failed to access device
*************************************************************************/
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );

}/* i2c_rep_start */


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
void i2c_stop(void)
{
    /* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	// wait until stop condition is executed and bus released
	while(TWCR & (1<<TWSTO));

}/* i2c_stop */


/*************************************************************************
  Send one byte to I2C device
  
  Input:    byte to be transfered
  Return:   0 write successful 
            1 write failed
*************************************************************************/
unsigned char i2c_write( unsigned char data )
{	
    uint8_t   twst;
    
	// send data to the previously addressed device
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits
	twst = TW_STATUS & 0xF8;
	if( twst != TW_MT_DATA_ACK) return 1;
	return 0;

}

// buttons PB0, PB1
// encoder PD3, PD4

#define OLED_I2C_ADDR   0x3C
#define MCP4017_ADDR    0x2F // IDK the address yet

#define ENCODER_PORT    PORTD
#define ENCODER_PIN     PIND
#define ENCODER_DDR     DDRD
#define ENCODER_A       3
#define ENCODER_B       4

volatile int16_t frequency = 100;  // IN MHZ
volatile int16_t encoderValue = 0;
volatile int8_t encoderLast = 0;

void initEncoder() {
    ENCODER_DDR &= ~((1 << ENCODER_A) | (1 << ENCODER_B));
    ENCODER_PORT |= (1 << ENCODER_A) | (1 << ENCODER_B);
    //GICR |= (1 << INT0) | (1 << INT1);
    //MCUCR |= (1 << ISC01) | (1 << ISC11);
    //sei();
}

void initOLED() {
    i2c_start(OLED_I2C_ADDR);
    i2c_write(0xAF);  // Display on
    i2c_stop();
}
void initDP(){
   i2c_start(MCP4017_ADDR);
   i2c_write(64);
   i2c_stop();
}

void updateOLED() {
    char buffer[16];
    sprintf(buffer, "Freq: %d", frequency);
    i2c_write(0x40);

    for (uint8_t i = 0; i < strlen(buffer); i++) {
        i2c_write(buffer[i]);
    }
    i2c_stop();
}

void updateDP(int value) {
    //uint8_t wiperValue = (uint8_t) (0x7F - (frequency / 10);
    i2c_start(MCP4017_ADDR);
    i2c_write(value);
    i2c_stop();
}

/*
ISR(INT0_vect) {
    if (!(ENCODER_PIN & (1 << ENCODER_A))) {
        if (ENCODER_PIN & (1 << ENCODER_B)) {
            encoderValue++;
        } else {
            encoderValue--;
        }
    }
}

ISR(INT1_vect) {
    if (!(ENCODER_PIN & (1 << ENCODER_B))) {
        if (ENCODER_PIN & (1 << ENCODER_A)) {
            encoderValue++;
        } else {
            encoderValue--;
        }
    }
}
*/

void setup() {
    i2c_init();
    DDRB = 0;
}

int main(void) {
    setup();
    //initEncoder();
    //initOLED();

    int pot_value = 30;

    while(1) {
        /*
        if (encoderValue != encoderLast) {
            frequency += encoderValue;
            encoderLast = encoderValue;

            if (pot_value < 0) { 
                pot_value = 0;
            } 
            else if (pot_value > 120) {
                pot_value = 120;
            }
            updateOLED();
        }
        */

        
        // Button 1
        if (PORTB & (1 << 0)) {
            pot_value++;
            updateDP(pot_value);
        }
        // Button 2
        if (PORTB & (1 << 1)) {
            pot_value--;
            updateDP(pot_value);
        }


        updateDP(pot_value);
        _delay_ms(100);  // Delay to reduce display update frequency
    }

    return 0;
}

