#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <draw_stuff.h>
#include <unistd.h>



static bool isInitialized = false;
static pthread_t lcdThread;

static void *lcd_thread(void* arg) {
    (void)arg;
    while(1){
        char hz[16], dips[16], ms[16];

        snprintf(hz, sizeof(hz), "%d", 0);   //update values
        snprintf(dips, sizeof(dips), "%d", 500);  
        snprintf(ms, sizeof(ms), "%d", 1500); 

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