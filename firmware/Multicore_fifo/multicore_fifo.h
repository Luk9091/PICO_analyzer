#ifndef _MULTICORE_FIFO_
#define _MULTICORE_FIFO_

#include <stdbool.h>
#include <stdio.h>
#include <pico/multicore.h>

/// Core 0 valid buffer defines
#define mc_fifoBuffer0 0
#define mc_fifoBuffer1 1
#define mc_fifoBuffer2 2
#define mc_fifoBuffer3 3

#define digital_analyzerMaxBufferSize 256

#define mc_readyCore1 0x00000001

/// @note
/// Pi Pico multicore fifo has 8 words each 32 bit's 
/// 
///
/// Core0 -> Core1 frame format
/// _________________________________________________________________________________________________________________________
/// | valid buffer [1|2|3|4] | NOT USED | NOT USED | NOT USED | NOT USED | NOT USED | NOT USED  | Valid data size [0 - 255] |   
/// |          &&&&          |   XXXX   |   XXXX   |   XXXX   |  XXXX    |   XXXX   |   XXXX    |            XXXX           |   
/// ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
///
/// Core1 -> Core0 frame format
/// ___________________________________________________________________________________________________________________
/// | NOT USED | NOT USED | NOT USED | NOT USED | NOT USED | NOT USED | NOT USED | Received data from fifo flag [0|1] |   
/// |   XXXX   |   XXXX   |   XXXX   |   XXXX   |   XXXX   |   XXXX   |  XXXX    |                XX&&                |   
/// ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾



/// @brief Core0 fifo helper function - Valid buffer setter
/// @param buffer_number    - valid buffer number | 1 | 2 | 3 | 4 |
/// @param valid_bufferSize - valid buffer size [0 - 255]
/// @return true -> valid buffer was successfully set, false otherwise
bool fifo_trySetValidBuffer(uint8_t buffer_number, uint8_t valid_bufferSize);

/// @brief Core1 fifo helper function - Ready status register setter
/// @param  - --
/// @return true -> status was successfully set, false otherwise
bool fifo_trySetReadStatus(void);

/// @brief Core1 fifo helper function - Valid buffer getter
/// @param buffer_number - --
/// @param buffer_size - --
/// @return true -> read buffer was successfully read, false otherwise
bool fifo_tryReadBufferData(uint8_t *buffer_number, uint8_t *buffer_size);

#endif