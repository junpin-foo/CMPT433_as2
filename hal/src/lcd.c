#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <draw_stuff.h>
#include <hal/pwm_rotary.h>
#include <hal/light_sensor.h>

static bool isInitialized = false;
static pthread_t lcdThread;

static void *lcd_thread(void* arg) {
    (void)arg;
    while(1){
        char hz[100], dips[100], ms[100];

        snprintf(hz, sizeof(hz), "%dHz", PwmRotary_getFrequency()); 
        snprintf(dips, sizeof(dips), "%d", Sampler_getDipCount());  
        snprintf(ms, sizeof(ms), "%.2f", Sampler_getMaxTime());

        DrawStuff_updateScreen(hz, dips, ms);

        sleep(1);

    }
    return NULL;
}


void Lcd_init()
{
    assert(!isInitialized);

    // Exception handling:ctrl + c
    // signal(SIGINT, Handler_1IN54_LCD);
    
    // Module Init
	DrawStuff_init();
    pthread_create(&lcdThread, NULL, &lcd_thread, NULL);
    isInitialized = true;
}
void Lcd_cleanup()
{
    assert(isInitialized);
    pthread_join(lcdThread, NULL);

    DrawStuff_cleanup();
    isInitialized = false;
}