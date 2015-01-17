#pragma once

#define BUTTON_HOLD   0x100 	// button was held
#define REPEAT_THRESHOLD 15

void Sleep_kc(uint16_t kiloclocks);
// note: use constants, so the compiler does this math, not the poor AVR
#define Sleep(ms) Sleep_kc((int)((ms) * (F_CPU / 1024000.0)))
uint16_t GetButtons(uint8_t pin, uint8_t mask);
