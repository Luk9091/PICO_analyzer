#ifndef __wire_Communication_H__
#define __wire_Communication_H__

#include <pico/stdlib.h>
#include <hardware/pio.h>

#if LIB_PICO_STDIO_USB
#include <tusb.h>
#include <hardware/structs/usb.h>
#elif LIB_PICO_STDIO_UART
#include <pico/stdio_uart.h>
#endif



#if LIB_PICO_STDIO_USB
#define MESSAGE_MAX_LEN CFG_TUD_CDC_RX_BUFSIZE
#elif LIB_PICO_STDIO_UART
#define MESSAGE_MAX_LEN 256
#endif

#define NOWRITE_DELAY_MAX           1024
#define WIRE_COMMUNICATION_SPEED_TEST    false

#if LIB_PICO_STDIO_USB
static inline int print(const void *str, uint32_t size){
    return tud_cdc_write(str, size);
}
#elif LIB_PICO_STDIO_UART
static inline int print(const void *str, uint32_t size){
    for (uint i = 0; i < size; i++){
        uart_putc(uart0, ((char*)(str))[i]);
    }
    return size;
}
#endif

static inline int printTag(int32_t data){
    print(&data, 4);
}



void wireCommunication_run();
void wireCommunication_init();

static inline uint wireCommunication_read(const char *str);
void wireCommunication_sendWithTimeProcedure();
void wireCommunication_sendProcedure();


extern uint getMainFreq();


#if wireCommunication_SPEED_TEST
uint measureTime_print();
uint measureTime_printf();
#endif

#endif