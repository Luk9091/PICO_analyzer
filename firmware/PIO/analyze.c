#include "analyze.h"

#include "dma.h"
#include "timer.h"

#include "read.pio.h"
#include "read.h"


static PIO pio;
static uint sm;
static uint offset;


static bool timer;





static inline void DMA_data_config(bool priority){
    DMA_PIOconfig(
        sampleData,
        &pio->rxf[sm],
        pio_get_dreq(pio, sm, false),
        DMA_DATA_0, DMA_DATA_1,
        priority, priority
    );
}

static inline void DMA_timer_config(){
    DMA_PIOconfig(
        timeStamp,
        &(pwm_hw->slice[TIMER_SLICE].ctr),
        pio_get_dreq(pio, sm, false),
        DMA_TIME_0, DMA_TIME_1,
        1, 0
    );
}



void ANALYZE_init(){

    // uint read_mask =
    //     1 <<  0 | 1 <<  1 | 1 <<  2 | 1 <<  3 |
    //     1 <<  4 | 1 <<  5 | 1 <<  6 | 1 <<  7 |
    //     1 <<  8 | 1 <<  9 | 1 << 10 | 1 << 11 |
    //     1 << 12 | 1 << 13 | 1 << 14 | 1 << 15
    // ;

    uint read_mask = 0;
    for (uint i = LSB_GPIO; i < LSB_GPIO + PIO_NUM_PIN; i++){
        read_mask |= 1 << i;
    }


    gpio_init_mask(read_mask);
    gpio_init(TRIGGER_GPIO);


    gpio_set_dir_in_masked(read_mask);
    gpio_set_dir(TRIGGER_GPIO, false);

    // triggerPin = TRIGGER_GPIO;
    // sampleFreq = 100*MHz;
    // countSamples = DATA_SIZE;
    // timer = false;

    ANALYZE_triggeredInit(1, TRIGGER_GPIO);

}

void ANALYZE_enable(bool run){
    DMA_clear();
    if (timer){
        DMA_setEnable(DMA_TIME_0, run);
    } else {
        DMA_setEnable(DMA_TIME_0, false);
    }
    DMA_setEnable(DMA_DATA_0, run);
    pio_sm_set_enabled(pio, sm, run);
}

bool ANALYZE_timerIsOn(){
    return timer;
}


void ANALYZE_triggeredInit(uint timerFreq, uint triggerPin){
    ANALYZE_deinit();
    pio_claim_free_sm_and_add_program_for_gpio_range(&triggered_read_program, &pio, &sm, &offset, LSB_GPIO, PIO_NUM_PIN, true);
    triggered_read_program_init(pio, sm, offset, LSB_GPIO, PIO_NUM_PIN, triggerPin);

    if (timerFreq){
        timer = true;
        DMA_data_config(0);
        TIMER_setFreq(TIMER_SLICE, timerFreq);
        DMA_timer_config();
    } else {
        timer = false;
        DMA_data_config(1);
    }
}


void ANALYZE_continueInit(uint sampleFreq){
    timer = false;
    ANALYZE_deinit();
    pio_claim_free_sm_and_add_program_for_gpio_range(&continue_read_program, &pio, &sm, &offset, LSB_GPIO, PIO_NUM_PIN, true);
    continue_read_program_init(pio, sm, offset, LSB_GPIO, PIO_NUM_PIN, sampleFreq);
    
    DMA_data_config(1);
}


void ANALYZE_countInit(uint countSample, uint sampleFreq, uint triggerPin){
    timer = false;
    ANALYZE_deinit();
    pio_claim_free_sm_and_add_program_for_gpio_range(&count_read_program, &pio, &sm, &offset, LSB_GPIO, PIO_NUM_PIN, true);
    count_read_program_init(pio, sm, offset, LSB_GPIO, PIO_NUM_PIN, triggerPin, sampleFreq);

    pio_sm_put_blocking(pio, sm, countSample);
    DMA_data_config(1);
}


void ANALYZE_deinit(){
    DMA_clear();

    switch (ANALYZE_MODE)
    {
        case TRIGGERED_ANALYZE:{
            pio_remove_program_and_unclaim_sm(&triggered_read_program, pio, sm, offset);
        } break;

        case FREE_RUN_ANALYZE:{
            pio_remove_program_and_unclaim_sm(&continue_read_program, pio, sm, offset);
        } break;

        case COUNT_ANALYZE:{
            pio_remove_program_and_unclaim_sm(&count_read_program, pio, sm, offset);
        } break;
    
        default:
        break;
    }
}