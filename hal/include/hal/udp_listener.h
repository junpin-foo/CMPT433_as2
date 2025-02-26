/*udp_listener.h
    * 
    * This file declares a structure for defining UDP listeners and functions for initializing, 
    * cleaning up.
    * 
    * The listener runs in a separate thread and uses the Sampler module to get the required data.
    */

#ifndef _UDP_LISTENER_H_
#define _UDP_LISTENER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// start thread to listen for UDP messages
void UdpListener_init(void);

// clean up thread
void UdpListener_cleanup(void);

//return stop running flag
bool UdpListener_isRunning(void);


#endif