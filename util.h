#pragma once

#define BUTTON_HOLD   0x100 	// button was held
#define REPEAT_THRESHOLD 15

void Sleep_kc(uint16_t kiloclocks);
#define Sleep(ms) Sleep_kc(ms * 8) // eh, not quite
uint16_t GetButtons(uint8_t pin, uint8_t mask);