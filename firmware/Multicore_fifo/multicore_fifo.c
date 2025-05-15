#include "multicore_fifo.h"

static void update_deviceConfigStatus(uint8_t received_tag);
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
      irq_set_exclusive_handler(SIO_FIFO_IRQ_NUM(0), core0_sio_irq);
    irq_set_enabled(SIO_FIFO_IRQ_NUM(0), true);

    sleep_ms(25);
}

static void core0_sio_irq(void)
{
    uint32_t core0_rx_val = 0;
    while (multicore_fifo_rvalid()) // fifo always contains only one 32-bit frame
        core0_rx_val = multicore_fifo_pop_blocking();

    uint8_t config_data = (core0_rx_val & 0x000fff00) >> 8;
    
    if(core0_rx_val & (1<<20))  //received config. frame AND data frame
    {
        update_deviceConfigStatus(config_data);
        /// @todo received data
    }
    
    else if(core0_rx_val & (1<<21))  //received only config. frame
        update_deviceConfigStatus(config_data);


    else if(core0_rx_val & (1<<22))  //received only data frame
        /// @todo received data
    
    multicore_fifo_clear_irq();
}

static void core1_sio_irq(void)
{
    uint32_t core1_rx_val = 0;
    while (multicore_fifo_rvalid()) // fifo always contains only one 32-bit frame
        core1_rx_val = multicore_fifo_pop_blocking();

    multicore_fifo_clear_irq();
}

static void update_deviceConfigStatus(uint8_t received_tag)
{
    if(received_tag &(1<<ADC_ADS1115_CH1_ENABLE))
        device_status.ADC_ADS1115_CH1_ENABLE = 1;
    else
        device_status.ADC_ADS1115_CH1_ENABLE = 0;


    if(received_tag &(1<<ADC_ADS1115_CH2_ENABLE))
        device_status.ADC_ADS1115_CH2_ENABLE = 1;
    else
        device_status.ADC_ADS1115_CH2_ENABLE = 0;


    if(received_tag &(1<<ADC_PICO_CH1_ENABLE))
        device_status.ADC_PICO_CH1_ENABLE = 1;
    else
        device_status.ADC_PICO_CH1_ENABLE = 0;


    if(received_tag &(1<<ADC_PICO_CH2_ENABLE))
        device_status.ADC_PICO_CH2_ENABLE = 1;
    else
        device_status.ADC_PICO_CH2_ENABLE = 0;


    if(received_tag &(1<<DIGITAL_PROBE_ENABLE))
        device_status.DIGITAL_PROBE_ENABLE = 1;
    else
        device_status.DIGITAL_PROBE_ENABLE = 0;


    if(received_tag &(1<<DIGITAL_MODE_FREERUN))
        device_status.DIGITAL_MODE_FREERUN = 1;
    else
        device_status.DIGITAL_MODE_FREERUN = 0;

        
    if(received_tag &(1<<DIGITAL_MODE_TRIGGERED))
        device_status.DIGITAL_MODE_TRIGGERED = 1;
    else
        device_status.DIGITAL_MODE_TRIGGERED = 0;


    if(received_tag &(1<<DIGITAL_SET_SAMPLE_FREQ))
        device_status.DIGITAL_SET_SAMPLE_FREQ = 1;
    else
        device_status.DIGITAL_SET_SAMPLE_FREQ = 0;


    if(received_tag &(1<<DIGITAL_SET_TIMER))
        device_status.DIGITAL_SET_TIMER = 1;
    else
        device_status.DIGITAL_SET_TIMER = 0;
    
}

bool multicore_fifoTryPushCore0(uint8_t buffer_size, core0_validBufferNumber buffer_number)
{
    uint32_t buffer_size_t = (uint32_t)buffer_size;
    uint32_t buffer_number_t = (uint32_t)buffer_number; buffer_number = (buffer_number<<8);
    uint32_t fifo_frame = (buffer_size_t | buffer_number_t);
    multicore_fifo_push_blocking_inline(fifo_frame);


    /// @todo exception's needed(multicore_fifo_push_blocking_inline doesn't return any data)
    return true;
}

bool multicore_fifoTryPushCore1(fifo_frameType_t frame_type, device_configStatus_t *new_deviceConfig, device_configStatus_t *current_deviceConfig)
{
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


    /// SET FIFO VALID ADC BUFFER'S
    if(current_deviceConfig->ADC_ADS1115_CH1_ENABLE)
        fifo_frame |= (1<<3);
    if(current_deviceConfig->ADC_ADS1115_CH2_ENABLE)
        fifo_frame |= (1<<2);
    if(current_deviceConfig->ADC_PICO_CH1_ENABLE)
        fifo_frame |= (1<<1);
    if(current_deviceConfig->ADC_PICO_CH2_ENABLE)
        fifo_frame |= (1<<0);   


    return true;
}