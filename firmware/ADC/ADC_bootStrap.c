#include "ADC_bootStrap.h"

static bool ADC_ADS1115Callback(struct repeating_timer *t);
static bool ADC_PicoStandardModeIrq(struct repeating_timer *t);

static ADS1115_doubleBufferState ADS1115BufferState_1 = {0};
static ADS1115_doubleBufferState ADS1115BufferState_2 = {0};
//static Pico_adcStandardMode pico_bufferState_1 = {0};
//static Pico_adcStandardMode pico_bufferState_2 = {0};

void ADC_bootStrap(void)
{
    static struct repeating_timer ADC_ADS1115Timer;

    //ADS1115_init();
    //ADS1115_setChannelDoubleBuffering(ADS1115_channel_0, ADC_ADS1115SampleNumber, &ADS1115BufferState_1);
    //ADS1115_setChannelDoubleBuffering(ADS1115_channel_1, ADC_ADS1115SampleNumber, &ADS1115BufferState_2);
    //add_repeating_timer_ms(-10, ADC_ADS1115Callback, NULL, &ADC_ADS1115Timer);      // Low freq. ADC <-> ADS1115
    
    ADC_PicoDMAModeInit();
}

static bool ADC_ADS1115Callback(struct repeating_timer *t)
{
    ADS1115_doubleBufferingCallback(&ADS1115BufferState_1);
    ADS1115_doubleBufferingCallback(&ADS1115BufferState_2);

    return true;
}

static bool ADC_PicoStandardModeIrq(struct repeating_timer *t)
{
    //ADC_PicoStandardModeCallback(&pico_bufferState_1);
    //ADC_PicoStandardModeCallback(&pico_bufferState_2);

    return true;
}

void ADS1115_ADCGetData(uint8_t channel_number, ADS1115_doubleBufferState *buffer)
{
    switch(channel_number)
    {
        case(ADS1115_channel_0): 
            buffer = &ADS1115BufferState_1;
        break;

        case(ADS1115_channel_1): 
            buffer = &ADS1115BufferState_2;
        break;

        default:
            buffer = NULL;
    }
}

