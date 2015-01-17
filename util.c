#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "util.h"

// Put the processor in idle mode for the specified number of "kiloclocks"
// (= periods of 1024 clock cycles)
volatile uint8_t wakeup;
ISR(TIMER1_COMPA_vect)
{
	wakeup = 1;
}
void Sleep_kc(uint16_t kiloclocks)
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

// poll this function every 50ms or so.
// here's how button presses work:
// - a press is registered when a button is released.
// - a hold is registered when the same button has been down
//   for a specified number of cycles.  the button release
//   following the hold does not register.
uint16_t GetButtons(uint8_t pin, uint8_t mask)
{
	static uint8_t prevState = 0xff;
	static uint8_t repeat = 0;

	uint8_t curState = (pin & mask); // 1 = not pressed; 0 = pressed
	
	// if we've already registered a "hold"
	if (repeat >= REPEAT_THRESHOLD) {
		prevState = curState;
		if (curState == mask)
			repeat = 0;	// no buttons are down.
		return 0;
	}

	if (curState != prevState) {
		// truth table:
		//  prev cur  released
		//  0    0    0
		//  0    1    1
		//  1    0    0
		//  1    1    0
		uint8_t released = ~prevState & curState & mask;
		prevState = curState;
		return released;
	} else if (curState != mask) {
		// button(s) are being held
		if (++repeat == REPEAT_THRESHOLD) {
			return BUTTON_HOLD | (~curState & mask);
		}
	} 
	
	return 0;
}
