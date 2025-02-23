#ifndef _BTN_STATEMACHINE_H_
#define _BTN_STATEMACHINE_H_

#include <stdbool.h>

void RotaryEncoderStateMachine_init(void);
void RotaryEncoderStateMachine_cleanup(void);
int RotaryEncoderStateMachine_getValue(void);
void RotaryEncoderStateMachine_setValue(int value);

#endif