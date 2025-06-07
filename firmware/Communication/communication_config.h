#ifndef _COMMUNICATION_CONFIG_
#define _COMMUNICATION_CONFIG_

/// @brief transmission status TAG's
typedef enum{
    TAG_ERROR               = -1,
    TAG_OK                  =  0,

    TAG_ADC_PICO_CH_1       =  11,
    TAG_ADC_PICO_CH_2       =  12,
    TAG_ADC_ADS1115_CH_1    =  13,
    TAG_ADC_ADS1115_CH_2    =  14,

    TAG_DIGITAL             =  21,
    TAG_DIGITAL_TIMER       =  22,
}send_dataTag_t;

/// @brief Device configuration available TAG 
typedef enum{
    CMD_DEVICE_RUN                      = 1,
    CMD_DEVICE_STOP                     = -1,

    CMD_ADC_ADS1115_CH1_ENABLE          = 11,
    CMD_ADC_ADS1115_CH2_ENABLE          = 12,
    CMD_ADC_PICO_CH1_ENABLE             = 13,
    CMD_ADC_PICO_CH2_ENABLE             = 14,

    CMD_DIGITAL_PROBE_ENABLE            = 20,
    CMD_DIGITAL_MODE_FREERUN            = 21,
    CMD_DIGITAL_MODE_TRIGGERED          = 22,
    CMD_DIGITAL_MODE_COUNT_FREERUN      = 23,
    CMD_DIGITAL_MODE_COUNT              = 24,

    // CMD_DIGITAL_SET_SAMPLE_FREQ         = 31,
    // CMD_DIGITAL_SET_TIMER               = 32,
    // CMD_DIGITAL_SET_TRIGGER_PIN         = 33,

    CMD_PICO_HELLO                      = 40,

}device_configTag_t;


// typedef struct{
//     bool ADC_PICO_CH1_VALID;
//     bool ADC_PICO_CH2_VALID;
//     bool ADC_ADS1115_CH1_VALID;
//     bool ADC_ADS1115_CH2_VALID;
// }ADC_validBufferStatus;


typedef enum{
    FIFO_FRAME_DATA,        // frame containing data
    FIFO_FRAME_CONFIG,      // frame containing configuration information(see "device_configTag_t")
    FIFO_FRAME_DATA_CONFIG  // frame containing data & configuration
}fifo_frameType_t;

typedef enum{
    mc_fifoBuffer0 = 0,
    mc_fifoBuffer1 = 1,
    mc_fifoBuffer2 = 2,
    mc_fifoBuffer3 = 3
}core0_validBufferNumber;




#endif
