#ifndef _ADC_
#define _ADC_

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

typedef enum{
    mode_circular,
    mode_singleShot
}ADS1115_operationMode;

typedef enum{
    channel_0,
    channel_1,
    channel_2,
    channel_3
}ADS1115_channel;

typedef enum{
    PGA_1,   //101b : FSR = ±0.256V
    PGA_2,  //100b : FSR = ±0.512V
    PGA_3,  //011b : FSR = ±1.024V
    PGA_4,  //010b : FSR = ±2.048V (default)
    PGA_5,  //001b : FSR = ±4.096V(1)
    PGA_6  //6.144V(1)
}ADS1115_PGA;

/// @brief ADS1115 write data
/// @param reg_mode - one of 4 available device registers 
/// @param data     - data to save in register
void ADS1115_writeReg(uint8_t reg_mode, uint16_t data);

/// @brief ADS1115 read data
/// @param reg_mode - one of 4 available device registers 
/// @param buffer   - buffer to store read data
void ADS1115_readReg(uint8_t reg_mode, uint16_t *buffer);

/// @brief ADS1115 initialize 
/// @param SDA_pin  - --
/// @param SCL_pin  - --
void ADS1115_init(uint8_t SDA_pin, uint8_t SCL_pin, ADS1115_operationMode operation_mode);

/// @brief ADC1115 set operation Mode Circular or one shot
/// @param mode - Circular/One shot
void ADS1115_setOperationMode(ADS1115_operationMode mode);

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


#endif