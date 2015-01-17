// RGB light sensor test app
// displays ADC value for photoresistor and optionally lights the subject
// with a red, green, or blue LED
// based on AVR ATMEGA168
// build with avr-gcc
//
// Source code (c) 2007-2015 by Jeremy Stanley
// Licensed under GNU GPL v2 or later

// Port assignments:
// PC0    (input)  = teh button
// PC1    (input)  = photoresistor
// PC3    (output) = red LED
// PC4    (output) = green LED
// PC5    (output) = blue LED
// PD0..7 (output) = segment anodes
// PB2..5 (output) = digit cathodes

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "util.h"
#include "display.h"

void adc_init()
{
	// power on the ADC
	PRR &= ~(1 << PRADC);
	
	// select AVCC reference, ADC1 source
	ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX0);
	
	// enable ADC and start conversions at 1/128 prescaler
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

ISR(ADC_vect)
{
	// average a whole bunch of readings for the display
	static uint32_t accum = 0;
	static uint16_t count = 0;
	
	accum += ADC;
	if (++count == 2048) {
		display_number(accum >> 11);
		count = 0;
		accum = 0;
	}
}

int main(void)
{
	display_init();
	adc_init();

	// Initialize I/O
	DDRC  = 0b00111000;
	PORTC = 0b00000011;
    
	// Enable interrupts
	sei();

	uint8_t led = 0;
	for(;;)
	{
		// toggle color if button is pressed
		uint8_t buttons = GetButtons(PINC, 1);
		if (buttons & 1) {
			led = (led < 3) ? led + 1 : 0;
			PORTC &= 0b11000111;
			PORTC |= ((0b100 << led) & 0b00111000);
		}
			
		Sleep(50);
 	}
}

