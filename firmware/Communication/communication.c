#include "communication.h"

#include <stdio.h>
#include <string.h>
#include <hardware/irq.h>

#include <tusb.h>
#include <hardware/structs/usb.h>

#include "dma.h"
#include "led.h"

static bool _transferData = false;

typedef union{
    // uint64_t u64;
    uint32_t u32;
    struct{
        uint16_t half[2];
    };

    struct {
        uint8_t quoter[4];
    };
} convert_t;


void waitUntilOK(){
    bool wait = true;
    char readMsg[256];
    int index = 0;
    do{
        LED_toggle();
        sleep_ms(100);
    } while (!_transferData);
    LED_on();
}


void communication_run(uint dma_1, uint dma_2, uint *data){
    waitUntilOK();
    _transferData = true;
    DMA_setEnable(dma_1, true);
    gpio_put(ENABLE_GPIO, 1);

    communication_sendProcedure(dma_1, dma_2, data);
}




void tud_cdc_rx_cb(uint8_t itf){
    char readMsg[CFG_TUD_CDC_RX_BUFSIZE];

    if (communication_read(readMsg)){
        if(strncmp(readMsg, "OK", 2) == PICO_OK){
            _transferData = true;
        }
        else if(strncmp(readMsg, "DONE", 4) == PICO_OK){
            _transferData = false;
        }
        else if(strncmp(readMsg, "HELLO", 5) == PICO_OK){
            tud_cdc_write_clear();
            tud_cdc_write("Raspberry PI PICO\n\r", 20);
            // char freqBuff[32] = {0};
            // uint freq = getMainFreq(); 
            // sprintf(freqBuff, "Clock: %3iMHz\n\r", freq);
            // tud_cdc_write(freqBuff, strlen(freqBuff));
            tud_cdc_write_flush();
        }
    }
}


void communication_init(){
    stdio_usb_init();
}


uint communication_read(const char *str){
    return tud_cdc_read((void*)str, CFG_TUD_CDC_RX_BUFSIZE);
}


void communication_sendProcedure(uint dma_1, uint dma_2, uint *data){
    uint dma[2] = {dma_1, dma_2};
    uint32_t index = 0;
    uint32_t sampleIndex = 0;
    uint32_t nowriteDelay = 0;
    uint32_t dmaSel = 0;

    while (_transferData){
        sampleIndex = dma_getCurrentIndex(dma[dmaSel]);
        if (index != sampleIndex){
            uint sample = data[index];
            // printf("Index: %u\tdma: %u:% 4u\n", index, dmaSel, sampleIndex);
            tud_cdc_write(&sample, 2);               // store two byte on USB write buffer
            index++;
            if (index >= DATA_SIZE){
                index = 0;
                if(dmaSel == 1){
                    dmaSel = 0;
                } else {
                    dmaSel = 1;
                }
            }
            nowriteDelay = 0;
        } else{
            if (nowriteDelay >= NOWRITE_DELAY_MAX){
                tud_cdc_write_flush();                          // send buffer even is not full
                nowriteDelay = 0;
            }

            uint buffCapacity = tud_cdc_write_available();
            if (buffCapacity != CFG_TUD_CDC_TX_BUFSIZE){
                nowriteDelay++; // if the buffer is not empty, count cycles until unconditional send
            }

            // tud_cdc_read
        }
    }

    printf("STOP\n");
    tud_cdc_write_clear();
    LED_off();
}



// ##############################
// ###### SPEED TEST ############
// ##############################
#if COMMUNICATION_SPEED_TEST

#define SEND_SAMPLE 500
uint measureTime_tud(){
    uint start = time_us_32();
    for(uint i = 0; i < SEND_SAMPLE; i ++){
        tud_cdc_write("Hello\n\r", 8);
    }
    uint stop = time_us_32();

   return stop - start;
}

uint measureTime_printf(){
    uint start = time_us_32();
    for(uint i = 0; i < SEND_SAMPLE; i ++){
        printf("Hello\n\r");
    }
    uint stop = time_us_32();

   return stop - start;
}

uint measureTime_uartPutChar(){
    uint start = time_us_32();
    char str[] = "Hello\n\r";
    for (uint i = 0; i < SEND_SAMPLE; i++){
        for (uint j = 0; j < 8; j++){
            uart_putc_raw(UART_ID, str[j]);
        }
    }
    uint stop = time_us_32();
    return stop - start;
}


#endif