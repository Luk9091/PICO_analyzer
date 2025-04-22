#ifndef _PICO_ADC_
#define _PICO_ADC_

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include "DMA/dma.h"
#include "../ring_buffer/ring_buffer.h"

#define Pico_ADCChannel_0 26    // ADC 0 Input Pin  <-> ADC 0 channel
#define Pico_ADCChannel_1 27    // ADC 1 Input Pin  <-> ADC 1 channel
#define Pico_ADCChannel_2 28    // ADC 2 Input Pin  <-> ADC 2 channel

#define ADC_PicoSampleNumber (4 * ADC_ADS1115SampleNumber) // 4 times higher freq <-> 4 times more samples

/// @brief - initialize Pi Pico embedded selected ADC 
/// @param ADC_channel - --
void ADC_PicoInit(uint8_t ADC_channel);

/// @brief ADC received raw data converter -> convert from raw data to voltages
/// @warning This method is not recommended to use on rp2040 !!!, is ONLY for debugging 
/// @param data - raw ADC value
/// @return     - voltage value
void ADC_PicoSetDualChannelRoutine(uint8_t first_channel, uint8_t second_channel, ring_buffer *data_buffer1, ring_buffer *data_buffer2);

#endif