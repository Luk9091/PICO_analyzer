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

    switch(mode)
    {
        case mode_circular:
            ADS1115_state &= ~(1<<8);
        break;

        case mode_singleShot:
            ADS1115_state |= (1<<8);
        break;
    }
    
    ADS1115_writeReg(config_reg, ADS1115_state);
}

void ADS1115_setPGA(uint8_t PGA_val)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(config_reg, &ADS1115_state);

    switch(PGA_val)
    {
        case PGA_1:
            ADS1115_state |= (1<<11) | (1<<9);
            ADS1115_state &= ~(1<<10);
        break;

        case PGA_2:
            ADS1115_state |= (1<<11);
            ADS1115_state &= ~((1<<10)|(1<<9));
        break;

        case PGA_3:
            ADS1115_state |= (1<<10) | (1<<9);
            ADS1115_state &= ~(1<<11);
        break;

        case PGA_4:
            ADS1115_state |= (1<<10);
            ADS1115_state &= ~((1<<11)|(1<<9));
        break;

        case PGA_5:
            ADS1115_state |= (1<<9);
            ADS1115_state &= ~((1<<11)|(1<<10));
        break;

        case PGA_6:
            ADS1115_state &= ~((1<<11)|(1<<10)|(1<<9));
        break;
    }

    ADS1115_writeReg(config_reg, ADS1115_state);
}

void ADS1115_setChannel(uint8_t channel_number)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(config_reg, &ADS1115_state);

    switch(channel_number)
    {
        case channel_0:
            ADS1115_state &= ~((1<<13)|(1<<12));
            ADS1115_state |= (1<<14);
        break;

        case channel_1:
            ADS1115_state &= ~(1<<13);
            ADS1115_state |= (1<<14) | (1<<12);
        break;

        case channel_2:
            ADS1115_state |= (1<<13) | (1<<12);
            ADS1115_state &= ~(1<<14);
        break;

        case channel_3:
            ADS1115_state |= (1<<14) | (1<<12) | (1<<13);
        break;
    }

    ADS1115_writeReg(config_reg, ADS1115_setChannel);
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