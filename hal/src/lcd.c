/* lcd.c
* LCD module starts thread that updates the screen with the current frequency, dip count and max time every second.
*/

#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <updateLcd.h>
#include <hal/pwm_rotary.h>
#include <hal/light_sensor.h>
#include "hal/udp_listener.h"


#define BUFFER_SIZE 100

static bool isInitialized = false;
static pthread_t lcdThread;
// static volatile bool lcdRunning = true;

static void *lcd_thread(void* arg) {
    (void)arg;
    while(UdpListener_isRunning()){
        char hz[BUFFER_SIZE], dips[BUFFER_SIZE], ms[BUFFER_SIZE];

        snprintf(hz, sizeof(hz), "%dHz", PwmRotary_getFrequency()); 
        snprintf(dips, sizeof(dips), "%d", Sampler_getDipCount());  
        snprintf(ms, sizeof(ms), "%.2f", Sampler_getMaxTime());

        UpdateLcd_updateScreen(hz, dips, ms);

        sleep(1);

    }
    return NULL;
}


void Lcd_init()
{
    assert(!isInitialized);
    
    // Module Init
	UpdateLcd_init();
    pthread_create(&lcdThread, NULL, &lcd_thread, NULL);
    isInitialized = true;
}
void Lcd_cleanup()
{
    assert(isInitialized);
    // lcdRunning = false;
    pthread_join(lcdThread, NULL);
    UpdateLcd_cleanup();
    isInitialized = false;
}