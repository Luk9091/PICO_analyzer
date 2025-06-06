#include "core1.h"

/// Private function prototypes
static void core1_entry(void);
static bool core1_timerIrq(struct repeating_timer *t);

/// Private variable's
static volatile bool wifi_sendReady = false;
send_bufferFrame frame = {0};

void core1_init(void)
{
    multicore_launch_core1(core1_entry);
}

static void core1_entry(void)
{
    wifi_init();

    ADC_bootStrap();
    sleep_ms(500);

    static repeating_timer_t ADC_irqTimer;
    add_repeating_timer_us(200, core1_timerIrq, NULL, &ADC_irqTimer);
    

    while(true)
    {        
        uint32_t mask = save_and_disable_interrupts();   
        bool do_send = wifi_sendReady;                   
        wifi_sendReady = false;                          
        restore_interrupts(mask);  
        if(do_send)
        {
            frame.ADC_ADS1115BufferCh0  = ADS1115_ADCGetData(ADS1115_channel_2); 
            wifi_sendData(frame.ADC_ADS1115BufferCh0, TAG_ADC_ADS1115_CH_1, ADC_ADS1115SampleNumber * sizeof(uint16_t));
 
            frame.ADC_ADS1115BufferCh1  = ADS1115_ADCGetData(ADS1115_channel_3); 
            wifi_sendData(frame.ADC_ADS1115BufferCh1, TAG_ADC_ADS1115_CH_2, ADC_ADS1115SampleNumber * sizeof(uint16_t));
    
            frame.ADC_PicoBufferCh0     = ADC_PicoStandardModeGetData(0);
            wifi_sendData(frame.ADC_PicoBufferCh0, TAG_ADC_PICO_CH_1, ADC_PicoSampleNumber * sizeof(uint16_t));

            frame.ADC_PicoBufferCh1     = ADC_PicoStandardModeGetData(1);
            wifi_sendData(frame.ADC_PicoBufferCh1, TAG_ADC_PICO_CH_2, ADC_PicoSampleNumber * sizeof(uint16_t));
    
            //frame.digital_analyzerBuffer= DigitalAnalyzerGetBuffer  
            //wifi_sendData(frame.digital_analyzerBuffer, TAG_DIGITAL_SCOPE, 256*sizeof(uint));
        
            //fifo_routineCore1();
            wifi_sendReady = false;
        }

        cyw43_arch_poll();
        sleep_ms(100);
    }
}

static bool core1_timerIrq(struct repeating_timer *t)
{
    static uint32_t data_ctr = 0;

    /// @brief Performs measurements using both the ADS1115 and Pi Pico ADCs.
    ADC_standardModeIrq();
    
    if(data_ctr >= 500) // equals to 500 ADC_Pico samples and 50 ADS1115 samples & executes one times per second
    {
        wifi_sendReady = true;
        data_ctr = 0;
    }
    data_ctr++;
    return true;
}
