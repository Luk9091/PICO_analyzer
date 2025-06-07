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
#include "wire_communication.h"
#include "ADS1115.h"
#include "ADC_bootStrap.h"
#include "wifi.h"
#include "core1.h"


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


int main()
{
    stdio_init_all();
    core1_init();


    wireCommunication_init();
    TIMER_init(TIMER_SLICE, 1000);
    ANALYZE_init();
    
    
    
    while(1){
        DMA_clear();
        wireCommunication_run();
        ANALYZE_enable(false);
    }
    //while(1)
    //{
    //    sleep_ms(1000);
    //}
}

