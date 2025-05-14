#ifndef _MULTICORE_FIFO_
#define _MULTICORE_FIFO_

#include <stdbool.h>
#include <stdio.h>
#include <pico/multicore.h>
#include "hardware/irq.h"
#include "Communication/communication_config.h"

/// Core 0 valid buffer defines
#define mc_fifoBuffer0 0
#define mc_fifoBuffer1 1
#define mc_fifoBuffer2 2
#define mc_fifoBuffer3 3


/// @note
/// Pi Pico multicore fifo has 8 words each 32 bit's 
/// 
///
/// Core0 --TO--> Core1 frame format
/// _____________________________________________________________________________________________________________
/// | NOT USED | NOT USED | NOT USED | NOT USED | NOT USED |  valid buffer | Valid data size | Valid data size |  
/// |   XXXX   |   XXXX   |   XXXX   |   XXXX   |   XXXX   |    [1|2|3|4]  |    [0 - 255]    |    [0 - 255]    |
/// |   XXXX   |   XXXX   |   XXXX   |   XXXX   |   XXXX   |       &&&&    |      XXXX       |      XXXX       |
/// ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
///
/// Core1 --TO--> Core0 frame format
/// _________________________________________________________________________________________________________________
/// | NOT USED | NOT USED | NOT USED | Frame Tag | received config | received config |  valid buffer | valid buffer | 
/// |   XXXX   |   XXXX   |   XXXX   |   0|&&&   |    [&|&|&|&]    |    [&|&|&|&]    |    [&|&|&|&]  |   [&|&|&|&]  |  
/// |   XXXX   |   XXXX   |   XXXX   |   X|&&&   |    [&|&|&|&]    |    [&|&|&|&]    |    [&|&|&|&]  |   [&|&|&|&]  |  
/// ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
/// FRAME TAG : 100 - frame contain data, 010 - frame contain config, 001 - frame contain data and config 


/// @brief 
/// @param  
void multicore_fifoInitCore0(void);

/// @brief 
/// @param  
void multicore_fifoInitCore1(void);

void core0_sio_irq();
void core1_sio_irq();
#endif