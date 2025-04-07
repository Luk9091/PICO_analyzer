#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>


#include "dma.h"
#include "timer.h"
#include "led.h"
#include "communication.h"

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
volatile uint sampleData[DATA_SIZE] = {0};
volatile uint timeStamp[DATA_SIZE] = {0};

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
    gpio_init_mask(read_mask);
    gpio_init(TRIGGER_GPIO);


    gpio_set_dir_in_masked(read_mask);
    gpio_set_dir(TRIGGER_GPIO, false);


    uint sm;
    uint offset;
    pio_claim_free_sm_and_add_program_for_gpio_range(&triggered_read_program, &pio, &sm, &offset, LSB_GPIO, PIO_NUM_PIN, true);
    triggered_read_program_init(pio, sm, offset, LSB_GPIO, PIO_NUM_PIN, TRIGGER_GPIO);
    // pio_claim_free_sm_and_add_program_for_gpio_range(&continue_read_program, &pio, &sm, &offset, LSB_GPIO, PIO_NUM_PIN, true);
    // continue_read_program_init(pio, sm, offset, LSB_GPIO, PIO_NUM_PIN, 10 * kHz);




    LED_init();

    DMA_PIOconfig(
        sampleData,
        &pio->rxf[sm],
        pio_get_dreq(pio, sm, false),
        DMA_DATA_0, DMA_DATA_1
    );

    TIMER_init(TIMER_SLICE, 1000);
    DMA_TIMERconfig(
        timeStamp,
        &(pwm_hw->slice[TIMER_SLICE].ctr),
        pio_get_dreq(pio, sm, false),
        DMA_TIME
    );


    communication_init();



    while(1){
        communication_run(pio, sm);

        pio_sm_set_enabled(pio, sm, false);
        pio_sm_restart(pio, sm);
        DMA_clear();
    }

}