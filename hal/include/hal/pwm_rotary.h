/* pwm_rotary.h
* 
*   This file declares a structure for defining PWM rotary and functions for initializing,
*   cleaning up. Where it starts a thread that updates light emitter based on rotary encoder.
*/

#ifndef _PWM_ROTARY_H_
#define _PWM_ROTARY_H_

#include <stdio.h>
#include <stdlib.h>

//Starts thread that monitors rotary encoder and updates PWM frequency
void PwmRotary_init(void);

void PwmRotary_cleanup(void);

//Return the current frequency of the PWM
int PwmRotary_getFrequency(void);

#endif