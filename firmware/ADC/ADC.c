#include "ADC.h"



void I2C_writeReg(uint8_t dev_addr, uint8_t reg_mode, uint8_t data_length, uint8_t data)
{
    const uint8_t tab[] = {reg_mode, data};
    i2c_write_blocking(ADS1115_I2CInstance, dev_addr, tab, data_length, false);
}

void I2C_readReg(uint8_t dev_addr, uint8_t reg_mode, uint8_t data_length, uint8_t* buffer)
{
    i2c_write_blocking(ADS1115_I2CInstance, reg_mode, buffer, 1, false);
    i2c_read_blocking(ADS1115_I2CInstance, dev_addr, buffer, data_length, false);
}

void ADS1115_init(uint8_t SDA_pin, uint8_t SCL_pin)
{
    uint16_t tab = 0b0000000000000000;
    I2C_writeReg(ADS1115_Address, config_reg, 2, tab);

    //TODO
}