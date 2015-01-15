#pragma once

extern const uint8_t digits[10] PROGMEM;

#define DIGITS_OFF()   { PORTB &= (uint8_t)0xc3; }
#define DIGIT_ON(x)    { PORTB |= (4 << x) ^ 0x3c; }

void display_init();
extern volatile uint8_t display[4];