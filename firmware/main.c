#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>


#include "dma.h"
#include "led.h"
#include "communication.h"

#include "read.pio.h"


#define TRIGGER_GPIO    16

#define LSB_GPIO        0
#define PIO_NUM_PIN     16


uint read_mask =
    1 <<  0 | 1 <<  1 | 1 <<  2 | 1 <<  3 |
    1 <<  4 | 1 <<  5 | 1 <<  6 | 1 <<  7 |
    1 <<  8 | 1 << 9 | 1 << 10 | 1 << 11 |
    1 << 12 | 1 << 13 | 1 << 14 | 1 << 15;
uint sampleData[DATA_SIZE] = {0};

PIO pio;
uint sm;
uint offset;


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
    gpio_init_mask(read_mask);
    gpio_init(ENABLE_GPIO);
    gpio_init(TRIGGER_GPIO);


    gpio_set_dir_in_masked(read_mask);
    gpio_set_dir(ENABLE_GPIO, true);
    gpio_set_dir(TRIGGER_GPIO, false);


    pio_claim_free_sm_and_add_program_for_gpio_range(&read_gpio_program, &pio, &sm, &offset, LSB_GPIO, PIO_NUM_PIN, true);
    read_gpio_program_init(pio, sm, offset, LSB_GPIO, PIO_NUM_PIN, TRIGGER_GPIO);



    LED_init();
    communication_init();


    uint dma_1, dma_2;
    DMA_PIOconfig(
        sampleData,
        &pio->rxf[sm],
        pio_get_dreq(pio, sm, false),
        &dma_1, &dma_2
    );

    while(1){
        DMA_clear();
        communication_run(dma_1, dma_2, sampleData);
        DMA_setEnable(dma_1, false);
        DMA_setEnable(dma_2, false);
    }

}