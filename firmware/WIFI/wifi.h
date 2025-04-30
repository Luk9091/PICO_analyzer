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


#define UDP_port 4444
#define SSID "PicoProbe"
#define server_IP "192.168.0.1"
#define password "PicoProbe"


void wifi_init(void);
#endif