#include "Pico_ADC.h"

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

void ADC_PicoStandardModeInit(uint8_t channel_number, uint32_t buffer_size, Pico_ADCBufferState *buffer_state)
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
}

void ADC_PicoStandardModeCallback(Pico_ADCBufferState *buffer_state)
{
    if(buffer_state == NULL)
        return;

    adc_select_input(buffer_state->channel_number);
    ring_bufferPush(buffer_state->current_buffer, adc_read());

    if(ring_bufferGetCapacity(buffer_state->current_buffer) >= buffer_state->current_buffer->buffer_size)
    {
        if(buffer_state->current_buffer ==   buffer_state->buffer_1)
           buffer_state->current_buffer =    buffer_state->buffer_2;
        else
            buffer_state->current_buffer = buffer_state->buffer_1;
    }
}

