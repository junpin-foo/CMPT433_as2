/* udp_listner.c
 * This file implements a UDP listener that listens for commands from a client and responds with the requested data.
 * The listener listens on port 12345 and supports the following commands:
 * - help: list of commands and summary
 * - count: Return the total number of light samples taken so far
 * - length: Return how many samples were captured during the previous second
 * - dips: Return how many dips were detected during the previous second’s samples
 * - history: Return all the data samples from the previous second
 * - stop: Exit the program
 * The listener runs in a separate thread and uses the Sampler module to get the required data.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "hal/light_sensor.h"

#define PORT 12345
#define BUFFER_SIZE 1024

static pthread_t udp_thread;
static int sockfd;
static struct sockaddr_in server_addr, client_addr;
static socklen_t addr_len = sizeof(client_addr);
static char *last_command = NULL;

void* udp_listener_thread(void* arg) {
    (void)arg; // Suppress unused parameter warning
    char buffer[BUFFER_SIZE];
    ssize_t received_len;

    while (1) {
        received_len = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (received_len < 0) {
            perror("Receive failed");
            continue;
        }
        if (received_len < BUFFER_SIZE) {
            buffer[received_len] = '\0';
            buffer[strcspn(buffer, "\r\n")] = '\0';  // Strip trailing newline or carriage return
        }

        if (strlen(buffer) == 0) {
            if (last_command == NULL) {
                sendto(sockfd, "Unknown command. Type 'help' for a list of commands.\n", 53, 0, (struct sockaddr*)&client_addr, addr_len);
                continue;
            }
            strncpy(buffer, last_command, sizeof(buffer) - 1); // Use the last valid command
            buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination
        } else {
            free(last_command);
            last_command = strdup(buffer);  // Store last valid command
        }

        printf("Received command: %s\n", buffer);

        if (strcmp(buffer, "help") == 0 || strcmp(buffer, "?") == 0) {
            sendto(sockfd, 
            "\nAvailable commands: \nhelp: list of commands and summary,\ncount: Return the total number of light samples take so far,\nlength: Return how many samples were captured during the previous second,\ndips: Return how many dips were detected during the previous second’s samples,\nhistory: Return all the data samples from the previous second,\nstop: Exit the program,\n<enter>: repeat last command\n",
                390,
                0, (const struct sockaddr *)&client_addr, addr_len);
                
        } else if (strcmp(buffer, "count") == 0) {
            char response[64];
            snprintf(response, sizeof(response), "# samples taken total: %lld\n", Sampler_getNumSamplesTaken());
            sendto(sockfd, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);

        } else if (strcmp(buffer, "length") == 0) {
            char response[64];
            snprintf(response, sizeof(response), "# samples taken last second: %d\n", Sampler_getHistorySize());
            sendto(sockfd, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);

        } else if (strcmp(buffer, "dips") == 0) {
            char response[64];
            snprintf(response, sizeof(response), "# Dips: %d\n", Sampler_getDipCount());
            sendto(sockfd, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);

        } else if (strcmp(buffer, "history") == 0) {
            int size = 0;
            double* history = Sampler_getHistory(&size);
            
            if (history) {
                char response[1500];  // Maximum packet size
                int offset = 0;
                
                for (int i = 0; i < size; i++) {
                    double voltage = (3.3 / 4096) * history[i];  // Convert ADC value to voltage (3.3V reference, 12-bit ADC)
                    int written = snprintf(response + offset, sizeof(response) - offset, "%.3f, ", voltage);
                    
                    if (written < 0 || (size_t)(offset + written) >= sizeof(response) - 10) {  //By stopping 10 bytes before the end, we ensure that at least one full formatted value fits in the current packet.
                        // Ensure we have room and no single value is split
                        sendto(sockfd, response, offset, 0, (struct sockaddr*)&client_addr, addr_len);
                        offset = 0;
                        written = snprintf(response, sizeof(response), "%.3f, ", voltage);
                    }
                    
                    offset += written;
                    
                    // After every 10 numbers, add a newline
                    if ((i + 1) % 10 == 0) {
                        response[offset - 1] = '\n';
                    }
                }
                
                if (offset > 0) {
                    // Replace last comma with newline
                    if (offset > 2 && response[offset - 2] == ',') {
                        response[offset - 2] = '\n';
                        offset--;  // Adjust length
                    }
                    sendto(sockfd, response, offset, 0, (struct sockaddr*)&client_addr, addr_len);
                }

                
                free(history);
            } else {
                sendto(sockfd, "No history available\n", 21, 0, (struct sockaddr*)&client_addr, addr_len);
            }
        } else if (strcmp(buffer, "stop") == 0) {
            sendto(sockfd, "Program terminating.\n", 21, 0, (struct sockaddr*)&client_addr, addr_len);
            close(sockfd);
            exit(0);

        } else {
            sendto(sockfd, "Unknown command. Type 'help' for a list of commands.\n", 53, 0, (struct sockaddr*)&client_addr, addr_len);
        }
    }
    return NULL;
}

void UdpListener_init(void) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_create(&udp_thread, NULL, udp_listener_thread, NULL);
}

void UdpListener_cleanup(void) {
    free(last_command);
    pthread_cancel(udp_thread);
    pthread_join(udp_thread, NULL);
    close(sockfd);
}
