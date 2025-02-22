/* light_sensor.c
 * 
 * This file provides functions to initialize and clean up LED control, 
 * set LED triggers, brightness, and delays by interacting with the 
 * sysfs files for the LEDs.
 */

#include "hal/light_sensor.h"
#include "hal/i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define I2CDRV_LINUX_BUS "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x48 // ADC chip
#define REG_CONFIGURATION 0x01
#define REG_DATA 0x00
#define TLA2024_CHANNEL_CONF_2 0x83E2 // Configuration for light sensor
#define MAX_HISTORY_SIZE 1000 // Maximum samples stored per second
#define SMOOTHING_FACTOR 0.001 // 0.1% new sample, 99.9% previous average
#define VOLTAGE_CONVERSION_FACTOR (3.3 / 4096)
#define DIP_THRESHOLD 0.1  // 0.1V drop to trigger a dip
#define HYSTERESIS 0.03  // 0.07V rise needed before another dip


static double currentSamples[MAX_HISTORY_SIZE];
static double historySamples[MAX_HISTORY_SIZE];
static int currentSampleCount = 0;
static int historySampleCount = 0;
static long long totalSamplesTaken = 0;
static double smoothedAverage = 0.0; // Exponential moving average
static bool isFirstSample = true;
static int dipCount = 0;
static double lastVoltage = 0.0;
static bool belowThreshold = false;

static int i2c_file_desc = -1;
static bool isInitialized = false;
static pthread_t samplerThread;
static bool keepSampling = true;
static pthread_mutex_t sampleMutex = PTHREAD_MUTEX_INITIALIZER;

// Function Prototypes
void Sampler_init(void);
void Sampler_cleanup(void);
double Sampler_getReading(void);
void Sampler_moveCurrentDataToHistory(void);
int Sampler_getHistorySize(void);
double* Sampler_getHistory(int *size);
double Sampler_getAverageReading(void);
long long Sampler_getNumSamplesTaken(void);
int Sampler_getDipCount(void);
static void* samplerThreadFunc(void* arg);
static void Sampler_detectDips(void);


static void* samplerThreadFunc(void* arg) {
    (void)arg; // Suppress unused parameter warning
    while (keepSampling) {
    static struct timespec lastMoveTime = {0, 0};  
    struct timespec currentTime;
    
    Sampler_getReading();

    // Sleep for 1ms
    struct timespec reqDelay = {0, 1000000}; 
    nanosleep(&reqDelay, NULL);

    // Get current time
    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    // Check if 1 second has passed
    if (currentTime.tv_sec > lastMoveTime.tv_sec) {
        Sampler_moveCurrentDataToHistory();
        lastMoveTime = currentTime;  // Update last move time
        Sampler_detectDips();
    }
}
    return NULL;
}

void Sampler_init(void) {
    I2c_initialize();
    i2c_file_desc = init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    currentSampleCount = 0;
    historySampleCount = 0;
    totalSamplesTaken = 0;
    keepSampling = true;
    pthread_create(&samplerThread, NULL, &samplerThreadFunc, NULL);
    isInitialized = true;
}

void Sampler_cleanup(void) {
    keepSampling = false;
    pthread_join(samplerThread, NULL);
    I2c_cleanUp();
    isInitialized = false;
}

double Sampler_getReading() {
    if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }

    write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, TLA2024_CHANNEL_CONF_2);
    uint16_t raw_value = read_i2c_reg16(i2c_file_desc, REG_DATA);
    uint16_t shifted_value = ((raw_value & 0xFF00) >> 8 | (raw_value & 0x00FF) << 8) >> 4;
    double reading = (double)shifted_value;
    // printf("Sensor current: %f\n", reading);

    pthread_mutex_lock(&sampleMutex);
    // Update the exponential moving average
    if (isFirstSample) {
        smoothedAverage = reading;
        isFirstSample = false;
    } else {
        smoothedAverage = (SMOOTHING_FACTOR * reading) + ((1.0 - SMOOTHING_FACTOR) * smoothedAverage);
    }

    // Store the sample
    if (currentSampleCount < MAX_HISTORY_SIZE) {
        currentSamples[currentSampleCount++] = reading;
    }
    totalSamplesTaken++;
    pthread_mutex_unlock(&sampleMutex);

    return reading;
}

void Sampler_moveCurrentDataToHistory(void) {
    if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&sampleMutex);
    memcpy(historySamples, currentSamples, currentSampleCount * sizeof(double));
    historySampleCount = currentSampleCount;
    currentSampleCount = 0;
    pthread_mutex_unlock(&sampleMutex);
}

int Sampler_getHistorySize(void) {
    if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }

    return historySampleCount;
}

double* Sampler_getHistory(int *size) {
    if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }

    if (!size) return NULL;

    pthread_mutex_lock(&sampleMutex);
    *size = historySampleCount;
    
    double* copy = (double*)malloc(*size * sizeof(double));
    if (!copy) {
        fprintf(stderr, "Error: Memory allocation failed in Sampler_getHistory()\n");
        pthread_mutex_unlock(&sampleMutex);
        return NULL;
    }

    memcpy(copy, historySamples, *size * sizeof(double));
    pthread_mutex_unlock(&sampleMutex);

    return copy; // Caller must free this
}

double Sampler_getAverageReading(void) {
     if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }

    return smoothedAverage;
}

long long Sampler_getNumSamplesTaken(void) {
    if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }

    return totalSamplesTaken;
}

static void Sampler_detectDips(void) {
    if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&sampleMutex);
    dipCount = 0; // Reset dip count for this second
    for (int i = 0; i < historySampleCount; i++) {
        double voltage = VOLTAGE_CONVERSION_FACTOR * historySamples[i];
        double threshold = smoothedAverage * VOLTAGE_CONVERSION_FACTOR - DIP_THRESHOLD;
        double resetThreshold = smoothedAverage * VOLTAGE_CONVERSION_FACTOR - (DIP_THRESHOLD - HYSTERESIS);

        if (!belowThreshold && voltage < threshold) {
            dipCount++;
            belowThreshold = true;
        } else if (belowThreshold && voltage > resetThreshold) {
            belowThreshold = false;
        }

        lastVoltage = voltage;
    }
    pthread_mutex_unlock(&sampleMutex);

    printf("Dips detected: %d\n", dipCount);
}

int Sampler_getDipCount(void) {
    if (!isInitialized) {
        fprintf(stderr, "Error: LightSensor not initialized!\n");
        exit(EXIT_FAILURE);
    }

    return dipCount;
}
