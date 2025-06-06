#include "dma.h"
#include <math.h>
#include <hardware/irq.h>
#include <hardware/clocks.h>
#include <hardware/adc.h>

#include "util.h"
//#include "ADC/ADS1115.h"
//#include "ADC/Pico_ADC.h"

static uint16_t dataArray[DATA_SIZE * 2] = {0};
uint16_t *sampleData = dataArray;
uint16_t *timeStamp  = dataArray + (DATA_SIZE);

static uint dataSize;

static inline bool dma_config(const void *writeAddress, const void *readAddress, uint dreq, uint dma, dma_channel_config *config, bool priority, bool doubleSize){
    channel_config_set_transfer_data_size(config, DMA_SIZE_16);
    channel_config_set_read_increment(config, false);
    channel_config_set_write_increment(config, true);
    channel_config_set_dreq(config, dreq);
    channel_config_set_high_priority(config, priority);

    dataSize = DATA_SIZE;
    if (doubleSize){
        dataSize = dataSize * 2;
    }

    dma_channel_configure(dma,
        config,
        writeAddress,
        readAddress,
        dataSize,
        false
    );
    return true;
}

void dma_1_handler(){
    if (dma_hw->ints0 & (1 << DMA_DATA_0)){
        dma_hw->ints0 = 1 << DMA_DATA_0;
        dma_channel_set_write_addr(DMA_DATA_0, sampleData, false);
        dma_channel_set_trans_count(DMA_DATA_0, dataSize, false);
    }
    if (dma_hw->ints0 & (1 << DMA_DATA_1)){
        dma_hw->ints0 = 1 << DMA_DATA_1;
        dma_channel_set_write_addr(DMA_DATA_1, sampleData, false);
        dma_channel_set_trans_count(DMA_DATA_1, dataSize, false);
    }
    
    if (dma_hw->ints0 & (1 << DMA_TIME_0)){
        dma_hw->ints0 = 1 << DMA_TIME_0;
        dma_channel_set_write_addr(DMA_TIME_0, timeStamp, false);
        dma_channel_set_trans_count(DMA_TIME_0, dataSize, false);
    }
    if (dma_hw->ints0 & (1 << DMA_TIME_1)){
        dma_hw->ints0 = 1 << DMA_TIME_1;
        dma_channel_set_write_addr(DMA_TIME_1, timeStamp, false);
        dma_channel_set_trans_count(DMA_TIME_1, dataSize, false);
    }
}

// void dma_2_handler(){
//     dma_hw->ints1 = 1 << DMA_DATA_1;
//     dma_channel_set_write_addr(DMA_DATA_1, sampleData, false);
//     dma_channel_set_trans_count(DMA_DATA_1, DATA_SIZE, false);
// }

bool DMA_clear(){
    // Clear write address DMA 1
    dma_channel_set_write_addr(DMA_DATA_0, sampleData, true);
    dma_channel_set_trans_count(DMA_DATA_0, dataSize, true);

    // Clear write address DMA 2
    dma_channel_set_write_addr(DMA_DATA_1, sampleData, true);
    dma_channel_set_trans_count(DMA_DATA_1, dataSize, true);
    

    // Clear time address DMA
    dma_channel_set_write_addr(DMA_TIME_0, timeStamp, true);
    dma_channel_set_trans_count(DMA_TIME_0, dataSize, true);

    dma_channel_set_write_addr(DMA_TIME_1, timeStamp, true);
    dma_channel_set_trans_count(DMA_TIME_1, dataSize, true);

    dma_channel_abort(DMA_DATA_0);
    dma_channel_abort(DMA_DATA_1);
    dma_channel_abort(DMA_TIME_0);
    dma_channel_abort(DMA_TIME_1);

    return true;
}


int DMA_PIOconfig(const void *writeAddress, const void *readAddress, uint dreq, uint dma_1, uint dma_2, bool priority, bool doubleSize){
    if (!dma_channel_is_claimed(dma_1)){
        dma_channel_claim(dma_1);
        dma_channel_claim(dma_2);
    }


    dma_channel_config config_1 = dma_channel_get_default_config(dma_1);
    dma_channel_config config_2 = dma_channel_get_default_config(dma_2);
    channel_config_set_chain_to(&config_1, dma_2);
    channel_config_set_chain_to(&config_2, dma_1);

    dma_channel_set_irq0_enabled(dma_1, true);
    dma_channel_set_irq0_enabled(dma_2, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_1_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_config(writeAddress, readAddress, dreq, dma_1, &config_1, priority, doubleSize);
    dma_config(writeAddress, readAddress, dreq, dma_2, &config_2, priority, doubleSize);

    return 0;
}

void DMA_chain(uint dma_1, uint dma_2, bool chain){
    dma_channel_config config_1 = dma_get_channel_config(dma_1);
    dma_channel_config config_2 = dma_get_channel_config(dma_2);

    if (chain){
        channel_config_set_chain_to(&config_1, dma_2);
        channel_config_set_chain_to(&config_2, dma_1);
    } 
    else {
        channel_config_set_chain_to(&config_1, dma_1);
        channel_config_set_chain_to(&config_2, dma_2);
    }
}

void DMA_setEnable(uint dmaChannel, bool enabled){
    dma_channel_config config = dma_get_channel_config(dmaChannel);
    dma_channel_set_config(dmaChannel, &config, enabled);
}

uint dma_getCurrentIndex(uint dmaChannel){
    dma_channel_hw_t *channel = dma_channel_hw_addr(dmaChannel);
    uint transfer_count = channel->al3_transfer_count;
    return dataSize - transfer_count;
}
