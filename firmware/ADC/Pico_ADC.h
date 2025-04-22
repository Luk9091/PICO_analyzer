#ifndef _PICO_ADC_
#define _PICO_ADC_

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"

#define ADC_0 26    // ADC 0 Input Pin
#define ADC_1 27    // ADC 1 Input Pin


/// @brief ADC init - initialize ADC and DMA
/// @param ADC_pin - --
void Pico_ADCInit(uint8_t ADC_pin0, uint8_t ADC_pin1);


#endif