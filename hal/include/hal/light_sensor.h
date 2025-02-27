/* light_sensor.h
 * 
 * This module is responsible for sampling light levels in the background using a thread.  
 * It continuously collects light intensity readings and stores them internally.  
 *  
 * Features:  
 * - Continuously samples light levels and maintains a history.  
 * - Provides access to light data from the previous complete second.  
 * - Supports data synchronization for actions like computing dips and logging.  
 * - Allows retrieval of historical data and statistical insights. 
 * 
 *  Usage Notes:  
 * - The application must call `Sampler_moveCurrentDataToHistory()` every second  
 *   to transfer the latest collected samples into the history buffer.  
 * - The module supports querying historical light data, including dips, averages,  
 *   and sample counts.
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