#include "ADC_bootStrap.h"
#include "ADS1115.h"
#include "Pico_ADC.h"

#include <pico/stdlib.h>
#include <hardware/timer.h>

static bool ADC_ADS1115Callback(struct repeating_timer *t);

static ADS1115_doubleBufferState ADS1115BufferState_1 = {0};
static ADS1115_doubleBufferState ADS1115BufferState_2 = {0};
// static ring_buffer ADC_PicoBuffer_1    = {0};
// static ring_buffer ADC_PicoBuffer_2    = {0};

void ADC_bootStrap(void)
{
    struct repeating_timer timer;

    // ADS1115 initialization - double buffering mode
    ADS1115_init();
    ADS1115_doubleBufferingInit(ADS1115_channel_0, ADC_ADS1115SampleNumber, &ADS1115BufferState_1);
    ADS1115_doubleBufferingInit(ADS1115_channel_1, ADC_ADS1115SampleNumber, &ADS1115BufferState_2);



    //ADC_PicoInit(channel_);

    add_repeating_timer_ms(-2, ADC_ADS1115Callback, NULL, &timer);   // Low freq. ADC <-> ADS1115
    //add_repeating_timer_us(-10, Pico_timerCallback, NULL, &timer);        // High freq. ADC <-> Pi Pico ADC
}


static bool ADC_ADS1115Callback(struct repeating_timer *t)
{
    ADS1115_measureRoutineCallback(ADS1115_channel_0, &ADS1115BufferState_1);
    ADS1115_measureRoutineCallback(ADS1115_channel_1, &ADS1115BufferState_2);

    if(ADS1115BufferState_1.buffer_1->counter == ADC_ADS1115SampleNumber - 1)
    {
        printf("Your data has to be sent");
        // TODO -> DATA SEND ROUTINE
    }

    return true;
}

