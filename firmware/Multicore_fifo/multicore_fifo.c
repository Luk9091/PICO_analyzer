#include "multicore_fifo.h"

static void update_deviceConfigStatus(uint32_t received_tag);
static void core0_sio_irq(void);
static void core1_sio_irq(void);

device_configStatus_t device_status = {0};

void multicore_fifoInitCore0(void)
{
    irq_set_exclusive_handler(SIO_FIFO_IRQ_NUM(0), core0_sio_irq);
    irq_set_enabled(SIO_FIFO_IRQ_NUM(0), true);

    sleep_ms(25);
}

void multicore_fifoInitCore1(void)
{
    irq_set_exclusive_handler(SIO_FIFO_IRQ_NUM(1), core1_sio_irq);
    irq_set_enabled(SIO_FIFO_IRQ_NUM(1), true);

    sleep_ms(25);
}

static void core0_sio_irq(void)
{
    uint32_t core0_rx_val = 0;
    while (multicore_fifo_rvalid()) // fifo always contains only one 32-bit frame
        core0_rx_val = multicore_fifo_pop_blocking();
 
    if(core0_rx_val & (1<<20))  //received config. frame AND data frame <-> currently NOT USED
    {
        update_deviceConfigStatus(core0_rx_val); 
        /// @todo received data
    }
    
    else if(core0_rx_val & (1<<21))  //received only config. frame
    {
        update_deviceConfigStatus(core0_rx_val);
    }

    else if(core0_rx_val & (1<<22))  //received only data frame
    {
        /// @todo received data
    }

    
    multicore_fifo_clear_irq();
}

static void core1_sio_irq(void)
{
    uint32_t core1_rx_val = 0;
    while (multicore_fifo_rvalid()) // fifo always contains only one 32-bit frame
        core1_rx_val = multicore_fifo_pop_blocking();

    multicore_fifo_clear_irq();
}

static void update_deviceConfigStatus(uint32_t frame)
{
    if(frame &(1<<ADC_ADS1115_CH1_ENABLE))
        device_status.ADC_ADS1115_CH1_ENABLE = 1;
    else
        device_status.ADC_ADS1115_CH1_ENABLE = 0;


    if(frame &(1<<ADC_ADS1115_CH2_ENABLE))
        device_status.ADC_ADS1115_CH2_ENABLE = 1;
    else
        device_status.ADC_ADS1115_CH2_ENABLE = 0;


    if(frame &(1<<ADC_PICO_CH1_ENABLE))
        device_status.ADC_PICO_CH1_ENABLE = 1;
    else
        device_status.ADC_PICO_CH1_ENABLE = 0;


    if(frame &(1<<ADC_PICO_CH2_ENABLE))
        device_status.ADC_PICO_CH2_ENABLE = 1;
    else
        device_status.ADC_PICO_CH2_ENABLE = 0;


    if(frame &(1<<DIGITAL_PROBE_ENABLE))
        device_status.DIGITAL_PROBE_ENABLE = 1;
    else
        device_status.DIGITAL_PROBE_ENABLE = 0;


    if(frame &(1<<DIGITAL_MODE_FREERUN))
        device_status.DIGITAL_MODE_FREERUN = 1;
    else
        device_status.DIGITAL_MODE_FREERUN = 0;

        
    if(frame &(1<<DIGITAL_MODE_TRIGGERED))
        device_status.DIGITAL_MODE_TRIGGERED = 1;
    else
        device_status.DIGITAL_MODE_TRIGGERED = 0;


    if(frame &(1<<DIGITAL_SET_SAMPLE_FREQ))
        device_status.DIGITAL_SET_SAMPLE_FREQ = 1;
    else
        device_status.DIGITAL_SET_SAMPLE_FREQ = 0;


    if(frame &(1<<DIGITAL_SET_TIMER))
        device_status.DIGITAL_SET_TIMER = 1;
    else
        device_status.DIGITAL_SET_TIMER = 0;
    
}

bool multicore_fifoTryPushCore0(uint8_t buffer_size, core0_validBufferNumber buffer_number)
{
    if(buffer_size == 0 || buffer_number > 4)
        return false;

    uint32_t buffer_size_t = (uint32_t)buffer_size;
    uint32_t buffer_number_t = (uint32_t)buffer_number; 
    buffer_number_t <<= 8;//= (buffer_number_t << 8);

    uint32_t fifo_frame = (buffer_size_t | buffer_number_t);
    multicore_fifo_push_blocking_inline(fifo_frame);


    /// @todo exception's needed(multicore_fifo_push_blocking_inline doesn't return any data)
    return true;
}

bool multicore_fifoTryPushCore1(fifo_frameType_t frame_type, device_configStatus_t *new_deviceConfig, ADC_validBufferStatus *ADC_bufferStatus)
{
    if(new_deviceConfig == NULL && ADC_bufferStatus == NULL)
        return false;
        
    uint32_t fifo_frame = 0;

    /// SET FIFO FRAME TYPE
    switch(frame_type)
    {
        case(FIFO_FRAME_CONFIG):
            fifo_frame |= (1<<20);
        break;

        case(FIFO_FRAME_DATA):
            fifo_frame |= (1<<21);
        break;    

        case(FIFO_FRAME_DATA_CONFIG):
            fifo_frame |= (1<<22);
        break;
    }


    if(new_deviceConfig != NULL)
    {
        /// SET FIFO NEW DEVICE CONFIGURATION(RECEIVED VIA WIFI)
        if(new_deviceConfig->ADC_ADS1115_CH1_ENABLE)
            fifo_frame |= (1<<18);
        if(new_deviceConfig->ADC_ADS1115_CH2_ENABLE)
            fifo_frame |= (1<<17);
        if(new_deviceConfig->ADC_PICO_CH1_ENABLE)
            fifo_frame |= (1<<16);
        if(new_deviceConfig->ADC_PICO_CH2_ENABLE)
            fifo_frame |= (1<<15);
        if(new_deviceConfig->DIGITAL_PROBE_ENABLE)
            fifo_frame |= (1<<14);
        if(new_deviceConfig->DIGITAL_MODE_FREERUN)
            fifo_frame |= (1<<13);
        if(new_deviceConfig->DIGITAL_MODE_TRIGGERED)
            fifo_frame |= (1<<12);
        if(new_deviceConfig->DIGITAL_SET_SAMPLE_FREQ)
            fifo_frame |= (1<<11);
        if(new_deviceConfig->DIGITAL_SET_TIMER)
            fifo_frame |= (1<<10);
        if(new_deviceConfig->DEVICE_STOP)
            fifo_frame |= (1<<9);
        if(new_deviceConfig->DEVICE_RUN)
            fifo_frame |= (1<<8);
    }   

    /// SET FIFO VALID ADC BUFFER'S
    if(ADC_bufferStatus != NULL)
    {
        if(ADC_bufferStatus->ADC_ADS1115_CH1_VALID)
        fifo_frame |= (1<<3);
        if(ADC_bufferStatus->ADC_ADS1115_CH2_VALID)
        fifo_frame |= (1<<2);
        if(ADC_bufferStatus->ADC_PICO_CH1_VALID)
        fifo_frame |= (1<<1);
        if(ADC_bufferStatus->ADC_PICO_CH2_VALID)
        fifo_frame |= (1<<0);   
    }

    multicore_fifo_push_blocking_inline(fifo_frame);

    return true;
}