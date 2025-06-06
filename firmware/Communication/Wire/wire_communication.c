#include "wire_communication.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <hardware/irq.h>


#include "analyze.h"
#include "dma.h"
#include "led.h"

#include "communication_config.h"

// static inline enum pico_error_codes wireCommunication_validator(char *line);
static inline enum pico_error_codes wireCommunication_valid(char *readMsg);

static bool transferData = false;

void waitUntilRun(){
    do{
        LED_toggle();
        sleep_ms(100);
    } while (!transferData);
    LED_on();
}


void wireCommunication_run(){
    waitUntilRun();
    ANALYZE_enable(true);

    if (ANALYZE_timerIsOn()){
        wireCommunication_sendWithTimeProcedure();
    } else {
        wireCommunication_sendProcedure();
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
    
    if (wireCommunication_read(readMsg)){
        wireCommunication_valid(readMsg);
    }
}

#if LIB_PICO_STDIO_USB
uint wireCommunication_read(const char *str){
    return tud_cdc_read((void*)str, CFG_TUD_CDC_RX_BUFSIZE);
}
#elif LIB_PICO_STDIO_UART
uint wireCommunication_read(const char *str){
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


void wireCommunication_init(){
#ifdef LIB_PICO_STDIO_USB
    stdio_usb_init();
#elif defined LIB_PICO_STDIO_UART
    stdio_uart_init();
    irq_set_exclusive_handler(UART0_IRQ, UART_rx_cb);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);
#endif
}


static inline int32_t nextToken(char *line, uint *index){
    int32_t data = 
        line[(*index)*4 + 0] << 24 | line[(*index)*4 + 1] << 16 |
        line[(*index)*4 + 2] << 8  | line[(*index)*4 + 3] << 0;
    (*index)++;
    return data;
}
static inline uint16_t nextTokenList(char *line, uint *tokens, uint count, uint *index){
    for (uint i = 0; i < 2; i++){
        uint token = nextToken(line, index);
        tokens[i] = token;
    }
}

static inline enum pico_error_codes wireCommunication_validator(char *line){
    int index = 0;
    int token = nextToken(line, &index);

    if(transferData && token == CMD_DEVICE_STOP){
#if LIB_PICO_STDIO_USB
        tud_cdc_write_flush();
#endif
        transferData = false;
    }
    else if (transferData){
        return PICO_ERROR_NOT_PERMITTED;
    }
    else {
        switch (token)
        {
        case CMD_DEVICE_RUN:{
            transferData = true;
        } break;

        case CMD_DIGITAL_MODE_TRIGGERED:{
            int args[2];
            nextTokenList(line, args, 2, &index);
            ANALYZE_triggeredInit(args[0], args[1]);
        } break;

        case CMD_DIGITAL_MODE_FREERUN:{
            token = nextToken(line, &index);
            ANALYZE_continueInit(token);
        } break;

        case CMD_DIGITAL_MODE_COUNT:{
            int args[3];
            nextTokenList(line, args, 3, &index);
            ANALYZE_countInit(args[0], args[1], args[2]);
        }

        // case CMD_DIGITAL_SET_TRIGGER_PIN:{
        // // else if(strncmp(token, "TRIGGER", 7) == 0){
        //     token = nextToken(line, &index);
        //     ANALYZE_setTrigger(token);
        //     return PICO_OK;
        // }

        case CMD_PICO_HELLO:{
            print("Raspberry PI PICO\n", 18);
            char freqBuff[32] = {0};
            uint freq = getMainFreq(); 
            
            sprintf(freqBuff, "Clock: %3iMHz\n", freq);
            print(freqBuff, strlen(freqBuff));
        }
            
        default:
            return PICO_ERROR_IO;
        }
    }

    return PICO_OK;

}


enum pico_error_codes wireCommunication_valid(char *line){
    enum pico_error_codes error = wireCommunication_validator(line);

        
    print(&error, 4);
#if LIB_PICO_STDIO_USB
    tud_cdc_write_flush();
#endif
    return error;
}




void wireCommunication_sendWithTimeProcedure(){
    uint32_t index = 0;
    uint32_t nowriteDelay = 0;
    uint32_t dmaSel = 0;
    uint32_t count = MESSAGE_MAX_LEN;

    while (transferData){
        uint32_t dmaIndex = dma_getCurrentIndex(dmaSel);
        if (count >= MESSAGE_MAX_LEN-4){
            printTag(TAG_DIGITAL_TIMER);
            count = 0;
        }

        if (index != dmaIndex){
            count += print((uint8_t*)(timeStamp  + index), 2);
            count += print((uint8_t*)(sampleData + index), 2);
            // printf("Index: %4u, dmaIndex: %4u, time: %4hu, data: 0x%X\n", index, dmaIndex, (uint16_t)(timeStamp[index]), (uint16_t)(sampleData[index]));
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
                count = MESSAGE_MAX_LEN;
                nowriteDelay = -1;
            }

            if (nowriteDelay < NOWRITE_DELAY_MAX && count > 0){
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

static inline int addHeader(send_dataTag_t mode){
    uint16_t padding = 0;
    print((uint8_t*)mode, 4);
    return 4;
}

void wireCommunication_sendProcedure(){
    uint32_t index = 0;
    uint32_t nowriteDelay = 0;
    uint32_t dmaSel = 0;
    uint32_t count = MESSAGE_MAX_LEN;

    while (transferData){
        uint32_t dmaIndex = dma_getCurrentIndex(dmaSel);
        if (count >= MESSAGE_MAX_LEN-4){
            // addHeader(mode);
            printTag(TAG_DIGITAL);
            count = 0;
        }

        if (index != dmaIndex){
            count += print((uint8_t*)(sampleData + index), 2);
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
                count = MESSAGE_MAX_LEN;
                nowriteDelay = -1;
            }

            if (nowriteDelay < NOWRITE_DELAY_MAX && count > 0){
                uint buffCapacity = tud_cdc_write_available();
                if (buffCapacity != CFG_TUD_CDC_TX_BUFSIZE){
                    nowriteDelay++; // if the buffer is not empty, count cycles until unconditional send
                }
            }
        }
#endif
    }

// #if LIB_PICO_STDIO_USB
//     tud_cdc_write_clear();
// #endif
//     print("\0\n\0", 4);
// #if LIB_PICO_STDIO_USB
//     tud_cdc_write_flush();
// #endif
}



// ##############################
// ###### SPEED TEST ############
// ##############################
#if wireCommunication_SPEED_TEST

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