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
#include "ADC/ADS1115.h"
#include "ADC/Pico_ADC.h"
#include "Multicore_fifo/multicore_fifo.h"


#define UDP_port 4444
#define SSID "PicoProbe"
#define server_IP "192.168.0.1"
#define password "my_secure_pass"


/// !!!! @todo -> HOW TO RESOLVE MULTI USER CASE !!!! /// 

typedef struct{
    uint16_t device_status;
    uint16_t *ADC_ADS1115BufferCh0;//[ADC_ADS1115SampleNumber];
    uint16_t *ADC_ADS1115BufferCh1;//[ADC_ADS1115SampleNumber];
    uint16_t *ADC_PicoBuffer;//[ADC_PicoSampleNumber];                  //Pi Pico embedded ADC data(remember PicoAdc = {Ch1, Ch2, Ch1, Ch2, ....})
    uint digital_analyzerBuffer[digital_analyzerMaxBufferSize]; //TO DO -> CHANGE BUFFER to PTR
}send_bufferFrame;

typedef struct{
    /// TODO ///
    uint32_t status;
}receive_bufferFrame;

/// @brief Pi Pico wifi module(CYW43439) initialization
/// @param  - --
void wifi_init(void);

/// @brief Pi Pico send data via WIFI
/// @param data - --
/// @param data_size - -- 
void wifi_sendData(const void *data, size_t data_size);

void print_Ip_Address(void);
#endif