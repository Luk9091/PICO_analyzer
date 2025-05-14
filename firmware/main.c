#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>


#include "analyze.h"
 
#include "dma.h"
#include "timer.h"
#include "led.h"
#include "communication.h"
#include "ADC/ADS1115.h"
#include "ADC/ADC_bootStrap.h"





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
    LED_init();
    communication_init();
    TIMER_init(TIMER_SLICE, 1000);
    ANALYZE_init();



    while(1){
        DMA_clear();
        communication_run();
        ANALYZE_enable(false);
    }
}



// int main(){
//     stdio_init_all();

//     ADC_bootStrap();
//     uint16_t *adc_data = NULL;
//     uint16_t data = 0;
//     uint64_t timer_1 = 0;
//     uint64_t timer_2 = 0; 
//     uint32_t counter = 0;

//     //adc_data = ADC_PicoDMAModeGetData();
//     sleep_ms(3000);

//     while(1)
//     {  
//         //timer_1 = time_us_64();
//         //adc_data = ADC_PicoDMAModeGetData();
//         //for(uint32_t i = 0; i < ADC_PicoSampleNumber; i++)
//         //{
//         //    //printf("ctr: %d, sample: %d\n",counter, adc_data[i]);
//         //    printf("%d\n", adc_data[i]);
//         //    //counter++;
//         ////sleep_us(10);
//         //}
//         //timer_2 = time_us_64();
//         //counter = 0;
//         //printf("********************\n time: %llu\n*********************\n", time_us_64() - timer);
//         //sleep_us(100000 - (timer_2 - timer_1));  
//         //printf("%d\n", adc_hw->fifo);
//         //sleep_ms(1);
//         //sleep_ms(60); // WIFI receive data simulation
//         //printf("********************\n time: %llu\n*********************\n", time_us_64() - timer);  
        
//         adc_data = ADS1115_ADCGetData(ADS1115_channel_0);
//         for(uint8_t i = 0; i < ADC_ADS1115SampleNumber; i++)
//             printf("%d\n", adc_data[i]);
//         sleep_ms(190);
//     }

// }