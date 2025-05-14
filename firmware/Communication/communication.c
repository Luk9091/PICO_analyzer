#include "communication.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <hardware/irq.h>


#include "analyze.h"
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


void communication_run(){
    waitUntilRun();
    ANALYZE_enable(true);

    if (ANALYZE_timerIsOn()){
        communication_sendWithTimeProcedure();
    } else {
        communication_sendProcedure();
    }
    LED_off();
}




#if LIB_PICO_STDIO_USB
void tud_cdc_rx_cb(uint8_t itf)
#elif LIB_PICO_STDIO_UART
void UART_rx_cb()
#endif
{
    char readMsg[MESSAGE_MAX_LEN];
    
    if (communication_read(readMsg)){
        communication_valid(readMsg);
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


static inline char *nextToken(){
    return strtok(0, " \n");
}

static inline enum pico_error_codes communication_validator(char *line){
    char *token = strtok(line, " ");
    if(_transferData && strncmp(token, "DONE", 4) == PICO_OK){
        _transferData = false;
        print("\nSTOP\n", 6);
    }
    else if (_transferData){
        return PICO_ERROR_NOT_PERMITTED;
    }
    else if(strncmp(token, "RUN", 3) == PICO_OK){
        char *ok;
        token = nextToken();
        if (token == 0) return PICO_ERROR_INVALID_ARG;
        int timerFreq = strtol(token, &ok, 10);
        if (*ok != PICO_OK) return PICO_ERROR_INVALID_ARG;

        ANALYZE_triggeredInit(timerFreq);
        _transferData = true;
    }
    else if(strncmp(token, "CONTINUE", 8) == PICO_OK){
        char *ok;
        token = nextToken();
        if (token == 0) return PICO_ERROR_INVALID_ARG;
        int sampleFreq = strtol(token, &ok, 10);
        if (*ok != PICO_OK) return PICO_ERROR_INVALID_ARG;

        ANALYZE_continueInit(sampleFreq);
        _transferData = true;
    }
    else if(strncmp(token, "COUNT", 5) == PICO_OK){
        char *ok;
        int args[2] = {};
        for (uint i = 0; i < 2; i++){
            token = nextToken();
            if (token == 0) return PICO_ERROR_INVALID_ARG;
            args[i] = strtol(token, &ok, 10);
            if (*ok != PICO_OK){
                return PICO_ERROR_INVALID_ARG;
            }
        }

        ANALYZE_countInit(args[0], args[1]);
        _transferData = true;
        return PICO_OK;
    }
    else if(strncmp(token, "TRIGGER", 7) == 0){
        token = nextToken();
        if (token == 0) return PICO_ERROR_INVALID_ARG;
        char *ok;
        int triggerPin = strtol(token, &ok, 10);
        if (*ok != PICO_OK){
            return PICO_ERROR_INVALID_ARG;
        }

        ANALYZE_setTrigger(triggerPin);
        return PICO_OK;
    }
    else if(strncmp(token, "HELLO", 5) == PICO_OK){
        print("Raspberry PI PICO\n", 18);
        char freqBuff[32] = {0};
        uint freq = getMainFreq(); 
        
        sprintf(freqBuff, "Clock: %3iMHz\n", freq);
        print(freqBuff, strlen(freqBuff));
    }
    else {
        return PICO_ERROR_IO;
    }
    return PICO_OK;

}


enum pico_error_codes communication_valid(char *line){
    enum pico_error_codes error = communication_validator(line);

    switch (error)
    {
    case PICO_ERROR_NOT_PERMITTED:
    // No permitted send operation
    // Doesn't interrupt send stream
        break;
    case PICO_OK:
        print("OK\n", 3);
        break;
    case PICO_ERROR_INVALID_ARG:
        print("Invalid argument\n", 17);
        break;
    default:
        print("Syntax error\n", 13);
        break;
    }
    return error;
}




void communication_sendWithTimeProcedure(){
    uint32_t index = 0;
    uint32_t nowriteDelay = 0;
    uint32_t dmaSel = 0;

    while (_transferData){
        uint32_t dmaIndex = dma_getCurrentIndex(dmaSel);
        if (index != dmaIndex){
            // print((uint8_t*)(timeStamp  + index), 2);
            // print((uint8_t*)(sampleData + index), 2);
            printf("Index: %4u, dmaIndex: %4u, time: %4hu, data: 0x%X\n", index, dmaIndex, (uint16_t)(timeStamp[index]), (uint16_t)(sampleData[index]));
            index++;

            if (index >= DATA_SIZE){
                index = 0;
                if(dmaSel == 1){
                    dmaSel = DMA_DATA_0;
                } else {
                    dmaSel = DMA_DATA_1;
                }
            }
            nowriteDelay = 0;
        } 
#if LIB_PICO_STDIO_USB
        else {
            if (nowriteDelay >= NOWRITE_DELAY_MAX){
                tud_cdc_write_flush();                          // send buffer even is not full
                nowriteDelay = -1;
            }

            if (nowriteDelay < NOWRITE_DELAY_MAX){
                uint buffCapacity = tud_cdc_write_available();
                if (buffCapacity != CFG_TUD_CDC_TX_BUFSIZE){
                    nowriteDelay++; // if the buffer is not empty, count cycles until unconditional send
                }
            }
        }
#endif
    }

#if LIB_PICO_STDIO_USB
    tud_cdc_write_clear();
#endif
    print("\0\n\0", 4);
#if LIB_PICO_STDIO_USB
    tud_cdc_write_flush();
#endif
}

void communication_sendProcedure(){
    uint32_t index = 0;
    uint32_t nowriteDelay = 0;
    uint32_t dmaSel = 0;

    while (_transferData){
        uint32_t dmaIndex = dma_getCurrentIndex(dmaSel);
        if (index != dmaIndex){
            // print((uint8_t*)(sampleData + index), 2);
            printf("Index: %4i, dmaIndex: %4i, data: 0x%X\n", index, dmaIndex, (uint16_t)(sampleData[index]));
            index++;

            if (index >= (DATA_SIZE*2)){
                index = 0;
                if(dmaSel == 1){
                    dmaSel = DMA_DATA_0;
                } else {
                    dmaSel = DMA_DATA_1;
                }
            }
            nowriteDelay = 0;
        } 
#if LIB_PICO_STDIO_USB
        else {
            if (nowriteDelay == NOWRITE_DELAY_MAX){
                tud_cdc_write_flush();                          // send buffer even is not full
                nowriteDelay = -1;
            }

            if (nowriteDelay < NOWRITE_DELAY_MAX){
                uint buffCapacity = tud_cdc_write_available();
                if (buffCapacity != CFG_TUD_CDC_TX_BUFSIZE){
                    nowriteDelay++; // if the buffer is not empty, count cycles until unconditional send
                }
            }
        }
#endif
    }

#if LIB_PICO_STDIO_USB
    tud_cdc_write_clear();
#endif
    print("\0\n\0", 4);
#if LIB_PICO_STDIO_USB
    tud_cdc_write_flush();
#endif
}



// ##############################
// ###### SPEED TEST ############
// ##############################
#if COMMUNICATION_SPEED_TEST

#define SEND_SAMPLE 500
uint measureTime_print(){
    uint start = time_us_32();
    for(uint i = 0; i < SEND_SAMPLE; i ++){
        print("Hello, world\n", 13);
    }
    uint stop = time_us_32();

   return stop - start;
}

uint measureTime_printf(){
    uint start = time_us_32();
    for(uint i = 0; i < SEND_SAMPLE; i ++){
        printf("Hello, world\n");
    }
    uint stop = time_us_32();

   return stop - start;
}
#endif