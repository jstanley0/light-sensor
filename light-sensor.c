// light sensor
// based on AVR ATMEGA168
// build with avr-gcc
//
// Source code (c) 2007-2015 by Jeremy Stanley
// Licensed under GNU GPL v2 or later
//

// Port assignments:
// PC0    (input)  = teh button
// PD0..7 (output) = segment cathodes
// PB2..5 (output) = digit anodes

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

// Put the processor in idle mode for the specified number of "kiloclocks"
// (= periods of 1024 clock cycles)

volatile uint8_t wakeup;
ISR(TIMER1_COMPA_vect)
{
	wakeup = 1;
}

void Sleep(uint16_t kiloclocks)
{
	TCCR1A = 0;
	TCCR1B = 0;				// stop the timer
	TIFR1 = (1 << OCF1A);	// clear output-compare A flag
	OCR1A = kiloclocks;		// set compare match A target
	TCNT1 = 0;				// reset timer counter
	TIMSK1 = (1 << OCIE1A);	// enable compare match A interrupt
	TCCR1B = (1 << CS12) | (1 << CS10);	// start timer with 1/1024 prescaler

	// sleep until it's time to wake up
	// use a loop here because other interrupts will happen
	wakeup = 0;
	set_sleep_mode(SLEEP_MODE_IDLE);
	do {
		sleep_mode();
	} while( !wakeup );

	TIMSK1 = 0;				// stop the interrupt
	TCCR1B = 0;				// stop the timer
}


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

#define DIGITS_OFF()   { PORTB &= (uint8_t)0xc3; }
#define DIGIT_ON(x)    { PORTB |= (4 << x) ^ 0x3c; }


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

// here's how button presses work:
// - a press is registered when a button is released.
// - a hold is registered when the same button has been down
//   for a specified number of cycles.  the button release
//   following the hold does not register.

#define TEH_BUTTON    0x01
#define BUTTON_HOLD  0x10 	// button was held
#define REPEAT_THRESHOLD 15

uint8_t GetButtons()
{
	static uint8_t prevState = 0xff;
	static uint8_t repeat = 0;

	uint8_t curState = (PINC & 1);
	
	// if we've already registered a "hold"
	if (repeat >= REPEAT_THRESHOLD) {
		prevState = curState;
		if (curState == 7)
			repeat = 0;	// no buttons are down.
		return 0;
	}

	if (curState != prevState) {
		uint8_t pressed = ~prevState & curState;
		prevState = curState;
		return pressed;
	} else if (curState != 7) {
		// button(s) are being held
		if (++repeat == REPEAT_THRESHOLD) {
			return BUTTON_HOLD | ~(curState & 7);
		}
	} 
	
	return 0;
}

int main(void)
{
	// Initialize I/O
	DDRB  = 0b00111100;
	PORTB = 0b00000000;
	DDRC  = 0b00000000;
	PORTC = 0b00000001;
	DDRD  = 0b11111111;
	PORTD = 0b00000000;

	// Setup the display timer...
	
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

