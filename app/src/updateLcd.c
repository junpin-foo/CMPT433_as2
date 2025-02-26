/* updateLcd.c
* Provided file to update LCD screen, updated DrawStuff_updateScreen to diplay name, frequency, dips, and max ms.
*/
#include "updateLcd.h"

#include "DEV_Config.h"
#include "LCD_1in54.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <stdbool.h>
#include <assert.h>

#define DELAY_MS 2000
#define BACKLIGHT 1023
#define INITIAL_X 5
#define INITIAL_Y 40
#define NEXTLINE_Y 40
#define FREQUENCY_X 140
#define DIPS_X 120
#define MAX_MS_X 160

static UWORD *s_fb;
static bool isInitialized = false;

void UpdateLcd_init()
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
    DEV_Delay_ms(DELAY_MS);
	LCD_1IN54_Init(HORIZONTAL);
	LCD_1IN54_Clear(WHITE);
	LCD_SetBacklight(BACKLIGHT);

    UDOUBLE Imagesize = LCD_1IN54_HEIGHT*LCD_1IN54_WIDTH*2;
    if((s_fb = (UWORD *)malloc(Imagesize)) == NULL) {
        perror("Failed to apply for black memory");
        exit(0);
    }
    isInitialized = true;
}
void UpdateLcd_cleanup()
{
    assert(isInitialized);

    // Module Exit
    free(s_fb);
    s_fb = NULL;
	DEV_ModuleExit();
    isInitialized = false;
}

void UpdateLcd_updateScreen(char* hz, char* dips, char* ms)
{
    assert(isInitialized);

    const int x = INITIAL_X;
    int y = INITIAL_Y; // Start at a higher position for multiple lines

    // Initialize the RAM frame buffer to be blank (white)
    Paint_NewImage(s_fb, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);

    Paint_DrawString_EN(x, y, "JP Foo's!", &Font20, WHITE, BLACK);
    y += NEXTLINE_Y; // Move to the next line
    Paint_DrawString_EN(x, y, "Flashes @ ", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(x + FREQUENCY_X, y, hz, &Font20, WHITE, BLACK);
    y += NEXTLINE_Y;
    Paint_DrawString_EN(x, y, "Dips =  ", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(x + DIPS_X, y, dips, &Font20, WHITE, BLACK);
    y += NEXTLINE_Y;
    Paint_DrawString_EN(x, y, "Max ms: ", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(x + MAX_MS_X, y, ms, &Font20, WHITE, BLACK);

    // Send the RAM frame buffer to the LCD (actually display it)
    LCD_1IN54_Display(s_fb);
}
