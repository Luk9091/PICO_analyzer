#include "ADS1115.h"

/// PRIVATE FUNCTIONS PROTOTYPES ///
static void i2c_irqHandler(void);
static void ADS1115_writeReg(uint8_t reg_mode, uint16_t data);
static void ADS1115_readReg(uint8_t reg_mode, uint16_t *buffer);
static void ADS1115_writeRegAsync(uint8_t reg_mode, uint16_t data);
static void ADS1115_readRegAsync(uint8_t reg_mode, uint16_t *buffer);


// ADS1115 current settings //
static ADS1115_configState ADS1115_state_t = {0};
static uint8_t ADS1115_currentChannel = 0;


// ADS1115 double buffering mode - private buffer's(copy) pointers
static ADS1115_channelConfig *ADS1115_ch0 = NULL;
static ADS1115_channelConfig *ADS1115_ch1 = NULL;

// I2C state machine
static I2C_irqTransmissionState I2C_irqTransmissionState_t = {0};


/// I2C COMMUNICATION ///
static void ADS1115_writeReg(uint8_t reg_mode, uint16_t data)
{
    uint8_t *data_ptr = (uint8_t*)&data;
    const uint8_t tab[3] = {reg_mode, data_ptr[1], data_ptr[0]};
    //i2c_write_timeout_us(ADS1115_I2CInstance, ADS1115_Address, tab, 3, false, 200);
    i2c_write_blocking(ADS1115_I2CInstance, ADS1115_Address, tab, 3, false);
}

static void ADS1115_readReg(uint8_t reg_mode, uint16_t *buffer)
{
    uint8_t tab[2] = {0};
    //i2c_write_timeout_us(ADS1115_I2CInstance, ADS1115_Address, &reg_mode, 1, true, 200);
    //i2c_read_timeout_us(ADS1115_I2CInstance, ADS1115_Address, tab, 2, false, 200);
    
    i2c_write_blocking(ADS1115_I2CInstance, ADS1115_Address, &reg_mode, 1, true);
    i2c_read_blocking(ADS1115_I2CInstance, ADS1115_Address, tab, 2, false);

    *buffer = ((uint16_t)tab[0] << 8) | tab[1];
}

static void ADS1115_writeRegAsync(uint8_t reg_mode, uint16_t data)
{
    I2C_irqTransmissionState_t.I2C_irqWritePhase_t = I2C_WRITE_PHASE_WRITE;
    I2C_irqTransmissionState_t.user_configRegTypeW = reg_mode;
    I2C_irqTransmissionState_t.user_dataToWriteW[0] = (uint8_t)(data>>8);
    I2C_irqTransmissionState_t.user_dataToWriteW[1] = (uint8_t)data;

    I2C_irqTransmissionState_t.user_dataToWriteIndexW = 0;

    i2c0_hw->intr_mask |=    I2C_IC_INTR_MASK_M_TX_EMPTY_BITS |
                            I2C_IC_INTR_MASK_M_TX_ABRT_BITS;   

    //i2c0_hw->tar = ADS1115_Address;
    i2c0_hw->data_cmd = I2C_irqTransmissionState_t.user_configRegTypeW;
    I2C_irqTransmissionState_t.user_dataToWriteIndexW++;
}

static void ADS1115_readRegAsync(uint8_t reg_mode, uint16_t *buffer)
{
    I2C_irqTransmissionState_t.I2C_irqReadPhase_t   = I2C_READ_PHASE_WRITE;
    I2C_irqTransmissionState_t.user_configRegTypeR  = reg_mode; 
    I2C_irqTransmissionState_t.user_dataReceivedR   = buffer;
    I2C_irqTransmissionState_t.user_dataReceivedIndexR  = 0;
    I2C_irqTransmissionState_t.user_dataReceivedValid   = false;

    i2c0_hw->intr_mask |=    I2C_IC_INTR_MASK_M_TX_EMPTY_BITS |
                            I2C_IC_INTR_MASK_M_TX_ABRT_BITS |
                            I2C_IC_INTR_MASK_M_RX_FULL_BITS;


    //i2c0_hw->tar = ADS1115_Address;
    i2c0_hw->data_cmd = I2C_irqTransmissionState_t.user_configRegTypeR;
}

static void i2c_irqHandler(void)
{
    if(i2c0_hw->intr_stat & I2C_IC_INTR_STAT_R_TX_ABRT_BITS)
    {
        (void)i2c0_hw->clr_tx_abrt;
        I2C_irqTransmissionState_t.I2C_transmissionError = true;
        return;
    }

    if(i2c0_hw->status & I2C_IC_INTR_STAT_R_TX_EMPTY_BITS) // check if tx buffer is empty
    {
        if(I2C_irqTransmissionState_t.I2C_irqWritePhase_t == I2C_WRITE_PHASE_WRITE)
        {
            //i2c0_hw->tar = ADS1115_Address;
            i2c0_hw->data_cmd = I2C_irqTransmissionState_t.user_dataToWriteW[I2C_irqTransmissionState_t.user_dataToWriteIndexW++];
            
            if(I2C_irqTransmissionState_t.user_dataToWriteIndexW >= 3) // if all data sent
            {
                i2c0_hw->intr_mask &= ~I2C_IC_INTR_MASK_M_TX_EMPTY_BITS;
                I2C_irqTransmissionState_t.I2C_irqWritePhase_t = I2C_WRITE_PHASE_DONE;
            }
        }

        if(I2C_irqTransmissionState_t.I2C_irqReadPhase_t == I2C_READ_PHASE_WRITE)
        {
            I2C_irqTransmissionState_t.I2C_irqReadPhase_t = I2C_READ_PHASE_READ;
            i2c0_hw->intr_mask &= ~I2C_IC_INTR_MASK_M_TX_EMPTY_BITS;
        }
    }

    if (i2c0_hw->status & I2C_IC_STATUS_RFNE_BITS)// check if rx buffer has 1 received byte
    {
        *I2C_irqTransmissionState_t.user_dataReceivedR <<= 8;
        *I2C_irqTransmissionState_t.user_dataReceivedR |= (uint8_t)i2c0_hw->data_cmd;
        I2C_irqTransmissionState_t.user_dataReceivedIndexR++;
        
        if(I2C_irqTransmissionState_t.user_dataReceivedIndexR >= 2)
        {
            i2c0_hw->intr_mask &= ~I2C_IC_INTR_MASK_M_RX_FULL_BITS;
            I2C_irqTransmissionState_t.user_dataReceivedValid   = true;
            I2C_irqTransmissionState_t.I2C_irqReadPhase_t = I2C_READ_PHASE_DONE;
        }
    }
    
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
    ADS1115_state_t.is_convReadyMode  = false;
}

void ADS1115_initWithIrqMode(void)
{
    /// I2C initialization ///
    i2c_init(ADS1115_I2CInstance, 400 * 1000);
    gpio_set_function(ADS1115_SDAPin, GPIO_FUNC_I2C);
    gpio_set_function(ADS1115_SCLPin, GPIO_FUNC_I2C);
    gpio_pull_up(ADS1115_SDAPin);
    gpio_pull_up(ADS1115_SCLPin);
    
    irq_set_exclusive_handler(I2C0_IRQ, i2c_irqHandler);
    irq_set_enabled(I2C0_IRQ, true);
    i2c0_hw->intr_mask =    (I2C_IC_INTR_MASK_M_TX_EMPTY_BITS    | 
                            I2C_IC_INTR_MASK_M_STOP_DET_BITS     | 
                            I2C_IC_INTR_MASK_M_TX_ABRT_BITS      |
                            I2C_IC_INTR_MASK_M_RX_FULL_BITS);

    I2C_irqTransmissionState_t.I2C_irqReadPhase_t = I2C_READ_PHASE_IDLE;
    I2C_irqTransmissionState_t.I2C_irqWritePhase_t = I2C_WRITE_PHASE_IDLE;
    i2c0_hw->tar = ADS1115_Address;

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
    ADS1115_state_t.is_convReadyMode  = false;
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

void ADS1115_setChannelDoubleBuffering(uint8_t channel_number, uint32_t buffer_size, ADS1115_channelConfig *BufferState)
{  
     if(channel_number > 3 || buffer_size == 0)
        return;
    
    if(BufferState == NULL)
        return;

    BufferState->data_counter   = 0;
    BufferState->channel_number = channel_number;
    ring_bufferInit(&BufferState->buffer_0, buffer_size);
    ring_bufferInit(&BufferState->buffer_1, buffer_size);
    BufferState->current_buffer = 0;
}

void ADS1115_doubleBufferingSaveData(ADS1115_channelConfig *buffer_state, uint16_t new_adcSample)
{
    if(buffer_state == NULL)
        return;

    if(buffer_state->current_buffer == 0)
    {
        ring_bufferPush(&buffer_state->buffer_0, new_adcSample);
        buffer_state->data_counter++;
    }
    else
    {
        ring_bufferPush(&buffer_state->buffer_1, new_adcSample);//ADS1115_getSample(buffer_state->channel_number)
        buffer_state->data_counter++;
    }

    
    // Check buffers capacity(and swap if one is full)
    if(buffer_state->data_counter >= buffer_state->buffer_1.buffer_size) // buffer is full
    {
        if (buffer_state->current_buffer == 0)
        {
            //ring_bufferClear(&buffer_state->buffer_1);  // clear buffer
            buffer_state->data_counter = 0;
            buffer_state->current_buffer =  1;          // swap buffer
        }
        else
        {
            //ring_bufferClear(&buffer_state->buffer_0);   // clear buffer
            buffer_state->data_counter = 0;
            buffer_state->current_buffer = 0;            // swap buffer
        }
    }
}

void ADS1115_setModeWithGpioAlert(bool enable, ADS1115_channelConfig *buffer_stateConfig0, ADS1115_channelConfig *buffer_stateConfig1)
{
    if(enable)
    {
        if(buffer_stateConfig0 == NULL || buffer_stateConfig1 == NULL)
            return;

        uint16_t ADS1115_state = 0;
        ADS1115_readReg(ADS1115_LOThreshReg, &ADS1115_state);
        ADS1115_state &= ~(1<<15);
        ADS1115_writeReg(ADS1115_LOThreshReg, ADS1115_state);

        ADS1115_readReg(ADS1115_HiThreshReg, &ADS1115_state);
        ADS1115_state |= (1<<15);
        ADS1115_writeReg(ADS1115_HiThreshReg, ADS1115_state);

        ADS1115_readReg(ADS1115_configReg, &ADS1115_state);
        ADS1115_state &= ~((1<<0) | (1<<1));
        ADS1115_writeReg(ADS1115_configReg, ADS1115_state);

        ADS1115_state_t.is_convReadyMode = true;
        ADS1115_state_t.COMP_QUE_state = ADS1115_compQueAfterOne; 


        gpio_init(ADS1115_alertRdyGpio);
        gpio_set_dir(ADS1115_alertRdyGpio, GPIO_IN);
        gpio_pull_down(ADS1115_alertRdyGpio);
        // todo // gpio_set_irq_enabled_with_callback(ADS1115_alertRdyGpio, GPIO_IRQ_EDGE_RISE, true, ADS1115_convReadyIrq);

        ADS1115_ch0 = buffer_stateConfig0;
        ADS1115_ch1 = buffer_stateConfig1;
    }

    else
    {
        uint16_t ADS1115_state = 0;
        ADS1115_readReg(ADS1115_LOThreshReg, &ADS1115_state);
        ADS1115_state |= (1<<15);
        ADS1115_writeReg(ADS1115_LOThreshReg, ADS1115_state);

        ADS1115_readReg(ADS1115_HiThreshReg, &ADS1115_state);
        ADS1115_state &= ~(1<<15);
        ADS1115_writeReg(ADS1115_HiThreshReg, ADS1115_state);

        ADS1115_readReg(ADS1115_configReg, &ADS1115_state);
        ADS1115_state &= ~((1<<0) | (1<<1));
        ADS1115_writeReg(ADS1115_configReg, ADS1115_state);

        ADS1115_state_t.is_convReadyMode = false;
        ADS1115_state_t.COMP_QUE_state = ADS1115_compQueDisable;
    }

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
        break;

        case(ADS1115_PGA_2):
            voltage = data * 0.000015625f;
        break;

        case(ADS1115_PGA_3):
            voltage = data * 0.00003125f;
        break;

        case(ADS1115_PGA_4):
            voltage = data * 0.0000625f;
        break;

        case(ADS1115_PGA_5):
            voltage = data * 0.000125f;
        break;

        case(ADS1115_PGA_6):
            voltage = data * 0.0001875f;
        break;

        default:
            voltage = 0.0f;
    }
    
    return voltage;
}
