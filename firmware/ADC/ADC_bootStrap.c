#include "ADC_bootStrap.h"

static bool ADC_DMAModeIrq(struct repeating_timer *t);
static bool ADC_standardModeIrq(struct repeating_timer *t);

static ADS1115_doubleBufferState ADS1115_ch0 = {0};
static ADS1115_doubleBufferState ADS1115_ch1 = {0};
static Pico_adcStandardMode ADC_picoCh1 = {0};
static Pico_adcStandardMode ADC_picoCh2 = {0};

void ADC_bootStrap(void)
{
    static repeating_timer_t ADC_irqTimer;     

    /// ADS1115 initialization
    ADS1115_init();
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_0, ADC_ADS1115SampleNumber, &ADS1115_ch0);
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_1, ADC_ADS1115SampleNumber, &ADS1115_ch1);

    /// PI Pico(embedded) ADC initialization 
    /// ADC_PicoDMAModeInit(); /// - due to only one dma in RP2040 and its urgent need in digital part we don't use DMA mode in ADC
    ADC_PicoStandardModeInit(ADC_PicoChannel_0, ADC_PicoSampleNumber, &ADC_picoCh1);
    ADC_PicoStandardModeInit(ADC_PicoChannel_1, ADC_PicoSampleNumber, &ADC_picoCh2);

    add_repeating_timer_ms(-2, ADC_standardModeIrq, NULL, &ADC_irqTimer);
}

static bool ADC_DMAModeIrq(struct repeating_timer *t)
{
    /// ADS1115 I2C pooling routine ///
    ADS1115_doubleBufferingCallback(&ADS1115_ch0);
    ADS1115_doubleBufferingCallback(&ADS1115_ch1);
    
    return true;
}

static bool ADC_standardModeIrq(struct repeating_timer *t)
{
    static uint32_t ADS1115_ctr = 0;

    ADC_PicoStandardModeCallback(&ADC_picoCh1);
    ADC_PicoStandardModeCallback(&ADC_picoCh2);

    if(ADS1115_ctr%10 == 0) // make 1 ADS1115 measure every 50 Pi Pico embedded ADC
    {
        ADS1115_doubleBufferingCallback(&ADS1115_ch0);
        ADS1115_doubleBufferingCallback(&ADS1115_ch1);
    }

    ADS1115_ctr++;
    return true;
}

uint16_t *ADS1115_ADCGetData(uint8_t channel_number)
{
    switch(channel_number)
    {
        case(ADS1115_channel_0):
             if(ADS1115_ch0.current_buffer == 0)
                return ADS1115_ch0.buffer_2.data;
             else 
                return ADS1115_ch0.buffer_1.data;
        break;

        case(ADS1115_channel_1): 
            if(ADS1115_ch1.current_buffer == 0)
                return ADS1115_ch1.buffer_2.data;
            else 
                return ADS1115_ch1.buffer_1.data;
        break;

        default:
            return NULL;
            
    }
}

uint16_t *ADC_PicoStandardModeGetData(uint8_t channel)
{
    if(channel > 3)
        return NULL;

    switch(channel)
    {
        case(ADC_PicoChannel_0):
            if(ADC_picoCh1.current_buffer == 0)
                return ADC_picoCh1.buffer_1.data;
            else
                return ADC_picoCh1.buffer_2.data;
        break;

        case(ADC_PicoChannel_1):
            if(ADC_picoCh2.current_buffer == 0)
                return ADC_picoCh2.buffer_1.data;
            else
                return ADC_picoCh2.buffer_2.data;
        break;
    }

    return NULL;
}
