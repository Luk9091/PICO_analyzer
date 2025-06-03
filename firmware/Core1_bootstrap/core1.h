#ifndef _CORE_1_
#define _CORE_1_

#include <hardware/timer.h>

#include "wifi.h"
#include "multicore_fifo.h"
#include "ADC_bootStrap.h"
#include "ADS1115.h"
#include "Pico_ADC.h"

/// @brief initialize core 1 working routine
/// @param - --
void core1_init(void);

#endif