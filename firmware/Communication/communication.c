#include "communication.h"

#include <stdio.h>
#include <string.h>
#include <hardware/irq.h>


#include "dma.h"
#include "led.h"

static bool _transferData = false;

void waitUntilRun(){
    do{
        LED_toggle();
        sleep_ms(100);
    } while (!_transferData);
    LED_on();
}


void communication_run(PIO pio, uint sm){
    waitUntilRun();
    DMA_setEnable(DMA_DATA_0, true);
    DMA_setEnable(DMA_TIME_0, true);
    pio_sm_set_enabled(pio, sm, true);

    communication_sendProcedure();
}




#if LIB_PICO_STDIO_USB
void tud_cdc_rx_cb(uint8_t itf)
#elif LIB_PICO_STDIO_UART
void UART_rx_cb()
#endif
{
    char readMsg[MESSAGE_MAX_LEN];
    
    if (communication_read(readMsg)){
        if(_transferData && strncmp(readMsg, "DONE", 1) == PICO_OK){
            _transferData = false;
            print("\n\rSTOP\n\r", 8);
            print("OK\n\r", 4);
        }
        else if (_transferData){
            return;
        }
        else if(strncmp(readMsg, "RUN", 1) == PICO_OK){
            _transferData = true;
            print("OK\n\r", 4);
        }
        else if(strncmp(readMsg, "HELLO", 1) == PICO_OK){
            print("Raspberry PI PICO\n\r", 19);
            char freqBuff[32] = {0};
            uint freq = getMainFreq(); 
            sprintf(freqBuff, "Clock: %3iMHz\n\r", freq);
            print(freqBuff, strlen(freqBuff));
            print("OK\n\r", 4);
        }
        else {
            print("Syntax error\n\r", 14);
        }
#if LIB_PICO_STDIO_USB
        tud_cdc_write_flush();
#endif
    }
}

#if LIB_PICO_STDIO_USB
uint communication_read(const char *str){
    return tud_cdc_read((void*)str, CFG_TUD_CDC_RX_BUFSIZE);
}
#elif LIB_PICO_STDIO_UART
uint communication_read(const char *str){
    static char readLine[MESSAGE_MAX_LEN + 1];
    static int index;
    char c = uart_getc(uart0);
    uart_putc(uart0, c);

    if (c == '\r' || index == MESSAGE_MAX_LEN){
        uart_putc(uart0, '\n');
        int tmp_index = index;
        readLine[index + 1] = '\x00';
        memcpy((void*)str, (void*)readLine, index);
        index = 0;
        return tmp_index;
    }

    if (c == '\x7F'){
        index--;
        uart_putc(uart0, '\x08');
        uart_putc(uart0, ' ');
        uart_putc(uart0, '\x08');
        return 0;
    }

    readLine[index] = c;
    index++;
    return 0;
}
#endif


void communication_init(){
#ifdef LIB_PICO_STDIO_USB
    stdio_usb_init();
#elif defined LIB_PICO_STDIO_UART
    stdio_uart_init();
    irq_set_exclusive_handler(UART0_IRQ, UART_rx_cb);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);
#endif
}




void communication_sendProcedure(){
    uint32_t index = 0;
    uint32_t sampleIndex = 0;
    uint32_t nowriteDelay = 0;
    uint32_t dmaSel = 0;
    extern uint16_t sampleData[DATA_SIZE];
    extern uint16_t timeStamp[DATA_SIZE];

    while (_transferData){
        sampleIndex = dma_getCurrentIndex(dmaSel);
        if (index != sampleIndex){
            print((uint8_t*)(timeStamp + index), 2);
            print((uint8_t*)(sampleData + index), 2);
            // printf("%4i\tData: %5hu, 0x%4X, run: %i\n\r", index, timeStamp[index], sampleData[index], sampleIndex);
            // printf("%i,%i,%i\n", index, timeStamp[index], sampleData[index]);
            index++;
            if (index >= DATA_SIZE){
                index = 0;
                if(dmaSel == 1){
                    dmaSel = DMA_DATA_0;
                } else {
                    dmaSel = DMA_DATA_1;
                    _transferData = false;
                }
            }
            nowriteDelay = 0;
        } 
#if LIB_PICO_STDIO_USB
        else {
            if (nowriteDelay >= NOWRITE_DELAY_MAX){
                tud_cdc_write_flush();                          // send buffer even is not full
                nowriteDelay = 0;
            }

            uint buffCapacity = tud_cdc_write_available();
            if (buffCapacity != CFG_TUD_CDC_TX_BUFSIZE){
                nowriteDelay++; // if the buffer is not empty, count cycles until unconditional send
            }
        }
#endif
    }

#if LIB_PICO_STDIO_USB
    tud_cdc_write_clear();
    print("\n\r\0", 3);
    tud_cdc_write_flush();
#endif
    LED_off();
}



// ##############################
// ###### SPEED TEST ############
// ##############################
#if COMMUNICATION_SPEED_TEST

#define SEND_SAMPLE 500
uint measureTime_print(){
    uint start = time_us_32();
    for(uint i = 0; i < SEND_SAMPLE; i ++){
        print("Hello, world\n\r", 14);
    }
    uint stop = time_us_32();

   return stop - start;
}

uint measureTime_printf(){
    uint start = time_us_32();
    for(uint i = 0; i < SEND_SAMPLE; i ++){
        printf("Hello, world\n\r");
    }
    uint stop = time_us_32();

   return stop - start;
}
#endif