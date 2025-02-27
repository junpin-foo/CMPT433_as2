/* rotary_encoder_statemachine.h 
 * Header file for the rotary encoder state machine implementation.  
 * This module is responsible for monitoring and processing rotary encoder  
 * signals using a state machine approach. It provides functions to initialize,  
 * clean up, and retrieve the current encoder value.  
 *  
 * Features:  
 * - Uses a state machine to track rotary encoder position changes.  
 * - Supports incrementing and decrementing based on clockwise and  
 *   counterclockwise rotations.  
 * - Provides thread-based monitoring of GPIO events for reliable tracking.  
 * - Allows external components to retrieve or reset the encoder value. 
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