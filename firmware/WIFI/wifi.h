#ifndef _WIFI_
#define _WIFI_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lwip/init.h"        
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "pico/cyw43_arch.h"
#include "pico/util/queue.h"
#include "dhcpserver.h"
#include "hardware/timer.h"


#define UDP_port 4444
#define SSID "PicoProbe"
#define server_IP "192.168.0.1"
#define password "my_secure_pass"


/// !!!! @todo -> HOW TO RESOLVE MULTI USER CASE !!!! /// 

typedef struct{
    uint32_t device_status;         // Device status flag, default: status = 0
    uint16_t *ADC_picoBuffer;       // Pi Pico embedded ADC data(remember PicoAdc = {Ch1, Ch2, Ch1, Ch2, ....})
    uint16_t *ADC_ADS1115BufferCh1; // ADS1115 ADC data channel 1
    uint16_t *ADC_ADS1115BufferCh2; // ADS1115 ADC data channel 2

    /// TODO ///
    /// DATA FROM LOGIC ANALYZER ///
}send_bufferFrame;

typedef struct{
    /// TODO ///
    uint32_t status;
}receive_bufferFrame;

/// @brief Pi Pico wifi module(CYW43439) initialization
/// @param  - --
void wifi_init(void);

/// @brief Pi Pico send data via WIFI
/// @param data_frame dato to send
void wifi_sendData(const send_bufferFrame* data_frame);

void print_Ip_Address(void);
#endif