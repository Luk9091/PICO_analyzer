#include "Pico_ADC.h"

static Pico_adcChannelConfigDmaMode DMA_state = {0};
static void ADC_PicoDmaHandler(void);


void ADC_PicoStandardModeInit(uint8_t channel_number, uint32_t buffer_size, Pico_adcChannelConfig *buffer_state)
{
    if(channel_number > 3)
        return;

    if(buffer_size == 0)
        return;

    adc_init();
    adc_gpio_init(ADC_picoGetChannelPin(channel_number));

    buffer_state->data_counter = 0;
    buffer_state->channel_number = channel_number;
    ring_bufferInit(&buffer_state->buffer_0, buffer_size);
    ring_bufferInit(&buffer_state->buffer_1, buffer_size);
    buffer_state->current_buffer = 0; // begin with buffer 0
}

void ADC_PicoStandardModeCallback(Pico_adcChannelConfig *buffer_state)
{
    if(buffer_state == NULL)
        return;

    //adc_set_clkdiv(0);
    adc_select_input(buffer_state->channel_number);
    
    switch(buffer_state->current_buffer)
    {
        case(0):   
            ring_bufferPush(&buffer_state->buffer_0, adc_read());
            buffer_state->data_counter++;
        break;

        case(1):
            ring_bufferPush(&buffer_state->buffer_1, adc_read());
            buffer_state->data_counter++;
        break;


        default:
            return;
    }

   if(buffer_state->data_counter >= buffer_state->buffer_1.buffer_size) // buffer is full
    {
        if (buffer_state->current_buffer == 0)
        {
            //ring_bufferClear(&buffer_state->buffer_1);    // clear buffer
            buffer_state->data_counter = 0;
            buffer_state->current_buffer =  1;              // swap buffer
        }
        else
        {
            //ring_bufferClear(&buffer_state->buffer_0);    // clear buffer
            buffer_state->data_counter = 0;
            buffer_state->current_buffer = 0;               // swap buffer
        }
    }
}

void ADC_PicoDmaModeInit(void)
{
    adc_gpio_init(ADC_PicoPinCh_0);
    adc_gpio_init(ADC_PicoPinCh_1);
    adc_init();
    adc_set_round_robin((1 << ADC_PicoChannel_0) | (1 << ADC_PicoChannel_1));
    adc_fifo_setup(true, true, 1, false, false);
    adc_set_clkdiv(100); // 5kHz sampling rate
    adc_fifo_drain();

    DMA_state.current_buffer = 0;     // begin with buffer 0

    DMA_state.DMA_channel = dma_claim_unused_channel(true);
    DMA_state.DMA_cfg = dma_channel_get_default_config(DMA_state.DMA_channel);
    channel_config_set_transfer_data_size(&DMA_state.DMA_cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&DMA_state.DMA_cfg, false);
    channel_config_set_write_increment(&DMA_state.DMA_cfg, true);
    channel_config_set_dreq(&DMA_state.DMA_cfg, DREQ_ADC);
    dma_channel_configure(DMA_state.DMA_channel, &DMA_state.DMA_cfg, DMA_state.ADC_buffer_0, &adc_hw->fifo, ADC_PicoSampleNumber, false);
    dma_channel_set_irq1_enabled(DMA_state.DMA_channel, true);

    irq_set_exclusive_handler(DMA_IRQ_1, ADC_PicoDmaHandler);
    irq_set_enabled(DMA_IRQ_1, true);
    adc_run(true);
    dma_channel_start(DMA_state.DMA_channel);
}

static void ADC_PicoDmaHandler(void)
{     
    uint32_t status = dma_hw->ints1;
    if (!(status & (1u << DMA_state.DMA_channel)))
        return;
    dma_hw->ints1 = (1u << DMA_state.DMA_channel);
    
    if(DMA_state.current_buffer == 0)
    {
        dma_channel_set_write_addr(DMA_state.DMA_channel, DMA_state.ADC_buffer_1, true);
        DMA_state.current_buffer = 1;
    }
    else
    {
        dma_channel_set_write_addr(DMA_state.DMA_channel, DMA_state.ADC_buffer_0, true);
        DMA_state.current_buffer = 0;
    }
}

uint16_t *ADC_PicoDmaModeGetData(void)
{
    uint16_t *buffer = NULL;
    switch(DMA_state.current_buffer)
    {
        case(0):
            buffer = DMA_state.ADC_buffer_1; 
        break;

        case(1):
            buffer = DMA_state.ADC_buffer_0;
        break;
    }
    return buffer;
}