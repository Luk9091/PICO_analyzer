#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>
 
#include "dma.h"
#include "timer.h"
#include "led.h"
#include "communication.h"
#include "ADC/ADS1115.h"
#include "ADC/ADC_bootStrap.h"
#include "WIFI/wifi.h"
#include "Core1_bootstrap/core1.h"

#include "read.pio.h"
#include "read.h"


#define TRIGGER_GPIO    16

#define LSB_GPIO        0
#define PIO_NUM_PIN     16


uint read_mask =
    1 <<  0 | 1 <<  1 | 1 <<  2 | 1 <<  3 |
    1 <<  4 | 1 <<  5 | 1 <<  6 | 1 <<  7 |
    1 <<  8 | 1 << 9 | 1 << 10 | 1 << 11 |
    1 << 12 | 1 << 13 | 1 << 14 | 1 << 15;
volatile uint16_t sampleData[DATA_SIZE] = {0};
volatile uint16_t timeStamp[DATA_SIZE] = {0};

PIO pio;


inline uint getMainFreq(){
    uint freq = frequency_count_mhz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    return freq;
}

uint reverseBit(uint64_t data){
    uint reverseData = 0;
    for (int i = 0; i < 32; i++){
        if ((data & (1 << i)))
            reverseData |= 1 << ((32 - 1) - i);
    }
    return reverseData;
}


int main(){
    stdio_init_all();
    sleep_ms(1000);
    //gpio_init(25);
    //gpio_set_dir(25, GPIO_OUT);
    //cyw43_arch_init();

    core1_init();


    //ADC_bootStrap();
    uint16_t *adc_data_1 = NULL;
    uint16_t *adc_data_2 = NULL;


    while(1)
    {  
       adc_data_1 = ADC_PicoStandardModeGetData();
       adc_data_2 = ADS1115_ADCGetData(ADS1115_channel_0);
        // ################ TEST PI PICO ADC ####################
        //for(uint32_t i = 0; i < ADC_PicoSampleNumber; i++){
        //    printf("%d\n", adc_data_1[i]);
        //    sleep_ms(1);
        //} 
        // ######################################################


       // ################### TEST ADS1115 ######################
       //for(uint32_t i = 0; i < ADC_ADS1115SampleNumber; i++){
       //     printf("%d\n", adc_data_1[i]);
            //sleep_ms(1);
        //}

        //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        //sleep_ms(500);
        //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(500);
    }
}