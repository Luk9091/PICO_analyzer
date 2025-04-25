#ifndef _PICO_ADC_
#define _PICO_ADC_

#include <assert.h>
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "DMA/dma.h"
#include "../ring_buffer/ring_buffer.h"

#define ADC_PicoChannel_0 26    // ADC 0 Input Pin  <-> ADC 0 channel
#define ADC_PicoChannel_1 27    // ADC 1 Input Pin  <-> ADC 1 channel
#define ADC_PicoChannel_2 28    // ADC 2 Input Pin  <-> ADC 2 channel

#define ADC_PicoSampleNumber (4 * ADC_ADS1115SampleNumber) // 4 times higher freq <-> 4 times more samples


typedef struct{
    uint16_t    channel_number;        // channel number
    ring_buffer *current_buffer;    // current buffer where new samples are saved
    ring_buffer *buffer_1;         
    ring_buffer *buffer_2;         
}Pico_ADCBufferState;

/// @brief - initialize Pi Pico embedded selected ADC 
/// @param ADC_channel - --
void ADC_PicoInit(uint8_t ADC_channel);

void ADC_PicoStandardModeInit(uint8_t channel_number, uint32_t buffer_size, Pico_ADCBufferState *buffer_state);

void ADC_PicoStandardModeCallback(Pico_ADCBufferState *buffer_state);
#endif