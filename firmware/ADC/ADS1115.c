#include "ADS1115.h"

// ADS1115 current settings //
static ADS1115_configState ADS1115_state_t = {0};

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

void ADS1115_init(void)
{

    /// I2C initialization ///
    i2c_init(ADS1115_I2CInstance, 400000);
    gpio_set_function(ADS1115_SDAPin, GPIO_FUNC_I2C);
    gpio_set_function(ADS1115_SCLPin, GPIO_FUNC_I2C);
    gpio_pull_up(ADS1115_SDAPin);
    gpio_pull_up(ADS1115_SCLPin);
    

    /// ADS1115 Basic Configuration ///
    uint16_t config_val = 0b0100001111100011;
    ADS1115_writeReg(ADS1115_configReg, config_val);

    ADS1115_state_t.OS_state          = ADS1115_OS_0;
    ADS1115_state_t.MUX_state         = ADS1115_channel_0;
    ADS1115_state_t.PGA_state         = ADS1115_PGA_5;
    ADS1115_state_t.MODE_state        = ADS1115_modeSingleShot;
    ADS1115_state_t.DR_state          = ADS1115_dataRate860;
    ADS1115_state_t.COMP_MODE_state   = ADS1115_compModeTraditional;
    ADS1115_state_t.COMP_POL_state    = ADS1115_compPolActiveLow; 
    ADS1115_state_t.COMP_LAT_state    = ADS1115_compLatNoLatching; 
    ADS1115_state_t.COMP_QUE_state    = ADS1115_compQueDisable; 
}

void ADS1115_setOperationMode(uint8_t mode)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(ADS1115_configReg, &ADS1115_state);

    switch(mode)
    {
        case ADS1115_modeCircular:
            ADS1115_state &= ~(1<<8);
            ADS1115_state_t.MODE_state = ADS1115_modeCircular;
        break;

        case ADS1115_modeSingleShot:
            ADS1115_state |= (1<<8);
            ADS1115_state_t.MODE_state = ADS1115_modeSingleShot;
        break;
    }
    
    ADS1115_writeReg(ADS1115_configReg, ADS1115_state);
}

void ADS1115_setPGA(uint8_t PGA_val)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(ADS1115_configReg, &ADS1115_state);

    switch(PGA_val)
    {
        case ADS1115_PGA_1:
            ADS1115_state |= (1<<11) | (1<<9);
            ADS1115_state &= ~(1<<10);
            ADS1115_state_t.PGA_state = ADS1115_PGA_1;
        break;

        case ADS1115_PGA_2:
            ADS1115_state |= (1<<11);
            ADS1115_state &= ~((1<<10)|(1<<9));
            ADS1115_state_t.PGA_state = ADS1115_PGA_2;
        break;

        case ADS1115_PGA_3:
            ADS1115_state |= (1<<10) | (1<<9);
            ADS1115_state &= ~(1<<11);
            ADS1115_state_t.PGA_state = ADS1115_PGA_3;
        break;

        case ADS1115_PGA_4:
            ADS1115_state |= (1<<10);
            ADS1115_state &= ~((1<<11)|(1<<9));
            ADS1115_state_t.PGA_state = ADS1115_PGA_4;
        break;

        case ADS1115_PGA_5:
            ADS1115_state |= (1<<9);
            ADS1115_state &= ~((1<<11)|(1<<10));
            ADS1115_state_t.PGA_state = ADS1115_PGA_5;
        break;

        case ADS1115_PGA_6:
            ADS1115_state &= ~((1<<11)|(1<<10)|(1<<9));
            ADS1115_state_t.PGA_state = ADS1115_PGA_6;
        break;
    }

    ADS1115_writeReg(ADS1115_configReg, ADS1115_state);
}

void ADS1115_setChannel(uint8_t channel_number)
{
    uint16_t ADS1115_state = 0;
    ADS1115_readReg(ADS1115_configReg, &ADS1115_state);

    switch(channel_number)
    {
        case ADS1115_channel_0:
            ADS1115_state &= ~((1<<13)|(1<<12));
            ADS1115_state |= (1<<14);
            ADS1115_state_t.MUX_state = ADS1115_channel_0;
        break;

        case ADS1115_channel_1:
            ADS1115_state &= ~(1<<13);
            ADS1115_state |= (1<<14) | (1<<12);
            ADS1115_state_t.MUX_state = ADS1115_channel_1;
        break;

        case ADS1115_channel_2:
            ADS1115_state |= (1<<14) | (1<<13);
            ADS1115_state &= ~(1<<12);
            ADS1115_state_t.MUX_state = ADS1115_channel_2;
        break;

        case ADS1115_channel_3:
            ADS1115_state |= (1<<14) | (1<<12) | (1<<13);
            ADS1115_state_t.MUX_state = ADS1115_channel_3;
        break;
    }

    ADS1115_writeReg(ADS1115_configReg, ADS1115_state);
}

uint16_t ADS1115_getSample(uint8_t channel)
{
    if(channel > 3)
        return 0;

    ADS1115_setChannel(channel);

    uint16_t ADS1115_state = 0; 
    ADS1115_readReg(ADS1115_configReg, &ADS1115_state);
    ADS1115_state |= (1<<15);
    ADS1115_writeReg(ADS1115_configReg, ADS1115_state); //start conversion
    //sleep_us(800); 

    uint16_t ADS1115_isBusy = 0;
    ADS1115_readReg(ADS1115_configReg, &ADS1115_isBusy);
    while((ADS1115_isBusy & (1<<15)) == 0) //Device is currently performing a conversion
    {
        ADS1115_readReg(ADS1115_configReg, &ADS1115_isBusy);
        //sleep_us(1);
    }

    uint16_t raw_data = 0;
    ADS1115_readReg(ADS1115_conversionReg, &raw_data);

    return raw_data;
}

float ADS1115_dataConvert(int16_t data)
{
    float voltage = 0.0f;

    switch(ADS1115_state_t.PGA_state)
    {
        case(ADS1115_PGA_1):
            voltage = data *  0.0000078125f;
            return voltage;
        break;

        case(ADS1115_PGA_2):
            voltage = data * 0.000015625f;
            return voltage;
        break;

        case(ADS1115_PGA_3):
            voltage = data * 0.00003125f;
            return voltage;
        break;

        case(ADS1115_PGA_4):
            voltage = data * 0.0000625f;
            return voltage;
        break;

        case(ADS1115_PGA_5):
            voltage = data * 0.000125f;
            return voltage;
        break;

        case(ADS1115_PGA_6):
            voltage = data * 0.0001875f;
            return voltage;
        break;

    }
}

bool ADS1115_setChannelDoubleBuffering(uint8_t channel_number, uint32_t buffer_size, ADS1115_doubleBufferState *BufferState)
{  
    if(channel_number < 0 || channel_number > 4)
        return true;

    if(buffer_size <= 0)
        return true;
    
    BufferState->channel_number = channel_number;
    ring_bufferInit(BufferState->buffer_1, buffer_size);
    ring_bufferInit(BufferState->buffer_2, buffer_size);
    BufferState->current_buffer = BufferState->buffer_1;
}

void ADS1115_doubleBufferingCallback(ADS1115_doubleBufferState *buffer_state)
{
    if(buffer_state == NULL)
        return;

    ring_bufferPush(buffer_state->current_buffer, ADS1115_getSample(buffer_state->channel_number));

    if(ring_bufferGetCapacity(buffer_state->current_buffer) >= buffer_state->current_buffer->buffer_size)
    {
        if (buffer_state->current_buffer == buffer_state->buffer_1)
            buffer_state->current_buffer =  buffer_state->buffer_2;
        else
            buffer_state->current_buffer = buffer_state->buffer_1;
    }
}
