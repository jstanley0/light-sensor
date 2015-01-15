// light sensor
// based on AVR ATMEGA168
// build with avr-gcc
//
// Source code (c) 2007-2015 by Jeremy Stanley
// Licensed under GNU GPL v2 or later

// Port assignments:
// PC0    (input)  = teh button
// PC1    (input)  = photoresistor
// PD0..7 (output) = segment anodes
// PB2..5 (output) = digit cathodes

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "util.h"
#include "display.h"

int main(void)
{
	display_init();

	// Initialize I/O
	DDRC  = 0b00000000;
	PORTC = 0b00000011;

	// make the display interesting
	int i;
	for(i = 0; i < 4; ++i)
		display[i] = pgm_read_byte(&digits[i + 6]);
    
	// Enable interrupts
	sei();

	for(;;)
	{
		Sleep(480);	// approx 62ms at 8MHz
 	}
}

