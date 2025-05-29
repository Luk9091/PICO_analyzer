#include "ADC_bootStrap.h"

static ADS1115_channelConfig ADS1115_ch0 = {0};
static ADS1115_channelConfig ADS1115_ch1 = {0};
static Pico_adcChannelConfig ADC_picoCh0 = {0};
static Pico_adcChannelConfig ADC_picoCh1 = {0};


void ADC_bootStrap(void)
{
    /// ADS1115 initialization
    ADS1115_init();
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_0, ADC_ADS1115SampleNumber, &ADS1115_ch0);
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_1, ADC_ADS1115SampleNumber, &ADS1115_ch1);
    ADS1115_setModeWithGpioAlert(true, &ADS1115_ch0, &ADS1115_ch1);

    /// PI Pico(embedded) ADC initialization 
    ADC_PicoStandardModeInit(ADC_PicoChannel_0, ADC_PicoSampleNumber, &ADC_picoCh0);
    ADC_PicoStandardModeInit(ADC_PicoChannel_1, ADC_PicoSampleNumber, &ADC_picoCh1);
}

void ADC_DmaModeIrq(void)
{
    /// ADS1115 I2C pooling routine ///
    ADS1115_saveData(&ADS1115_ch0, ADS1115_getSample(ADS1115_channel_0));
    ADS1115_saveData(&ADS1115_ch1, ADS1115_getSample(ADS1115_channel_1));
}

void ADC_standardModeIrq(void)
{
    static uint32_t ADS1115_ctr = 0;

    ADC_PicoStandardModeCallback(&ADC_picoCh0);
    ADC_PicoStandardModeCallback(&ADC_picoCh1);

    if(++ADS1115_ctr >= 10)
    {
        ADS1115_ctr = 0; 
        //ADS1115_routineCallback(&ADS1115_ch0, ADS1115_getSample(ADS1115_channel_0));
        //ADS1115_routineCallback(&ADS1115_ch1, ADS1115_getSample(ADS1115_channel_1));
        ADS1115_routineCallbackWithGpioAlert();
    }

    ADS1115_ctr++;
}

uint16_t *ADS1115_ADCGetData(uint8_t channel_number)
{
    switch(channel_number)
    {
        case(ADS1115_channel_0):
             if(ADS1115_ch0.current_buffer == 0)
                return ADS1115_ch0.buffer_1.data;
             else 
                return ADS1115_ch0.buffer_0.data;
        break;

        case(ADS1115_channel_1): 
            if(ADS1115_ch1.current_buffer == 0)
                return ADS1115_ch1.buffer_1.data;
            else 
                return ADS1115_ch1.buffer_0.data;
        break;

        default:
            return NULL;       
    }
}

uint16_t *ADC_PicoStandardModeGetData(uint8_t ADC_channelNumber)
{
    switch(ADC_channelNumber)
    {
        case(0):
            if(ADC_picoCh0.current_buffer == 0)
                return ADC_picoCh0.buffer_0.data;
            else
                return ADC_picoCh0.buffer_1.data;
        break;

        case(1):
            if(ADC_picoCh1.current_buffer == 0)
                return ADC_picoCh1.buffer_0.data;
            else
                return ADC_picoCh1.buffer_1.data;
        break;
    }
    return NULL;
}

