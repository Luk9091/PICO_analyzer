#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>


#include "analyze.h"
#include "dma.h"
#include "timer.h"
#include "led.h"
#include "communication.h"





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