/* main.c
* Start light sampler project.
*/
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include "hal/light_sensor.h"
#include "hal/udp_listener.h"
#include "hal/rotary_encoder_statemachine.h"
#include "hal/pwm_rotary.h"
#include "hal/lcd.h"


int main() {
    //Starts each thread and initializes the hardware, such as UDP listener, light sensor, rotary encoder, PWM, and LCD.
    UdpListener_init();
    Sampler_init();
    Lcd_init();
    
    UdpListener_cleanup();
    Sampler_cleanup();
    Lcd_cleanup();
    return 0;
}
