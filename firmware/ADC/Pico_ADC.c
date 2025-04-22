#include "Pico_ADC.h"

void ADC_PicoInit(uint8_t ADC_channel)
{
    if(ADC_channel > 3 || ADC_channel < 0)
        return;

    adc_init();
    adc_gpio_init(ADC_channel);

    uint robin_mask = 0;

    switch(ADC_channel)
    {
        case Pico_ADCChannel_0:
            robin_mask = (1<<0); // Set ADC0 in Robin mode
        break;

        case Pico_ADCChannel_1:
            robin_mask = (1<<1); // Set ADC1 in Robin mode
        break;

        case Pico_ADCChannel_2:
            robin_mask = (1<<2); // Set ADC2 in Robin mode
        break;

        default:
            robin_mask = (1 << 2) | (1 << 1) | (1 << 0); // set all channels in robin mode
    }

    adc_set_round_robin(robin_mask);

}


void ADC_PicoSetDualChannelRoutine(uint8_t first_channel, uint8_t second_channel, ring_buffer *data_buffer1, ring_buffer *data_buffer2)
{
    if(first_channel < 0 || first_channel > 3 || second_channel < 0 || second_channel > 3)
        return;

    


}