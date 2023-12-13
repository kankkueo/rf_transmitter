
main: main.out
	avr-objcopy -O ihex -R .eeprom main.out main.hex

main.out: main.c
	avr-gcc -Os -DF_APU=16000000UL -mmcu=atmega328p -o main.out main.c

upload:
	avrdude -F -V -c avrisp -p ATMEGA328PB -P /dev/ttyACM1 -b 19200 -U flash:w:main.hex

clean:
	rm *.hex *.out
