#include "core1.h"

static void core1_entry(void);
static bool core1_timerIrq(struct repeating_timer *t);

send_bufferFrame frame = {123};

void core1_init(void)
{
    multicore_launch_core1(core1_entry);
}


static void core1_entry(void)
{
    
    ADC_bootStrap();
    sleep_ms(500);

    static repeating_timer_t ADC_irqTimer;
    add_repeating_timer_us(200, core1_timerIrq, NULL, &ADC_irqTimer);

    wifi_init();

    while(true)
    {
        tight_loop_contents(); //nop
    }

}

static bool core1_timerIrq(struct repeating_timer *t)
{
    static uint32_t data_ctr = 0;
    ADC_standardModeIrq();
    printf("\r");
    if(data_ctr >= 500) // equals to 500 ADC_Pico samples and 50 ADS1115 samples
    {
        frame.ADC_ADS1115BufferCh0  = ADS1115_ADCGetData(ADS1115_channel_0);
        wifi_sendData(frame.ADC_ADS1115BufferCh0, TAG_ADC_ADS1115_CH_1, ADC_ADS1115SampleNumber * sizeof(uint16_t));

        frame.ADC_ADS1115BufferCh1  = ADS1115_ADCGetData(ADS1115_channel_1);
        wifi_sendData(frame.ADC_ADS1115BufferCh1, TAG_ADC_ADS1115_CH_2, ADC_ADS1115SampleNumber * sizeof(uint16_t));

        frame.ADC_PicoBuffer        = ADC_PicoStandardModeGetData();
        //printf("ADC: %d\n", frame.ADC_PicoBuffer[1]);
        wifi_sendData(frame.ADC_PicoBuffer, TAG_ADC_PICO, ADC_PicoSampleNumber * sizeof(uint16_t));

        wifi_sendData(frame.digital_analyzerBuffer, TAG_DIGITAL_SCOPE, 256*sizeof(uint));
        
        
        data_ctr = 0;
    }
    data_ctr++;
}