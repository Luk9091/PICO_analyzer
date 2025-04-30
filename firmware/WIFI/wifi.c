#include "wifi.h"

void wifi_init(void)
{
    cyw43_arch_init();
    cyw43_arch_enable_ap_mode(SSID, password, CYW43_AUTH_WPA2_AES_PSK);

    
    
}