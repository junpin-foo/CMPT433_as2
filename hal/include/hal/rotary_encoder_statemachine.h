/* rotary_encoder_statemachine.h 
* header file for Rotary encoder state machine.
*/
#ifndef _BTN_STATEMACHINE_H_
#define _BTN_STATEMACHINE_H_

#include <stdbool.h>

//Start thread to monitor the rotary encoder
void RotaryEncoderStateMachine_init(void);

void RotaryEncoderStateMachine_cleanup(void);

//Get the current value of the rotary encoder
int RotaryEncoderStateMachine_getValue(void);

//Set the value of the rotary encoder (mainly for resetting purposes)
void RotaryEncoderStateMachine_setValue(int value);

#endif