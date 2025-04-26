#ifndef _ADC_BOOT_STRAP_
#define _ADC_BOOT_STRAP_

#include "ADS1115.h"
#include "Pico_ADC.h"

#include <pico/stdlib.h>
#include <hardware/timer.h>


/// @brief set up both(ADS1115 and Pi Pico ADC) ADC's
/// @param - --  
void ADC_bootStrap(void);

/// @brief ADS1115 received samples getter
/// @param channel_number - --
void ADS1115_ADCGetData(uint8_t channel_number, ADS1115_doubleBufferState *buffer);

#endif