/* light_sensor.h
 * 
 * This file declares a structure for defining LEDs and functions for initializing, 
 * cleaning up, and controlling LED attributes like trigger, brightness, 
 * and delays using sysfs files. The `LED_FILE_NAME` macro defines the 
 * path to the sysfs LED directory.
 * 
 * // Module to sample light levels in the background (uses a thread).
//
// It continuously samples the light level, and stores it internally.
// It provides access to the samples it recorded during the _previous_
// complete second.
//
// The application will do a number of actions each second which must
// be synchronized (such as computing dips and printing to the screen).
// To make easy to work with the data, the app must call
// Sampler_moveCurrentDataToHistory() each second to trigger this
// module to move the current samples into the history.
 */

#ifndef _LIGHTSENSOR_H_
#define _LIGHTSENSOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define LIGHTSENSOR_FILE_NAME "/dev/hat/pwm/GPIO12"

void Sampler_init(void);

void Sampler_cleanup(void);

double Sampler_getReading(void);

// // Must be called once every 1s.
// // Moves the samples that it has been collecting this second into
// // the history, which makes the samples available for reads (below).
void Sampler_moveCurrentDataToHistory(void);

// // Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void);

// // Get a copy of the samples in the sample history.
// // Returns a newly allocated array and sets `size` to be the
// // number of elements in the returned array (output-only parameter).
// // The calling code must call free() on the returned pointer.
// // Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(int *size);

// // Get the average light level (not tied to the history).
double Sampler_getAverageReading(void);

// // Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void);

// Return dip count for previous second samples.
int Sampler_getDipCount(void);

//Return maxTime from periodTimer
double Sampler_getMaxTime(void);


#endif