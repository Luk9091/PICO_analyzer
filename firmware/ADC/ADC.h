#ifndef _ADC_
#define _ADC_

#include "hardware/i2c.h"

#define ADS1115_SDAPin 20
#define ADS1115_SCLPin 21
#define ADS1115_I2CInstance i2c0

#define ADS1115_Address 0x48

typedef enum{
    conversion_register,
    config_register,
    LO_thresh_register,
    Hi_thresh_register
}address_pointerRegister;


/// @brief I2C write register helper function
/// @param dev_addr     - device address
/// @param reg_address  - register value(address)
/// @param data         - data to save under register location
void I2C_writeReg(uint8_t dev_addr, address_pointerRegister reg_mode, uint8_t data_length, uint8_t data);

/// @brief I2C read register helper function
/// @param dev_addr     - device address
/// @param reg_addr     - register value(address)
/// @param buffer       - pointer to buffer where data has to be saved
void I2C_readReg(uint8_t dev_addr, address_pointerRegister reg_mode, uint8_t data_length, uint8_t *buffer);

/// @brief  get ADS1115 register mode: 
///         - 00b : Conversion register
///         - 01b : Config register
///         - 10b : Lo_thresh register
///         - 11b : Hi_thresh register
/// @param reg_mode target register mode
/// @return ADS1115 ADC register mode - see address_pointerRegister enum 
uint8_t get_ADS1115RegMode(address_pointerRegister reg_mode);

/// @brief ADS1115 initialize 
/// @param SDA_pin - --
/// @param SCL_pin - --
void ADS1115_init(uint8_t SDA_pin, uint8_t SCL_pin);

#endif