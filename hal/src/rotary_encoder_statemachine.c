/* rotary_encoder_statemachine.c 
* Rotary encoder state machine implementation as discussed in class. Uses state machine to keep track of the rotary encoder value.
*/
#include "hal/rotary_encoder_statemachine.h"
#include "hal/gpio.h"
#include "hal/udp_listener.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

#define GPIO_CHIP GPIO_CHIP_2
#define GPIO_LINE_A 7
#define GPIO_LINE_B 8


static bool isInitialized = false;
struct GpioLine* s_lineA = NULL;
struct GpioLine* s_lineB = NULL;
static atomic_int counter = 0;
static bool ccwFlag = false;
static bool cwFlag = false;
static pthread_t stateMachineThread;
// static volatile bool stateMachineRunning = true;


// Function Prototypes 
void RotaryEncoderStateMachine_init();
void RotaryEncoderStateMachine_cleanup();
static void* RotaryEncoderStateMachine_doState(void* arg);
int RotaryEncoderStateMachine_getValue();
static void on_clockwise(void);
static void on_counterclockwise(void);


/*
    Define the Statemachine Data Structures
*/
struct stateEvent {
    struct state* pNextState;
    void (*action)();
};
struct state {
    struct stateEvent aRise;
    struct stateEvent aFall;
    struct stateEvent bRise;
    struct stateEvent bFall;
};


/*
    START STATEMACHINE
*/
static void reset_flag(void) {
    cwFlag = false;
    ccwFlag = false;
}

static void on_clockwise(void) {
    if(cwFlag) {
        counter++;
        reset_flag();
    }
}
static void on_counterclockwise(void) {
    if(ccwFlag) {
        counter--;
        reset_flag();
    }
}

static void track_clockwise(void) {
    if (cwFlag == false) {
        cwFlag = true;
        ccwFlag = false;
    }
}

static void track_counterclockwise(void) {
    if (ccwFlag == false) {
        ccwFlag = true;
        cwFlag = false;
    }
}

struct state states[] = {
    { // State 0
        .aRise = {&states[0], reset_flag},
        .aFall = {&states[1], track_clockwise},
        .bRise = {&states[0], reset_flag},
        .bFall = {&states[3], track_counterclockwise},
    },
    { // State 1
        .aRise = {&states[0], on_counterclockwise},
        .aFall = {&states[1], NULL},
        .bRise = {&states[1], NULL},
        .bFall = {&states[2], NULL},
    },
    { // State 2
        .aRise = {&states[3], NULL},
        .aFall = {&states[2], NULL},
        .bRise = {&states[1], NULL},
        .bFall = {&states[2], NULL},
    },
    { // State 3
        .aRise = {&states[3], NULL},
        .aFall = {&states[2], NULL},
        .bRise = {&states[0], on_clockwise},
        .bFall = {&states[3], NULL},
    }
};
/*
    END STATEMACHINE
*/

struct state* pCurrentState = &states[0];



void RotaryEncoderStateMachine_init()
{
    assert(!isInitialized);
    Gpio_initialize();
    s_lineA = Gpio_openForEvents(GPIO_CHIP, GPIO_LINE_A);
    s_lineB = Gpio_openForEvents(GPIO_CHIP, GPIO_LINE_B);
    pthread_create(&stateMachineThread, NULL, &RotaryEncoderStateMachine_doState, NULL);
    isInitialized = true;
}
void RotaryEncoderStateMachine_cleanup()
{
    assert(isInitialized);
    // stateMachineRunning = false;
    pthread_join(stateMachineThread, NULL);
    Gpio_close(s_lineA);
    Gpio_close(s_lineB);
    Gpio_cleanup();
    isInitialized = false;
}

int RotaryEncoderStateMachine_getValue()
{
    assert(isInitialized);
    return counter;
}

void RotaryEncoderStateMachine_setValue(int value)
{
    counter = value;
}

static void* RotaryEncoderStateMachine_doState(void* arg)
{
    (void)arg; // Suppress unused parameter warning

    // printf("\n\nWaiting for an event...\n");
    while (UdpListener_isRunning()) {
        struct gpiod_line_bulk bulkEvents;
        int numEvents = Gpio_waitForLineChange(s_lineA, s_lineB, &bulkEvents);
        if (numEvents == -1) {
            break;  // Exit the loop on failure
        }

        // Iterate over the event
        for (int i = 0; i < numEvents; i++)
        {
            // Get the line handle for this event
            struct gpiod_line *line_handle = gpiod_line_bulk_get_line(&bulkEvents, i);

            // Get the number of this line
            unsigned int this_line_number = gpiod_line_offset(line_handle);

            // Get the line event
            struct gpiod_line_event event;
            if (gpiod_line_event_read(line_handle,&event) == -1) {
                perror("Line Event");
                exit(EXIT_FAILURE);
            }


            // Run the state machine
            bool isRising = event.event_type == GPIOD_LINE_EVENT_RISING_EDGE;

            // Can check with line it is, if you have more than one...
            bool isA = this_line_number == GPIO_LINE_A;
            bool isB = this_line_number == GPIO_LINE_B;

            struct stateEvent* pStateEvent = NULL;
            if (isA && isRising) {
                pStateEvent = &pCurrentState->aRise;
            } else if (isA && !isRising) {
                pStateEvent = &pCurrentState->aFall;
            } else if (isB && isRising) {
                pStateEvent = &pCurrentState->bRise;
            } else if (isB && !isRising) {
                pStateEvent = &pCurrentState->bFall;
            }


            // Do the action
            if (pStateEvent && pStateEvent->action) {
                pStateEvent->action();
            }
            pCurrentState = pStateEvent ? pStateEvent->pNextState : pCurrentState;

            // DEBUG INFO ABOUT STATEMACHINE
            #if 0
            int newState = (pCurrentState - &states[0]);
            double time = event.ts.tv_sec + event.ts.tv_nsec / 1000000000.0;
            printf("State machine Debug: i=%d/%d  line num/dir = %d %8s -> new state %d     [%f]\n", 
                i, 
                numEvents,
                this_line_number, 
                isRising ? "RISING": "falling", 
                newState,
                time);
            #endif
        }
    }
    return NULL;
}
