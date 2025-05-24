#include "wifi.h"

static void wifi_receiveCallback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
static void wifi_receiveInit(void);

static dhcp_server_t dhcp_server;
static struct udp_pcb *send_pcb = NULL;
static struct udp_pcb *receive_pcb = NULL;
static ip_addr_t receiver_ipAddress = {0};
static device_configStatus_t received_data;

void wifi_init(void)
{
    cyw43_arch_init();
    cyw43_arch_enable_ap_mode(SSID, password, CYW43_AUTH_OPEN);
    sleep_ms(1000);

    ip4_addr_t ip, netmask;
    IP4_ADDR(&ip, 192, 168, 4, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);

    netif_set_addr(netif_default, &ip, &netmask, &ip); 
    dhcp_server_init(&dhcp_server, &ip, &netmask);

    wifi_receiveInit();

    send_pcb = udp_new();
}

static void wifi_receiveInit(void)
{
    if(receive_pcb == NULL)
    {
        receive_pcb = udp_new();
        if(receive_pcb == NULL)
        {
            printf("Failed to create UDP PCB\n");
            return;
        }

        if (receive_pcb != NULL)
        {
            err_t err = udp_bind(receive_pcb, IP_ADDR_ANY, UDP_port); 
            if (err == ERR_OK) 
                udp_recv(receive_pcb, wifi_receiveCallback, NULL);
            else 
                printf("Failed to bind UDP PCB\n");
        }
        else 
            printf("Failed to create UDP PCB\n");
        }
}

static void wifi_receiveCallback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p != NULL)
    {
        memcpy(&received_data, p->payload, sizeof(device_configStatus_t));
        multicore_fifoTryPushCore1(FIFO_FRAME_CONFIG, &received_data, NULL); 
        // printf("data received !!!\n");
        // printf("Received data from IP: %s, Port: %d\n", ip4addr_ntoa(addr), port);
        // printf("Data: %c\n", p->payload
        pbuf_free(p);        
    }
}

void wifi_sendData(const uint16_t *data, send_dataTag_t tag, size_t data_sizeByte)
{
    int16_t tag_16 = (int16_t)tag;

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, data_sizeByte + sizeof(tag_16), PBUF_RAM);
    dhcp_getUserIP(&receiver_ipAddress);

    if (p != NULL)
    {
        uint16_t *payload = (uint16_t*)p->payload;
        payload[0] = tag_16; //memcpy(payload, &tag_16, sizeof(tag_16));
        memcpy(payload + 1, data, data_sizeByte);
        err_t err = udp_sendto(send_pcb, p, &receiver_ipAddress, UDP_port);
        if (err != ERR_OK) 
            printf("Failed to send UDP packet !!!\n");

        pbuf_free(p);
    }
    else
    {
        printf("Failed to allocate pbuf!\n");
    }
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