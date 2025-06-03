#ifndef _COMMUNICATION_CONFIG_
#define _COMMUNICATION_CONFIG_


typedef enum{
    TAG_ERROR               = -1,
    TAG_OK                  =  1,

    TAG_ADC_PICO            =  11,
    TAG_ADC_ADS1115_CH_1    =  12,
    TAG_ADC_ADS1115_CH_2    =  13,

    TAG_DIGITAL_SCOPE       =  21,
    TAG_DIGITAL_TIMER       =  22,
    TAG_DIGITAL_SAMPLE_FREQ =  23,
}send_dataTag_t;


typedef enum{
    CMD_DEVICE_RUN              = 1,
    // DEVICE_STOP             = 0, // IDK

    CMD_ADC_ADS1115_CH1_ENABLE  = 11,
    CMD_ADC_ADS1115_CH2_ENABLE  = 12,
    CMD_ADC_PICO_CH1_ENABLE     = 13,
    CMD_ADC_PICO_CH2_ENABLE     = 14,

    CMD_DIGITAL_PROBE_ENABLE    = 20,
    CMD_DIGITAL_MODE_FREERUN    = 21,
    CMD_DIGITAL_MODE_TRIGGERED  = 22,
    CMD_DIGITAL_MODE_COUNT      = 23,

    CMD_DIGITAL_SET_SAMPLE_FREQ = 31,
    CMD_DIGITAL_SET_TIMER       = 32,
    CMD_DIGITAL_SET_TRIGGER_PIN = 33,

    CMD_PICO_HELLO              = 40,

}config_tag_t;





#endif