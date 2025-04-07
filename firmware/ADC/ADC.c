#include "ADC.h"

static void ADS1115_writeReg(uint8_t reg_mode, uint16_t data)
{
    uint8_t *data_ptr = (uint8_t*)&data;
    const uint8_t tab[3] = {reg_mode, data_ptr[1], data_ptr[0]};
    i2c_write_blocking(ADS1115_I2CInstance, ADS1115_Address, tab, 3, false);
}

static void ADS1115_readReg(uint8_t reg_mode, uint16_t *buffer)
{
    uint8_t tab[2] = {0};
    i2c_write_blocking(ADS1115_I2CInstance, ADS1115_Address, &reg_mode, 1, true);
    i2c_read_blocking(ADS1115_I2CInstance, ADS1115_Address, tab, 2, false);
    
    *buffer = ((uint16_t)tab[0] << 8) | tab[1];
}

void ADS1115_init(uint8_t SDA_pin, uint8_t SCL_pin, ADS1115_operationMode operation_mode)
{

    /// I2C initialization ///
    i2c_init(ADS1115_I2CInstance, 100000);
    gpio_set_function(ADS1115_SDAPin, GPIO_FUNC_I2C);
    gpio_set_function(ADS1115_SCLPin, GPIO_FUNC_I2C);
    gpio_pull_up(ADS1115_SDAPin);
    gpio_pull_up(ADS1115_SCLPin);
    

    /// ADS1115 Basic Configuration ///
    uint16_t config_val = 0b0100001011100011;
    ADS1115_writeReg(config_reg, config_val);
}

void ADS1115_setOperationMode(ADS1115_operationMode mode)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(config_reg, &ADS1115_state);

    uint16_t mode_t = 0xffff;
    mode_t &= mode;
    mode_t &= ADS1115_state;
    ADS1115_writeReg(config_reg, mode_t);
}

void ADS1115_setPGA(uint8_t PGA_val)
{
    //TODO
}

void ADS1115_setChannel(uint8_t channel_number)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(config_reg, &ADS1115_state);

    uint16_t channel_t = 0xffff;
    channel_t &= channel_number;
    channel_t &= ADS1115_state;
    ADS1115_writeReg(config_reg, channel_t);
}

uint16_t ADS1115_readData(uint8_t channel)
{
    ADS1115_setChannel(channel);
    uint16_t raw_data = 0;
    ADS1115_readReg(conversion_reg, &raw_data);

    return raw_data;
}

uint16_t ADS1115_getSample(uint8_t channel)
{
    ADS1115_setChannel(channel);
    ADS1115_writeReg(config_reg, 0x8000);
    
    uint16_t ADS1115_isBusy = 0;
    ADS1115_readReg(conversion_reg, &ADS1115_isBusy);
    while(!ADS1115_isBusy & (1<<15)) // Device is currently performing a conversion
    {
        ADS1115_readReg(conversion_reg, &ADS1115_isBusy);
        sleep_ms(1);
    }

    uint16_t raw_data = 0;
    ADS1115_readReg(conversion_reg, &raw_data);

    return raw_data;
}