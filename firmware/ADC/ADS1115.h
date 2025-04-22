#ifndef _ADC_
#define _ADC_

#include <stdio.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define ADS1115_SDAPin 20
#define ADS1115_SCLPin 21
#define ADS1115_I2CInstance i2c0

#define ADS1115_Address 0x48
#define conversion_reg  0b00000000
#define config_reg      0b00000001
#define LO_thresh_reg   0b00000010
#define Hi_thresh_reg   0b00000011

#define OS_0  0     // 0 : Device is currently performing a conversion
#define OS_1  1     // 1 : Device is not currently performing a conversion

#define channel_0 0
#define channel_1 1
#define channel_2 2 
#define channel_3 3

#define PGA_1 1     // 101b : FSR = ±0.256V
#define PGA_2 2     // 100b : FSR = ±0.512V
#define PGA_3 3     // 011b : FSR = ±1.024V
#define PGA_4 4     // 010b : FSR = ±2.048V (default)
#define PGA_5 5     // 001b : FSR = ±4.096V
#define PGA_6 6     // 6.144V

#define mode_circular   0
#define mode_singleShot 1

#define data_rate8      0 // 000 : 8 SPS
#define data_rate16     1 // 001 : 16 SPS
#define data_rate32     2 // 010 : 32 SPS
#define data_rate64     3 // 011 : 64 SPS
#define data_rate128    4 // 100 : 128 SPS (default)
#define data_rate250    5 // 101 : 250 SPS
#define data_rate475    6 // 110 : 475 SPS
#define data_rate860    7 // 111 : 860 SPS

#define comp_modeTraditional    0
#define comp_modeWindow         1

#define comp_polActiveLow       0
#define comp_polActiveHigh      1

#define comp_latNoLatching      0
#define comp_latLatching        1

#define comp_queAfterOne        0
#define comp_queAfterTwo        1
#define comp_queAfterFour       2
#define comp_queDisable         3

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

/// @brief Read ADC data from selected channel
/// @param channel  - -- 
/// @return         - raw ads1115 ADC data
uint16_t ADS1115_readData(uint8_t channel);

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