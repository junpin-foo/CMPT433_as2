/*udp_listener.h
    * 
    * This file declares the UDP listener module, which is responsible for  
    * receiving UDP messages and handling incoming data.  
    *  
    * The listener runs in a separate thread and interacts with the Sampler module  
    * to process and retrieve the required data.  
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