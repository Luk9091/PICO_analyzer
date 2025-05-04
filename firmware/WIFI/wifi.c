#include "wifi.h"

static void wifi_receiveCallback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
static void wifi_receiveInit(void);

static dhcp_server_t dhcp_server;
static struct udp_pcb *send_pcb = NULL;
static struct udp_pcb *receive_pcb = NULL;
static ip_addr_t receiver_ipAddress = {0};

static bool wifi_irq(struct repeating_timer *t)
{
    static uint32_t n = 0;
    send_bufferFrame buffer_t = {n};
    wifi_sendData(&buffer_t);  
    n++;

    return true;
}


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

    /// temporary test
    static repeating_timer_t wifi_timer; 
    add_repeating_timer_ms(-100, wifi_irq, NULL, &wifi_timer);

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
        receive_bufferFrame received_data;
        memcpy(&received_data, p->payload, sizeof(receive_bufferFrame));
        //queue_try_add(&queue_Server_To_Pico, &received_data);
            printf("data received !!!\n");
            printf("Received data from IP: %s, Port: %d\n", ip4addr_ntoa(addr), port);
            printf("Data: %c\n", p->payload);
        pbuf_free(p);        
    }
}

void wifi_sendData(const send_bufferFrame* data_frame)
{
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(send_bufferFrame), PBUF_RAM);
    dhcp_getUserIP(&receiver_ipAddress);

    if(p != NULL)
    {
        memcpy(p->payload, data_frame, sizeof(send_bufferFrame));
        err_t err = udp_sendto(send_pcb, p, &receiver_ipAddress, UDP_port);
        if (err != ERR_OK) 
            printf("Failed to send UDP packet\n");
        pbuf_free(p);
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