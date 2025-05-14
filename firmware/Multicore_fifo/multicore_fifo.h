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





#endif