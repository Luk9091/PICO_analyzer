#include "multicore_fifo.h"

static void updata_deviceConfigStatus(uint8_t received_tag);

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


void core0_sio_irq(void)
{
    uint32_t core0_rx_val = 0;
    while (multicore_fifo_rvalid()) // fifo always contains only one 32-bit frame
        core0_rx_val = multicore_fifo_pop_blocking();

    uint8_t config_data = (core0_rx_val & 0x0000ff00);
    config_data = config_data>>8;
    
    if(core0_rx_val & (1<<16))  //received config. frame AND data frame
    {
        updata_deviceConfigStatus(config_data);
        /// @todo received data
    }
    
    else if(core0_rx_val & (1<<17))  //received only config. frame
        updata_deviceConfigStatus(config_data);


    else if(core0_rx_val & (1<<18))  //received only data frame
        /// @todo received data
    
    multicore_fifo_clear_irq();
}

static void updata_deviceConfigStatus(uint8_t received_tag)
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












void core1_sio_irq(void)
{
    uint32_t core1_rx_val = 0;
    while (multicore_fifo_rvalid()) // fifo always contains only one 32-bit frame
        core1_rx_val = multicore_fifo_pop_blocking();

    multicore_fifo_clear_irq();
}