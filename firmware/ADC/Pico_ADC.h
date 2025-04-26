#ifndef _PICO_ADC_
#define _PICO_ADC_

#include <assert.h>
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "DMA/dma.h"
#include "../ring_buffer/ring_buffer.h"

#define ADC_PicoChannel_0 0    // ADC 0 Input Pin  <-> ADC 0 channel
#define ADC_PicoChannel_1 1    // ADC 1 Input Pin  <-> ADC 1 channel
#define ADC_PicoChannel_2 2    // ADC 2 Input Pin  <-> ADC 2 channel

#define ADC_PicoSampleNumber 1000


typedef struct{
    uint16_t current_buffer;
    uint16_t DMA_buffer_1[2 * ADC_PicoSampleNumber];
    uint16_t DMA_buffer_2[2 * ADC_PicoSampleNumber];
    int DMA_channel;
    dma_channel_config DMA_cfg;
}DMA_state;

/// @brief standard mode including only double buffering <-> require timer irq pooling 
typedef struct{
    uint16_t    channel_number;        // channel number
    ring_buffer *current_buffer;    // current buffer where new samples are saved
    ring_buffer *buffer_1;         
    ring_buffer *buffer_2;
}Pico_adcStandardMode;

/// @brief - initialize Pi Pico embedded selected ADC 
/// @param ADC_channel - --
void ADC_PicoInit(uint8_t ADC_channel);

/// @brief initialize double buffering Pi Pico selected ADC standard mode
/// @param channel_number - --
/// @param buffer_size - --
/// @param buffer_state - struct containing double buffering data
/// 
/// @param ADC_freq - 1kHz
void ADC_PicoStandardModeInit(uint8_t channel_number, uint32_t buffer_size, Pico_adcStandardMode *buffer_state);

/// @brief standard double buffering mode callback
/// @param buffer_state - --
void ADC_PicoStandardModeCallback(Pico_adcStandardMode *buffer_state);

void ADC_PicoDMAModeInit(void);

uint16_t *Pico_ADCGetData(uint8_t channel_number);


#endif