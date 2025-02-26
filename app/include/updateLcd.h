/* updateLcd.h
* Provided file to update LCD screen
*/
#ifndef _UPDATELCD_H_
#define _UPDATELCD_H_

//Initialize and clean up the LCD screen.
void UpdateLcd_init();
void UpdateLcd_cleanup();

// Update the LCD screen with the frequency, dips and max time between ADC light level samples.
void UpdateLcd_updateScreen(char* hz, char* dips, char* ms);

#endif