// /*
// 	State machine for GPIO
// */

// // Relies on the gpiod library.
// // Insallation for cross compiling:
// //      (host)$ sudo dpkg --add-architecture arm64
// //      (host)$ sudo apt update
// //      (host)$ sudo apt install libgpdiod-dev:arm64
// // GPIO: https://www.ics.com/blog/gpio-programming-exploring-libgpiod-library
// // Example: https://github.com/starnight/libgpiod-example/blob/master/libgpiod-input/main.c

// #include <stdio.h>
// #include <unistd.h>
// #include <stdbool.h>

// #include "gpio.h"
// #include "btn_statemachine.h"

// #include <time.h>

// int main(void) 
// {
//     // Startup & Initialization
//     Gpio_initialize();
//     BtnStateMachine_init();

//     // TESTING State machine
//     while (true) {
//         // TODO: This should be on it's own thread!
//         BtnStateMachine_doState();

//         printf("Counter at %+5d\n", BtnStateMachine_getValue());
//     }

//     BtnStateMachine_cleanup();
//     Gpio_cleanup();

//     printf("\nDone!\n");
//     return 0;
// }

// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "hal/light_sensor.h"
#include "hal/udp_listener.h"
#include "hal/rotary_encoder_statemachine.h"
#include "hal/pwm_rotary.h"
#include "hal/lcd.h"

int main() {

    printf("Initializing sampler...\n");
    // UdpListener_init();
    // Sampler_init();
    PwmRotary_init();
    Lcd_init();
    printf("sampler initialized successfully.\n");

    // int count = 100;
    // while (count > 0) {
    //     // Get the joystick reading
    //     Sampler_getReading();
    //     count--;
    // }
    // Sampler_moveCurrentDataToHistory();
    // printf("history size: %d\n", Sampler_getHistorySize());

    // int size;
    // double* history = Sampler_getHistory(&size);
    // if (history) {
    //     for (int i = 0; i < size; i++) {
    //         printf("Sample %d: %f\n", i, history[i]);
    //     }
    //     free(history); // Prevent memory leak!
    // } else {
    //     fprintf(stderr, "Failed to retrieve history samples.\n");
    // }

    // Keep the main thread running
    while (1) {
        struct timespec reqDelay = {1, 0}; //Sample every 1ms
        nanosleep(&reqDelay, (struct timespec *) NULL);
    }

    // UdpListener_cleanup(); // Cleanup if needed (won't be reached in this case)
    return 0;

}
