#ifndef _ADC_BOOT_STRAP_
#define _ADC_BOOT_STRAP_

#include "ADS1115.h"
#include "Pico_ADC.h"

#include <pico/stdlib.h>
#include <hardware/timer.h>


/// @brief Sets up both the ADS1115 and Pi Pico ADCs.
/// @param - --  
void ADC_bootStrap(void);

/// @brief ADS1115 & Pico ADC - standard mode IRQ. Use this IRQ handler in your repeating loop to perform periodic measurements.
/// @param  - --
void ADC_standardModeIrq(void);

/// @brief ADS1115 received samples getter
/// @param channel_number - --
uint16_t *ADS1115_ADCGetData(uint8_t channel_number);

/// @brief Gets the proper buffer in standard mode for the Pi Pico ADC.
/// @param channel - --
/// @return pointer to proper buffer
uint16_t *ADC_PicoStandardModeGetData(uint8_t ADC_channelNumber);

#endif