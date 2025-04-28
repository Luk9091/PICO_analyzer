#include "ADC_bootStrap.h"

static bool ADC_bootStrapIrq(struct repeating_timer *t);

static ADS1115_doubleBufferState ADS1115_ch0 = {0};
static ADS1115_doubleBufferState ADS1115_ch1 = {0};

void ADC_bootStrap(void)
{
    static repeating_timer_t ADC_irqTimer;     

    /// ADS1115 initialization
    ADS1115_init();
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_0, ADC_ADS1115SampleNumber, &ADS1115_ch0);
    ADS1115_setChannelDoubleBuffering(ADS1115_channel_1, ADC_ADS1115SampleNumber, &ADS1115_ch1);
    
    /// PI Pico(embedded) ADC initialization 
    /// @todo

    add_repeating_timer_ms(2, ADC_bootStrapIrq, NULL, &ADC_irqTimer);
    //add_repeating_timer_ms();
}

static bool ADC_bootStrapIrq(struct repeating_timer *t)
{
    /// ADS1115 I2C pooling routine ///
    ADS1115_doubleBufferingCallback(&ADS1115_ch0);
    ADS1115_doubleBufferingCallback(&ADS1115_ch1);
    
    return true;
}

static bool ADC_PicoStandardModeIrq(struct repeating_timer *t)
{
    //ADC_PicoStandardModeCallback(&pico_bufferState_1);
    //ADC_PicoStandardModeCallback(&pico_bufferState_2);

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

