/* i2c.c
 * 
 * This file provides initialization, cleanup, and register read/write 
 * operations for I2C communication using the Linux I2C driver. It includes 
 * functions to open an I2C bus, configure it for a specific slave device, 
 * and perform 16-bit register read and write operations.
 */

#include "hal/i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <time.h>

#define SLEEP 55000

static bool isInitialized = false;

void I2c_initialize(void) {
    isInitialized = true;
}

void I2c_cleanUp(void) {
    isInitialized = false;
}


int init_i2c_bus(const char* bus, int address) {
    if (!isInitialized) {
        perror("Error: Ic2 not initialized!\n");
        exit(EXIT_FAILURE);
    }

    int i2c_file_desc = open(bus, O_RDWR);
    if (i2c_file_desc == -1) {
        printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
        perror("Error is:");
        exit(EXIT_FAILURE);
    }

    if (ioctl(i2c_file_desc, I2C_SLAVE, address) == -1) {
        perror("Unable to set I2C device to slave address.");
        exit(EXIT_FAILURE);
    }

    return i2c_file_desc;
}

void write_i2c_reg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value) {
    if (!isInitialized) {
        perror("Error: Ic2 not initialized!\n");
        exit(EXIT_FAILURE);
    }

    int tx_size = 1 + sizeof(value);
    uint8_t buff[tx_size];
    buff[0] = reg_addr;
    buff[1] = (value & 0xFF);
    buff[2] = (value & 0xFF00) >> 8;

    int bytes_written = write(i2c_file_desc, buff, tx_size);
    if (bytes_written != tx_size) {
        perror("Unable to write i2c register");
        exit(EXIT_FAILURE);
    }
    struct timespec reqDelay = {0, SLEEP};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

uint16_t read_i2c_reg16(int i2c_file_desc, uint8_t reg_addr) {
    if (!isInitialized) {
        perror("Error: Ic2 not initialized!\n");
        exit(EXIT_FAILURE);
    }

    int bytes_written = write(i2c_file_desc, &reg_addr, sizeof(reg_addr));
    if (bytes_written != sizeof(reg_addr)) {
        perror("Unable to write i2c register.");
        exit(EXIT_FAILURE);
    }

    uint16_t value = 0;
    int bytes_read = read(i2c_file_desc, &value, sizeof(value));
    if (bytes_read != sizeof(value)) {
        perror("Unable to read i2c register");
        exit(EXIT_FAILURE);
    }

    struct timespec reqDelay = {0, SLEEP};
    nanosleep(&reqDelay, (struct timespec *) NULL);
    return value;
}