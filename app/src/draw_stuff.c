#include "draw_stuff.h"

#include "DEV_Config.h"
#include "LCD_1in54.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <stdbool.h>
#include <assert.h>


static UWORD *s_fb;
static bool isInitialized = false;

void DrawStuff_init()
{
    assert(!isInitialized);

    // Exception handling:ctrl + c
    // signal(SIGINT, Handler_1IN54_LCD);
    
    // Module Init
	if(DEV_ModuleInit() != 0){
        DEV_ModuleExit();
        exit(0);
    }
	
    // LCD Init
    DEV_Delay_ms(2000);
	LCD_1IN54_Init(HORIZONTAL);
	LCD_1IN54_Clear(WHITE);
	LCD_SetBacklight(1023);

    UDOUBLE Imagesize = LCD_1IN54_HEIGHT*LCD_1IN54_WIDTH*2;
    if((s_fb = (UWORD *)malloc(Imagesize)) == NULL) {
        perror("Failed to apply for black memory");
        exit(0);
    }
    isInitialized = true;
}
void DrawStuff_cleanup()
{
    assert(isInitialized);

    // Module Exit
    free(s_fb);
    s_fb = NULL;
	DEV_ModuleExit();
    isInitialized = false;
}

void DrawStuff_updateScreen(char* hz, char* dips, char* ms)
{
    assert(isInitialized);

    const int x = 5;
    int y = 40; // Start at a higher position for multiple lines

    // Initialize the RAM frame buffer to be blank (white)
    Paint_NewImage(s_fb, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);

    // Draw each line of information separately
    Paint_DrawString_EN(x, y, "JP Foo's!", &Font20, WHITE, BLACK);
    y += 20; // Move to the next line
    Paint_DrawString_EN(x, y, "Flashes @ ", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(x + 140, y, hz, &Font20, WHITE, BLACK);
    int hz_width = strlen(hz) * 15; // Approximate width of `hz` in pixels
    Paint_DrawString_EN(x + 140 + hz_width + 5, y, "Hz", &Font20, WHITE, BLACK);
    y += 20;
    Paint_DrawString_EN(x, y, "Dips =  ", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(x + 120, y, dips, &Font20, WHITE, BLACK);
    y += 20;
    Paint_DrawString_EN(x, y, "Max ms: ", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(x + 160, y, ms, &Font20, WHITE, BLACK);

    // Send the RAM frame buffer to the LCD (actually display it)
    LCD_1IN54_Display(s_fb);
}
