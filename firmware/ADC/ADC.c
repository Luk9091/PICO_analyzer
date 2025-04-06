#include "ADC.h"

uint8_t get_ADS1115RegMode(address_pointerRegister reg_mode)
{
    
    switch(reg_mode)
    {
        case conversion_register:
             return 0b00000000;
        break;

        case config_register:
            return 0b00000001;
        break;

        case LO_thresh_register:
            return 0b00000010;
        break;

        case Hi_thresh_register:
            return 0b00000011;
        break;

        default:
            return 0b00000000;
    }
}

void I2C_writeReg(uint8_t dev_addr, address_pointerRegister reg_mode, uint8_t data_length, uint8_t data)
{
    const uint8_t tab[] = {get_ADS1115RegMode(reg_mode), data};
    i2c_write_blocking(ADS1115_I2CInstance, dev_addr, tab, data_length, false);
}

void I2C_readReg(uint8_t dev_addr, address_pointerRegister reg_mode, uint8_t data_length, uint8_t* buffer)
{
    const uint8_t tab = get_ADS1115RegMode(reg_mode);
    uint8_t register_mode =  get_ADS1115RegMode(reg_mode);
    i2c_write_blocking(ADS1115_I2CInstance, register_mode, &tab, 1, false);
    i2c_read_blocking(ADS1115_I2CInstance, dev_addr, buffer, data_length, false);
}

void ADS1115_init(uint8_t SDA_pin, uint8_t SCL_pin)
{
    uint16_t tab = 0b0000000000000000;
    I2C_writeReg(ADS1115_Address, config_register, 2, tab);

    //TODO
}