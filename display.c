#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"

const uint8_t digits[10] PROGMEM = {
	0x3f, // 0 = 00111111
	0x06, // 1 = 00000110
	0x5b, // 2 = 01011011
	0x4f, // 3 = 01001111
	0x66, // 4 = 01100110
	0x6d, // 5 = 01101101
	0x7c, // 6 = 01111100
	0x07, // 7 = 00000111
	0x7f, // 8 = 01111111
	0x67, // 9 = 01100111
};

void display_init()
{
	DDRB  = 0b00111100;
	PORTB = 0b00000000;
	DDRD  = 0b11111111;
	PORTD = 0b00000000;
	
	// prescaler 1/64; at 8MHz system clock, this gives us an overflow
	// at 488 Hz, providing a per-digit refresh rate of 122 Hz.
	TCCR0A = 0;
	TCCR0B = (1<<CS01) | (1<<CS00); 

	// Output compare value A - controls blanking.
	// In full brightness mode, we'll make this happen immediately before the refresh,
	// In lower brightness modes, we'll make it happen sooner.
	OCR0A = 255;

	// Enable overflow and compare match interrupts
    TIMSK0 = (1<<TOIE0) | (1<<OCIE0A);
}

// Refresh interrupt - refreshes the next digit on the display.
// By drawing each in turn quickly enough, we give the illusion of
// a solid display, but without requiring the output ports and wiring
// to drive each digit independently.
volatile uint8_t display[4] = { '\x00', '\x00', '\x00', '\x00' };

ISR(TIMER0_OVF_vect)
{
	static uint8_t didx = 0;
	PORTD = display[didx];
	DIGIT_ON(didx);
	didx = (didx + 1) & 3;
}

// Blanking interrupt - clears the display prior to the next refresh.
// We need to turn the digits off before switching segments to
// prevent ghosting caused by the wrong value being briefly displayed.
// By changing the value of OCR0A, we can control the effective 
// brightness of the display.
ISR(TIMER0_COMPA_vect)
{
	DIGITS_OFF();
}

// blank-padded, right-aligned, rolling over 10,000
void display_number(uint16_t num)
{
	char buf[6], *p;
	volatile uint8_t *d;
	
	itoa(num, buf, 10);
	
	p = &buf[strlen(buf) - 1];
	d = &display[3];
	while(p >= buf && d >= display) {
		*d-- = pgm_read_byte(&digits[*p-- - '0']);
	}
	while(d >= display) {
		*d-- = 0;
	}
}