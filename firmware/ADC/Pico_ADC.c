#include "Pico_ADC.h"

void Pico_ADCInit(uint8_t ADC_pin0, uint8_t ADC_pin1)
{
    if(ADC_pin0 != 0 && ADC_pin0 != 1 && ADC_pin0 != 2 && ADC_pin0 != 3)
        return;
    if(ADC_pin1 != 0 && ADC_pin1 != 1 && ADC_pin1 != 2 && ADC_pin1 != 3)
        return;


    adc_init();
    adc_gpio_init(ADC_pin0);
    adc_gpio_init(ADC_pin1);

    uint robin_mask = (1<<0) | (1<<1); // Set ADC0 and ADC1 in Robin mode
    adc_set_round_robin(robin_mask);
}