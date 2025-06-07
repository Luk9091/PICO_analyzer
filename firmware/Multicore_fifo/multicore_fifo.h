#ifndef _MULTICORE_FIFO_
#define _MULTICORE_FIFO_

#include <stdbool.h>
#include <stdio.h>
#include <pico/multicore.h>
#include "hardware/irq.h"
#include "Communication/communication_config.h"


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
/// ________________________________________________________________________________________________________________________
/// | NOT USED | NOT USED | Frame Tag | received config | received config | received config | valid buffer | valid buffer  | 
/// |   XXXX   |   XXXX   |   X|&&&   |    [X|AC|BC|CC] |  [DC|EC|FC|GC]  |  [HC|IC|JC|KC]  |  [X|X|X|X]   | [AD|BD|CD|DD] |  
/// |   XXXX   |   XXXX   |   X|&&&   |    [X|AC|BC|CC] |  [DC|EC|FC|GC]  |  [HC|IC|JC|KC]  |  [X|X|X|X]   | [AD|BD|CD|DD] | 
/// ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
/// FRAME TAG : 100 - frame contain data, 010 - frame contain config, 001 - frame contain data and config 
///
///AC = 1 - ADC_ADS1115_CH1_ENABLE,  0 otherwise
///BC = 1 - ADC_ADS1115_CH2_ENABLE,  0 otherwise
///CC = 1 - ADC_PICO_CH1_ENABLE,     0 otherwise
///DC = 1 - ADC_PICO_CH2_ENABLE,     0 otherwise
///EC = 1 - DIGITAL_PROBE_ENABLE,    0 otherwise
///FC = 1 - DIGITAL_MODE_FREERUN,    0 otherwise
///GC = 1 - DIGITAL_MODE_TRIGGERED,  0 otherwise
///HC = 1 - DIGITAL_SET_SAMPLE_FREQ, 0 otherwise
///IC = 1 - DIGITAL_SET_TIMER,       0 otherwise
///JC = 1 - DEVICE_STOP,             0 otherwise
///KC = 1 - DEVICE_RUN,              0 otherwise
///
///AD = 1 - ADC_ADS1115_CH1_VALID,   0 otherwise
///BD = 1 - ADC_ADS1115_CH2_VALID,   0 otherwise
///CD = 1 - ADC_PICO_CH1_VALID,      0 otherwise
///DD = 1 - ADC_PICO_CH2_VALID,      0 otherwise
///
/// XX - not used bit's 



/// @brief Core 0(primary core) fifo initialization
/// @param - --
void multicore_fifoInitCore0(void);

/// @brief Core 1(secondary) fifo initialization
/// @param - --
void multicore_fifoInitCore1(void);

/// @brief Core 0 helper function - try add valid data buffer number and size to fifo
/// @param buffer_size - --
/// @param buffer_number - --
/// @return true if everything goes correct, false otherwise
bool multicore_fifoTryPushCore0(uint8_t buffer_size, core0_validBufferNumber buffer_number);

/// @brief Core 1 helper function - try add received from PC config data and/or valid ADC buffer 
/// @param frame_type - 100(data frame), 010(config frame), 001(data and config frame) 
/// @param new_deviceConfig - received new device config frame, NULL otherwise
/// @param ADC_bufferStatus - valid buffer status, NULL otherwise
/// @return true if everything goes correct, false otherwise
// bool multicore_fifoTryPushCore1(fifo_frameType_t frame_type, device_configStatus_t *new_deviceConfig, ADC_validBufferStatus *ADC_bufferStatus);


///@note multicore_fifoTryPushCore0() & multicore_fifoTryPushCore1() are not necessary because of fifo rvalid IRQ's
#endif