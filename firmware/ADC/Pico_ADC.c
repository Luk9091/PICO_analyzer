#include "Pico_ADC.h"

static Pico_adcDmaModeState DMA_state_t = {0};
static void DMA_handler(void);
uint64_t timer = 0;

void ADC_PicoStandardModeInit(uint8_t channel_number, uint32_t buffer_size, Pico_adcStandardMode *buffer_state)
{
    adc_init();
    adc_gpio_init(channel_number);
    //adc_set_clkdiv(ADC_PicoADCClkDiv); //5kHz sampling rate

    if(channel_number < 0 || channel_number > 4)
        return;

    if(buffer_size <= 0)
        return;

    buffer_state->channel_number = channel_number;
    ring_bufferInit(buffer_state->buffer_1, buffer_size);
    ring_bufferInit(buffer_state->buffer_2, buffer_size);
    buffer_state->current_buffer = buffer_state->buffer_1; 
    adc_select_input(buffer_state->channel_number);
}

void ADC_PicoStandardModeCallback(Pico_adcStandardMode *buffer_state)
{
    if(buffer_state == NULL)
        return;

    adc_select_input(buffer_state->channel_number);
    ring_bufferPush(buffer_state->current_buffer, adc_read());

    if(ring_bufferGetCapacity(buffer_state->current_buffer) >= buffer_state->current_buffer->buffer_size)
    {
        if(buffer_state->current_buffer == buffer_state->buffer_1)
           buffer_state->current_buffer = buffer_state->buffer_2;
        
        else
            buffer_state->current_buffer = buffer_state->buffer_1;

    }
}

void ADC_PicoDMAModeInit(void)
{
    adc_gpio_init(ADC_PicoPinCh_0);
    adc_gpio_init(ADC_PicoPinCh_1);
    adc_init();
    adc_set_round_robin((1 << ADC_PicoChannel_0) | (1 << ADC_PicoChannel_1));
    adc_fifo_setup(true, true, 1, false, false);
    adc_set_clkdiv(25000); // 5kHz sampling rate
    adc_fifo_drain();

    //irq_set_priority(DMA_IRQ_1, 5);
    //irq_set_priority(I2C0_IRQ, 10);
    DMA_state_t.current_buffer = 0;     // begin with buffer 0

    DMA_state_t.DMA_channel = dma_claim_unused_channel(true);
    DMA_state_t.DMA_cfg = dma_channel_get_default_config(DMA_state_t.DMA_channel);
    channel_config_set_transfer_data_size(&DMA_state_t.DMA_cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&DMA_state_t.DMA_cfg, false);
    channel_config_set_write_increment(&DMA_state_t.DMA_cfg, true);
    channel_config_set_dreq(&DMA_state_t.DMA_cfg, DREQ_ADC);
    dma_channel_configure(DMA_state_t.DMA_channel, &DMA_state_t.DMA_cfg, DMA_state_t.ADC_buffer_1, &adc_hw->fifo, ADC_PicoSampleNumber, false);
    dma_channel_set_irq1_enabled(DMA_state_t.DMA_channel, true);

    irq_set_exclusive_handler(DMA_IRQ_1, DMA_handler);
    irq_set_enabled(DMA_IRQ_1, true);
    adc_run(true);
    dma_channel_start(DMA_state_t.DMA_channel);
}

static void DMA_handler(void)
{     
    uint32_t status = dma_hw->ints1;
    if (!(status & (1u << DMA_state_t.DMA_channel)))
        return;
    dma_hw->ints1 = (1u << DMA_state_t.DMA_channel);
    
    if(DMA_state_t.current_buffer == 0)
    {
        dma_channel_set_write_addr(DMA_state_t.DMA_channel, DMA_state_t.ADC_buffer_2, true);
        DMA_state_t.current_buffer = 1;
    }
    else
    {
        dma_channel_set_write_addr(DMA_state_t.DMA_channel, DMA_state_t.ADC_buffer_1, true);
        DMA_state_t.current_buffer = 0;
    }
}

uint16_t *ADC_PicoDMAModeGetData(void)
{
    uint16_t *buffer = NULL;
    switch(DMA_state_t.current_buffer)
    {
        case(0):
            buffer = DMA_state_t.ADC_buffer_2; 
        break;

        case(1):
            buffer = DMA_state_t.ADC_buffer_1;
        break;
    }
    return buffer;
}