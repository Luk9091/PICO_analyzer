#include "ADC_bootStrap.h"
#include "ADS1115.h"
#include "Pico_ADC.h"

#include <pico/stdlib.h>
#include <hardware/timer.h>

static bool ADC_ADS1115Callback(struct repeating_timer *t);
static bool ADC_PicoStandardModeIrq(struct repeating_timer *t);

static ADS1115_doubleBufferState ADS1115BufferState_1 = {0};
static ADS1115_doubleBufferState ADS1115BufferState_2 = {0};
static Pico_ADCBufferState pico_bufferState_1 = {0};
static Pico_ADCBufferState pico_bufferState_2 = {0};

void ADC_bootStrap(void)
{
    static struct repeating_timer ADC_ADS1115Timer;
    static struct repeating_timer ADC_PicoTimer;

    ADS1115_init();
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_0, ADC_ADS1115SampleNumber, &ADS1115BufferState_1);
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_1, ADC_ADS1115SampleNumber, &ADS1115BufferState_2);
    add_repeating_timer_ms(-50, ADC_ADS1115Callback, NULL, &ADC_ADS1115Timer);      // Low freq. ADC <-> ADS1115
    

    ADC_PicoStandardModeInit(ADC_PicoChannel_0, ADC_PicoSampleNumber, &pico_bufferState_1);
    ADC_PicoStandardModeInit(ADC_PicoChannel_1, ADC_PicoSampleNumber, &pico_bufferState_2);
    add_repeating_timer_ms(-1, ADC_PicoStandardModeIrq, NULL, &ADC_PicoTimer);
}

static bool ADC_ADS1115Callback(struct repeating_timer *t)
{
    ADS1115_doubleBufferingCallback(&ADS1115BufferState_1);
    ADS1115_doubleBufferingCallback(&ADS1115BufferState_2);

    return true;
}

static bool ADC_PicoStandardModeIrq(struct repeating_timer *t)
{
    ADC_PicoStandardModeCallback(&pico_bufferState_1);
    ADC_PicoStandardModeCallback(&pico_bufferState_2);

    return true;
}

