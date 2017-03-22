/*
 * Copyright (c) 2016 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "socket_example.h"

DigitalOut command_led(LED1);
NetworkInterface * network_interface;
Thread udp_thread;
static void udp_main();


void start_socket_example(NetworkInterface * interface)
{
    network_interface = interface;
    udp_thread.start(udp_main);
}

static void udp_main()
{
    uint8_t buffer[32];
    
    // Setup UDP socket
    UDPSocket sock(network_interface);
    sock.set_blocking(true);
    //sock.bind("ff02::1",1234);   //need to bind to address?
    sock.bind(1234);   //need to bind to address?
    
    while (true) {

        // Read data from the socket
        SocketAddress source_addr;
        memset(buffer, 0, sizeof(buffer));
        int length = sock.recvfrom(&source_addr, buffer, sizeof(buffer) - 1);
        if (length <= 0) {
            printf("Error with socket recvfrom: %i\n", length);
            continue;
        }
        printf("Packet from %s\n", source_addr.get_ip_address());

        // Send message back
        uint8_t * data = (uint8_t *)"Packet recieved\n";
        int ret = sock.sendto(source_addr, data, strlen((char*)data));
        if (ret <= 0) {
            printf("Error with socket sendto: %i\n", ret);
            continue;
        }

        // Handle command - "on", "off", "close" or "connect"
        if (strcmp((char*)buffer, "on") == 0) {
            printf("Turning led on\n");
            command_led = 0;
        }
        if (strcmp((char*)buffer, "off") == 0) {
            printf("Turning led off\n");
            command_led = 1;
        }

    }
}
