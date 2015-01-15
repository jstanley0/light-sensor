#pragma once

extern const uint8_t digits[10] PROGMEM;

#define DIGITS_OFF()   { PORTB |= 0x3c; }
#define DIGIT_ON(x)    { PORTB &= (4 << x) ^ 0x3c; }

void display_init();
void display_number(uint16_t num);

extern volatile uint8_t display[4];
