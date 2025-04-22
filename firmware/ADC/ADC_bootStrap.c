#include "ADC_bootStrap.h"
#include "ADS1115.h"
#include "Pico_ADC.h"

#include <pico/stdlib.h>
#include <hardware/timer.h>

static bool ADC_ADS1115TimerCallback(struct repeating_timer *t);

static ring_buffer ADC_ADS1115Buffer_0 = {0};
static ring_buffer ADC_ADS1115Buffer_1 = {0};
static ring_buffer ADC_PicoBuffer_0    = {0};
static ring_buffer ADC_PicoBuffer_1    = {0};

void ADC_bootStrap(void)
{
    struct repeating_timer timer;

    ring_bufferInit(&ADC_ADS1115Buffer_0, ADC_ADS1115SampleNumber);
    ring_bufferInit(&ADC_ADS1115Buffer_1, ADC_ADS1115SampleNumber);
    ring_bufferInit(&ADC_PicoBuffer_0, ADC_PicoSampleNumber);
    ring_bufferInit(&ADC_PicoBuffer_1, ADC_PicoSampleNumber);

    ADS1115_init();
    //ADC_PicoInit(channel_);

    add_repeating_timer_ms(-2, ADC_ADS1115TimerCallback, NULL, &timer);   // Low freq. ADC <-> ADS1115
    //add_repeating_timer_us(-10, Pico_timerCallback, NULL, &timer);        // High freq. ADC <-> Pi Pico ADC
}


static bool ADC_ADS1115TimerCallback(struct repeating_timer *t)
{
    ADS1115_setDualChannelRoutine(ADS1115_channel_0, ADS1115_channel_1, &ADC_ADS1115Buffer_0, &ADC_ADS1115Buffer_1);

    if(ADC_ADS1115Buffer_0.counter == ADC_ADS1115SampleNumber - 1)
    {
        printf("Your data has to be sent");
        // TODO -> DATA SEND ROUTINE
    }

    return true;
}

