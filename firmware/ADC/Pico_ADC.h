#ifndef _PICO_ADC_
#define _PICO_ADC_

#include <assert.h>
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "DMA/dma.h"
#include "../ring_buffer/ring_buffer.h"

#define ADC_PicoChannel_0 0         // ADC 0 Input Pin  <-> ADC 0 channel
#define ADC_PicoChannel_1 1         // ADC 1 Input Pin  <-> ADC 1 channel
#define ADC_PicoChannel_2 2         // ADC 2 Input Pin  <-> ADC 2 channel
#define ADC_PicoPinCh_0 26
#define ADC_PicoPinCh_1 27

#define ADC_PicoSampleNumber 500    // 500 samples = samples from channel_0 + samples from channel_1
#define ADC_PicoADCClkDiv 100.0f    // 100.0f <-> 5kHz 

typedef struct{
    uint16_t current_buffer;
    uint16_t ADC_buffer_1[2 * ADC_PicoSampleNumber];
    uint16_t ADC_buffer_2[2 * ADC_PicoSampleNumber];
    int DMA_channel;
    dma_channel_config DMA_cfg;
}Pico_adcDmaModeState;

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

/// @brief initialize Pi Pico embedded ADC in Circle mode with DMA
/// @param  - --
/// @attention this mode use ADC channel 0 and channel 1 
void ADC_PicoDMAModeInit(void);


/// @brief Pi pico double buffering with DMA received data getter
/// @param - --
/// @return ptr to fully filled data buffer 
uint16_t *ADC_PicoDMAModeGetData(void);

uint32_t ADC_PicoDMAModeGetCapacity(void);

#endif