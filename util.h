#pragma once

#define BUTTON_HOLD   0x100 	// button was held
#define REPEAT_THRESHOLD 15

void Sleep(uint16_t kiloclocks);
uint16_t GetButtons(uint8_t pin, uint8_t mask);