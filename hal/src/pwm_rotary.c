/*pwm_rotary.c
    * 
    *  This file defines a structure for defining PWM rotary and functions for initializing,
    *  cleaning up. Where it starts a thread that updates light emitter based on rotary encoder.
    * 
    */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include <gpiod.h>
#include <fcntl.h>
#include <assert.h>
#include "hal/rotary_encoder_statemachine.h"

#define PWM_PATH "/dev/hat/pwm/GPIO12/"
#define NANOSECONDS_IN_1SECOND 1000000000
#define MIN_FREQUENCY 0
#define MAX_FREQUENCY 500
#define BASE_FREQUENCY 10

atomic_int frequency = BASE_FREQUENCY;
static bool isInitialized = false;
static pthread_mutex_t pwm_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t pwmThread;

static void set_pwm_frequency(int hz) {
    if (hz < MIN_FREQUENCY) hz = MIN_FREQUENCY; //Supporting only from 0 to 500 Hz
    if (hz > MAX_FREQUENCY) hz = MAX_FREQUENCY;
    
    if (hz == frequency) return; // Avoid unnecessary updates
    frequency = hz;

    pthread_mutex_lock(&pwm_mutex);
    
    int period_ns = hz == 0 ? 0 : NANOSECONDS_IN_1SECOND / hz;
    int duty_cycle_ns = period_ns / 2;
    
    FILE *fp;
    fp = fopen(PWM_PATH "duty_cycle", "w");
    fprintf(fp, "0"); fclose(fp);
    
    fp = fopen(PWM_PATH "period", "w");
    fprintf(fp, "%d", period_ns); fclose(fp);
    
    fp = fopen(PWM_PATH "duty_cycle", "w");
    fprintf(fp, "%d", duty_cycle_ns); fclose(fp);
    
    fp = fopen(PWM_PATH "enable", "w");
    fprintf(fp, "1"); fclose(fp);
    
    pthread_mutex_unlock(&pwm_mutex);
    printf("Set frequency to %d Hz\n", hz);
}

static void *encoder_thread(void *arg) {
    (void)arg; // Suppress unused parameter warning
    while (1) {
        int counter_value = RotaryEncoderStateMachine_getValue();
        if (counter_value != 0) {
            int new_frequency = frequency + counter_value;

            set_pwm_frequency(new_frequency);
            
            // Reset the counter to avoid accumulation
            RotaryEncoderStateMachine_setValue(0);
        }


        sleep(1);
    }
    return NULL;
}

void PwmRotary_init(void){
    RotaryEncoderStateMachine_init();
    pthread_create(&pwmThread, NULL, &encoder_thread, NULL);
    isInitialized = true;
}
void PwmRotary_cleanup(void){
    assert(isInitialized);
    RotaryEncoderStateMachine_cleanup();
    isInitialized = false;
}