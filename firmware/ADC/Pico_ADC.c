#include "Pico_ADC.h"

static uint16_t Pico_DMAModeDataCh1[ADC_PicoSampleNumber] = {0};
static uint16_t Pico_DMAModeDataCh2[ADC_PicoSampleNumber] = {0};
static DMA_state DMA_state_t = {0};


static void DMA_handler(void);

///@warning IT is strongly recommended NOT TO USE ADC_PicoInit
void ADC_PicoInit(uint8_t ADC_channel)
{
    if(ADC_channel > 3 || ADC_channel < 0)
        return;

    adc_init();
    adc_gpio_init(ADC_channel);
    adc_set_clkdiv(1 + 499); //set clk freq. = 1000kHz

    uint robin_mask = 0;

    switch(ADC_channel)
    {
        case ADC_PicoChannel_0:
            robin_mask = (1<<0); // Set ADC0 in Robin mode
        break;

        case ADC_PicoChannel_1:
            robin_mask = (1<<1); // Set ADC1 in Robin mode
        break;

        case ADC_PicoChannel_2:
            robin_mask = (1<<2); // Set ADC2 in Robin mode
        break;

        default:
            robin_mask = (1 << 2) | (1 << 1) | (1 << 0); // set all channels in robin mode
    }

    adc_set_round_robin(robin_mask);

}

void ADC_PicoStandardModeInit(uint8_t channel_number, uint32_t buffer_size, Pico_adcStandardMode *buffer_state)
{
    adc_init();
    adc_gpio_init(channel_number);
    adc_set_clkdiv(500.0f); //1kHz sampling rate

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

    //adc_select_input(buffer_state->channel_number);
    ring_bufferPush(buffer_state->current_buffer, adc_read());

    if(ring_bufferGetCapacity(buffer_state->current_buffer) >= buffer_state->current_buffer->buffer_size)
    {
        if(buffer_state->current_buffer ==   buffer_state->buffer_1)
        {
           buffer_state->current_buffer =    buffer_state->buffer_2;
           adc_select_input(buffer_state->channel_number);
        }
        else
        {
            buffer_state->current_buffer = buffer_state->buffer_1;
            adc_select_input(buffer_state->channel_number);
        }
    }
}

void ADC_PicoDMAModeInit(void)
{
    adc_init();
    adc_gpio_init(ADC_PicoChannel_0);
    adc_gpio_init(ADC_PicoChannel_1);
    adc_set_clkdiv(500.0f); // 1kHz sampling rate
    adc_set_round_robin((1 << ADC_PicoChannel_0) | (1 << ADC_PicoChannel_1));
    adc_fifo_setup(true, true, 1, false, false);
    adc_fifo_drain();

    DMA_state_t.current_buffer = 0; // begin with buffer 0
    DMA_state_t.DMA_channel = dma_claim_unused_channel(true);
    DMA_state_t.DMA_cfg = dma_channel_get_default_config(DMA_state_t.DMA_channel);
    channel_config_set_transfer_data_size(&DMA_state_t.DMA_cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&DMA_state_t.DMA_cfg, false);
    channel_config_set_write_increment(&DMA_state_t.DMA_cfg, true);
    channel_config_set_dreq(&DMA_state_t.DMA_cfg, DREQ_ADC);
    dma_channel_configure(DMA_state_t.DMA_channel, &DMA_state_t.DMA_cfg, DMA_state_t.DMA_buffer_1, &adc_hw->fifo, 2 * ADC_PicoSampleNumber, false);
    dma_channel_set_irq1_enabled(DMA_state_t.DMA_channel, true);

    irq_set_exclusive_handler(DMA_IRQ_1, DMA_handler);
    irq_set_enabled(DMA_IRQ_1, true);
    adc_run(true);
    dma_channel_start(DMA_state_t.DMA_channel);
}

static void DMA_handler(void)
{
    uint32_t status = dma_hw->ints1; 
    dma_hw->ints1 = status; 
    

    if(DMA_state_t.current_buffer == 0)
    {
        dma_channel_set_write_addr(DMA_state_t.DMA_channel, DMA_state_t.DMA_buffer_2, true);
        DMA_state_t.current_buffer = 1;

        for (int i = 0; i < ADC_PicoSampleNumber; i++)
        {
            Pico_DMAModeDataCh1[i] = DMA_state_t.DMA_buffer_1[2*i];     
            Pico_DMAModeDataCh2[i] = DMA_state_t.DMA_buffer_1[2*i + 1];   
        }
    }
    else
    {
        dma_channel_set_write_addr(DMA_state_t.DMA_channel, DMA_state_t.DMA_buffer_1, true);
        DMA_state_t.current_buffer = 0;

        for (int i = 0; i < ADC_PicoSampleNumber; i++)
        {
            Pico_DMAModeDataCh1[i] = DMA_state_t.DMA_buffer_1[2*i];     
            Pico_DMAModeDataCh2[i] = DMA_state_t.DMA_buffer_1[2*i + 1];   
        }
    }
}

uint16_t *Pico_ADCGetData(uint8_t channel_number)
{
    switch(channel_number)
    {
        case(ADC_PicoChannel_0): 
            return Pico_DMAModeDataCh1;
        break;

        case(ADC_PicoChannel_1): 
            return Pico_DMAModeDataCh2;
        break;

        default:
            return NULL;
    }
}

