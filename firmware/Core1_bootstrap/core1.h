#ifndef _CORE_1_
#define _CORE_1_

#include "multicore_fifo.h"
#include "ADC/ADC_bootStrap.h"
#include "ADC/ADS1115.h"
#include "ADC/Pico_ADC.h"
#include "Wifi/wifi.h"
#include "hardware/timer.h"

/// @brief initialize core 1 working routine
/// @param - --
void core1_init(void);


#endif