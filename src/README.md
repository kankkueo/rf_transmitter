## Software
This software and Makefile was designed to work with the 
Atmega328p MCU. The programming is done through the SPI bus
using an Arduino Uno as an in-system programmer.

[This](https://github.com/Sovichea/avr-i2c-library) library was used for the i2c communication.

### Requirements
- avr-gcc
- avr-libc
- avrdude

### Compilation
`make` to compile

`make upload` to upload the code to the MCU
