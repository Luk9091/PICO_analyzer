#ifndef _ADC_
#define _ADC_

#include "hardware/i2c.h"

#define ADS1115_SDAPin 20
#define ADS1115_SCLPin 21
#define ADS1115_I2CInstance i2c0

#define ADS1115_Address 0x48
#define conversion_reg  0b00000000
#define config_reg      0b00000001
#define LO_thresh_reg   0b00000010
#define Hi_thresh_reg   0b00000011



/// @brief I2C write register helper function
/// @param dev_addr     - device address
/// @param reg_address  - register value(address)
/// @param data         - data to save under register location
void I2C_writeReg(uint8_t dev_addr, uint8_t reg_mode, uint8_t data_length, uint8_t data);

/// @brief I2C read register helper function
/// @param dev_addr     - device address
/// @param reg_addr     - register value(address)
/// @param buffer       - pointer to buffer where data has to be saved
void I2C_readReg(uint8_t dev_addr, uint8_t reg_mode, uint8_t data_length, uint8_t *buffer);


/// @brief ADS1115 initialize 
/// @param SDA_pin - --
/// @param SCL_pin - --
void ADS1115_init(uint8_t SDA_pin, uint8_t SCL_pin);

#endif