/*pwm_rotary.h
    * 
    *   This file declares a structure for defining PWM rotary and functions for initializing,
    *   cleaning up. Where it starts a thread that updates light emitter based on rotary encoder.
    */

#ifndef _PWM_ROTARY_H_
#define _PWM_ROTARY_H_

#include <stdio.h>
#include <stdlib.h>

void PwmRotary_init(void);
void PwmRotary_cleanup(void);

#endif