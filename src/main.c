#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2cmaster.h"  // Include library for I2C OLED display

#define OLED_I2C_ADDR   0x3C
#define MCP4017_ADDRESS 0x00 // IDK the address yet

#define ENCODER_PORT    PORTB
#define ENCODER_PIN     PINB
#define ENCODER_DDR     DDRB
#define ENCODER_A       PB3
#define ENCODER_B       PB4

volatile int16_t frequency = 100;  // IN MHZ
volatile int16_t encoderValue = 0;
volatile int8_t encoderLast = 0;

void initEncoder() {
    ENCODER_DDR &= ~((1 << ENCODER_A) | (1 << ENCODER_B));
    ENCODER_PORT |= (1 << ENCODER_A) | (1 << ENCODER_B);
    GICR |= (1 << INT0) | (1 << INT1);
    MCUCR |= (1 << ISC01) | (1 << ISC11);
    sei();
}

void initOLED() {
    i2c_init();
    i2c_start_wait(OLED_I2C_ADDR | I2C_WRITE);
    i2c_write(0x00);
    i2c_write(0xAE);  // Display off
    i2c_write(0xAF);  // Display on
    i2c_stop();
}
void initDP(){
   uint8_t wiperValue = 64;

   i2c_start_wait(potAddress | I2C_WRITE);
   i2c_write(0x00);
   i2c_write(wiperValue);
   i2c_stop();
}

void updateOLED() {
    char buffer[16];
    sprintf(buffer, "Freq: %d", frequency);
    i2c_start_wait(OLED_I2C_ADDR | I2C_WRITE);
    i2c_write(0x40);

    for (uint8_t i = 0; i < strlen(buffer); i++) {
        i2c_write(buffer[i]);
    }
    i2c_stop();
}
void updateDP(){
    i2c_start_wait(MCP4017_ADDRESS | I2C_WRITE);
    uint8_t wiperValue = uint8_t (0x7F - (frequency / 10);
    i2c_write(0x00);
    i2c_write(potValue);
    i2c_stop();
}

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

int main(void) {
    initEncoder();
    initOLED();

    while(1) {
        if (encoderValue != encoderLast) {
            frequency += encoderValue;
            encoderLast = encoderValue;

            if (frequency < 100) { 
                frequency = 100;
            } else if (frequency > 120) {
                frequency = 120;
            }
            updateDP();
            updateOLED();
        }
        _delay_ms(100);  // Delay to reduce display update frequency
    }

    return 0;
}

