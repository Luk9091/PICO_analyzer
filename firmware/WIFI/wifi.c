#include "wifi.h"

static dhcp_server_t dhcp_server;

void wifi_init(void)
{
    cyw43_arch_init();
    cyw43_arch_enable_ap_mode("PiPicoNetwork", "password123", CYW43_AUTH_WPA2_AES_PSK);

    sleep_ms(1000);
    print_Ip_Address();

    ip4_addr_t ip, netmask;
    IP4_ADDR(&ip, 192, 168, 4, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);

    netif_set_addr(netif_default, &ip, &netmask, &ip); // gateway = ip
    dhcp_server_init(&dhcp_server, &ip, &netmask);
}

void print_Ip_Address(void)
{
    struct netif *netif = netif_list;
    while (netif != NULL) {
        if (netif_is_up(netif) && netif->ip_addr.addr != 0) {
            printf("IP address: %s\n", ip4addr_ntoa((ip4_addr_t *)&netif->ip_addr));
            return;
        }
        netif = netif->next;
    }
    printf("Unable to get IP address\n");
}