/* lcd.h
* LCD module starts thread that updates the screen with the current frequency, dip count and max time every second.
*/
#ifndef _LCD_H_
#define _LCD_H_

/* 
* Lcd_init starts thread that updates the screen with the current frequency, dip count and max time every second.
* It initializes the draw_stuff module.
*/
void Lcd_init();
void Lcd_cleanup();

#endif