#ifndef _ADC_
#define _ADC_

#include <stdio.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "../ring_buffer/ring_buffer.h"

#define ADS1115_SDAPin 20
#define ADS1115_SCLPin 21
#define ADS1115_I2CInstance i2c0

#define ADS1115_Address         0x48
#define ADS1115_conversionReg   0b00000000
#define ADS1115_configReg       0b00000001
#define ADS1115_LOThreshReg     0b00000010
#define ADS1115_HiThreshReg     0b00000011

#define ADS1115_OS_0  0     // 0 : Device is currently performing a conversion
#define ADS1115_OS_1  1     // 1 : Device is not currently performing a conversion

#define ADS1115_channel_0 0
#define ADS1115_channel_1 1
#define ADS1115_channel_2 2 
#define ADS1115_channel_3 3

#define ADS1115_PGA_1 1     // 101b : FSR = ±0.256V
#define ADS1115_PGA_2 2     // 100b : FSR = ±0.512V
#define ADS1115_PGA_3 3     // 011b : FSR = ±1.024V
#define ADS1115_PGA_4 4     // 010b : FSR = ±2.048V (default)
#define ADS1115_PGA_5 5     // 001b : FSR = ±4.096V
#define ADS1115_PGA_6 6     // 6.144V

#define ADS1115_modeCircular   0
#define ADS1115_modeSingleShot 1

#define ADS1115_dataRate8      0 // 000 : 8 SPS
#define ADS1115_dataRate16     1 // 001 : 16 SPS
#define ADS1115_dataRate32     2 // 010 : 32 SPS
#define ADS1115_dataRate64     3 // 011 : 64 SPS
#define ADS1115_dataRate128    4 // 100 : 128 SPS (default)
#define ADS1115_dataRate250    5 // 101 : 250 SPS
#define ADS1115_dataRate475    6 // 110 : 475 SPS
#define ADS1115_dataRate860    7 // 111 : 860 SPS

#define ADS1115_compModeTraditional    0
#define ADS1115_compModeWindow         1

#define ADS1115_compPolActiveLow       0
#define ADS1115_compPolActiveHigh      1

#define ADS1115_compLatNoLatching      0
#define ADS1115_compLatLatching        1

#define ADS1115_compQueAfterOne        0
#define ADS1115_compQueAfterTwo        1
#define ADS1115_compQueAfterFour       2
#define ADS1115_compQueDisable         3

#define ADC_ADS1115SampleNumber 250

typedef struct{
    uint8_t OS_state;
    uint8_t MUX_state;
    uint8_t PGA_state;
    uint8_t MODE_state;
    uint8_t DR_state;
    uint8_t COMP_MODE_state;
    uint8_t COMP_POL_state;
    uint8_t COMP_LAT_state;
    uint8_t COMP_QUE_state;
}ADS1115_configState;


// typedef struct{
//     bool ADS1115_isInitializedCh0;      //ADS1115 is initialized ch0      
//     bool ADS1115_isInitializedCh1;      //ADS1115 is initialized ch1
//     bool ADS1115_isInitializedCh2;      //ADS1115 is initialized ch2
//     bool ADS1115_isInitializedCh3;      //ADS1115 is initialized ch3

//     uint16_t *ADS1115_bufferPtrCh0;     //ADS1115 buffer pointer(size = ADC_SAMPLE_NUMBER)
//     uint16_t *ADS1115_bufferPtrCh1;     //ADS1115 buffer pointer(size = ADC_SAMPLE_NUMBER)
//     uint16_t *ADS1115_bufferPtrCh2;     //ADS1115 buffer pointer(size = ADC_SAMPLE_NUMBER)
//     uint16_t *ADS1115_bufferPtrCh3;     //ADS1115 buffer pointer(size = ADC_SAMPLE_NUMBER)

//     uint16_t ADS1115_lastSample;        //ADS1115 last sample Channel 0
// }ADS1115_data;


/// @brief ADS1115 write data
/// @param reg_mode - one of 4 available device registers 
/// @param data     - data to save in register
void ADS1115_writeReg(uint8_t reg_mode, uint16_t data);

/// @brief ADS1115 read data
/// @param reg_mode - one of 4 available device registers 
/// @param buffer   - buffer to store read data
void ADS1115_readReg(uint8_t reg_mode, uint16_t *buffer);

/// @brief ADS1115 initialize 
void ADS1115_init(void);

/// @brief ADC1115 set operation Mode Circular or one shot
/// @param mode - Circular/One shot
void ADS1115_setOperationMode(uint8_t mode);

/// @brief ADS11115 set Programmable Gain Amplifier
/// @param  PGA gain value
void ADS1115_setPGA(uint8_t PGA_val);

/// @brief ADS1115 set measured channel number
/// @param channel_number   - --
void ADS1115_setChannel(uint8_t channel_number);

/// @brief set up dual channel mode - read(and save) sample from first channel, read(and save) second sample from second channel
/// @param first_channel    - first ADC channel in dual channel mode
/// @param second_channel   - second ADC channel in dual channel mode
/// @param data_buffer1     - first data buffer
/// @param data_buffer2     - second data buffer
void ADS1115_setDualChannelRoutine(uint8_t first_channel, uint8_t second_channel, ring_buffer *data_buffer1, ring_buffer *data_buffer2);

/// @brief READ ADC one sample from selected channel
/// @param channel  - --
/// @return         - ADC single sample   
uint16_t ADS1115_getSample(uint8_t channel);


/// @brief ADC received raw data converter -> convert from raw data to voltages
/// @warning This method is not recommended to use on rp2040 !!!, is ONLY for debugging 
/// @param data - raw ADC value
/// @return     - voltage value
float ADS1115_dataConvert(int16_t data);
#endif