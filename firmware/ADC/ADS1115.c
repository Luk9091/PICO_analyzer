#include "ADC.h"

// ADS1115 current settings //
ADS1115_configState ADS1115_state_t = {0};

void ADS1115_writeReg(uint8_t reg_mode, uint16_t data)
{
    uint8_t *data_ptr = (uint8_t*)&data;
    const uint8_t tab[3] = {reg_mode, data_ptr[1], data_ptr[0]};
    i2c_write_blocking(ADS1115_I2CInstance, ADS1115_Address, tab, 3, false);
}

void ADS1115_readReg(uint8_t reg_mode, uint16_t *buffer)
{
    uint8_t tab[2] = {0};
    i2c_write_blocking(ADS1115_I2CInstance, ADS1115_Address, &reg_mode, 1, true);
    i2c_read_blocking(ADS1115_I2CInstance, ADS1115_Address, tab, 2, false);
    
    *buffer = ((uint16_t)tab[0] << 8) | tab[1];
}

void ADS1115_init(uint8_t SDA_pin, uint8_t SCL_pin)
{

    /// I2C initialization ///
    i2c_init(ADS1115_I2CInstance, 100000);
    gpio_set_function(SDA_pin, GPIO_FUNC_I2C);
    gpio_set_function(SCL_pin, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_pin);
    gpio_pull_up(SCL_pin);
    

    /// ADS1115 Basic Configuration ///
    uint16_t config_val = 0b0100001111100011;
    ADS1115_writeReg(config_reg, config_val);

    ADS1115_state_t.OS_state          = OS_0;
    ADS1115_state_t.MUX_state         = channel_0;
    ADS1115_state_t.PGA_state         = PGA_5;
    ADS1115_state_t.MODE_state        = mode_singleShot;
    ADS1115_state_t.DR_state          = data_rate860;
    ADS1115_state_t.COMP_MODE_state   = comp_modeTraditional;
    ADS1115_state_t.COMP_POL_state    = comp_polActiveLow; 
    ADS1115_state_t.COMP_LAT_state    = comp_latNoLatching; 
    ADS1115_state_t.COMP_QUE_state    = comp_queDisable; 
}

void ADS1115_setOperationMode(uint8_t mode)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(config_reg, &ADS1115_state);

    switch(mode)
    {
        case mode_circular:
            ADS1115_state &= ~(1<<8);
            ADS1115_state_t.MODE_state = mode_circular;
        break;

        case mode_singleShot:
            ADS1115_state |= (1<<8);
            ADS1115_state_t.MODE_state = mode_singleShot;
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
            ADS1115_state_t.PGA_state = PGA_1;
        break;

        case PGA_2:
            ADS1115_state |= (1<<11);
            ADS1115_state &= ~((1<<10)|(1<<9));
            ADS1115_state_t.PGA_state = PGA_2;
        break;

        case PGA_3:
            ADS1115_state |= (1<<10) | (1<<9);
            ADS1115_state &= ~(1<<11);
            ADS1115_state_t.PGA_state = PGA_3;
        break;

        case PGA_4:
            ADS1115_state |= (1<<10);
            ADS1115_state &= ~((1<<11)|(1<<9));
            ADS1115_state_t.PGA_state = PGA_4;
        break;

        case PGA_5:
            ADS1115_state |= (1<<9);
            ADS1115_state &= ~((1<<11)|(1<<10));
            ADS1115_state_t.PGA_state = PGA_5;
        break;

        case PGA_6:
            ADS1115_state &= ~((1<<11)|(1<<10)|(1<<9));
            ADS1115_state_t.PGA_state = PGA_6;
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
            ADS1115_state_t.MUX_state = channel_0;
        break;

        case channel_1:
            ADS1115_state &= ~(1<<13);
            ADS1115_state |= (1<<14) | (1<<12);
            ADS1115_state_t.MUX_state = channel_1;
        break;

        case channel_2:
            ADS1115_state |= (1<<14) | (1<<13);
            ADS1115_state &= ~(1<<12);
            ADS1115_state_t.MUX_state = channel_2;
        break;

        case channel_3:
            ADS1115_state |= (1<<14) | (1<<12) | (1<<13);
            ADS1115_state_t.MUX_state = channel_3;
        break;
    }

    ADS1115_writeReg(config_reg, ADS1115_state);
}

uint16_t ADS1115_readData(uint8_t channel)
{  
    if(channel > 3)
        return 0;

    ADS1115_setChannel(channel);

    uint16_t raw_data = 0;
    ADS1115_readReg(conversion_reg, &raw_data);

    return raw_data;
}

uint16_t ADS1115_getSample(uint8_t channel)
{
    if(channel > 3)
        return 0;

    ADS1115_setChannel(channel);

    uint16_t ADS1115_state = 0; 
    ADS1115_readReg(config_reg, &ADS1115_state);
    ADS1115_state |= (1<<15);
    ADS1115_writeReg(config_reg, ADS1115_state); // start conversion
    sleep_ms(2); 

    uint16_t ADS1115_isBusy = 0;
    ADS1115_readReg(config_reg, &ADS1115_isBusy);
    while((ADS1115_isBusy & (1<<15)) == 0) // Device is currently performing a conversion
    {
        ADS1115_readReg(config_reg, &ADS1115_isBusy);
        sleep_ms(1);printf(".\n");
    }

    uint16_t raw_data = 0;
    ADS1115_readReg(conversion_reg, &raw_data);

    return raw_data;
}

float ADS1115_dataConvert(int16_t data)
{
    float voltage = 0.0f;

    switch(ADS1115_state_t.PGA_state)
    {
        case(PGA_1):
            voltage = data *  0.0000078125f;
            return voltage;
        break;

        case(PGA_2):
            voltage = data * 0.000015625f;
            return voltage;
        break;

        case(PGA_3):
            voltage = data * 0.00003125f;
            return voltage;
        break;

        case(PGA_4):
            voltage = data * 0.0000625f;
            return voltage;
        break;

        case(PGA_5):
            voltage = data * 0.000125f;
            return voltage;
        break;

        case(PGA_6):
            voltage = data * 0.0001875f;
            return voltage;
        break;

    }
}