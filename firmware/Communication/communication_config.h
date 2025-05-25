#ifndef _COMMUNICATION_CONFIG_
#define _COMMUNICATION_CONFIG_

/// @brief transmission status TAG's
typedef enum{
    TAG_OK = 0,
    TAG_ERROR = -1,
    TAG_ADC_PICO_CH_1,
    TAG_ADC_PICO_CH_2,
    TAG_ADC_ADS1115_CH_1,
    TAG_ADC_ADS1115_CH_2,
    TAG_DIGITAL_SCOPE
}send_dataTag_t;

/// @brief Device configuration available TAG 
typedef enum{
    ADC_ADS1115_CH1_ENABLE  = 0,
    ADC_ADS1115_CH2_ENABLE  = 1,
    ADC_PICO_CH1_ENABLE     = 2,
    ADC_PICO_CH2_ENABLE     = 3,

    DIGITAL_PROBE_ENABLE    = 4,
    DIGITAL_MODE_FREERUN    = 5,
    DIGITAL_MODE_TRIGGERED  = 6,
    DIGITAL_SET_SAMPLE_FREQ = 7,
    DIGITAL_SET_TIMER       = 8,

    DEVICE_STOP = 9,
    DEVICE_RUN  = 10
}device_configTag_t;

/// @brief Struct containing current device configuration 
typedef struct{
    bool ADC_ADS1115_CH1_ENABLE;
    bool ADC_ADS1115_CH2_ENABLE;
    bool ADC_PICO_CH1_ENABLE;
    bool ADC_PICO_CH2_ENABLE;

    bool DIGITAL_PROBE_ENABLE;
    bool DIGITAL_MODE_FREERUN;
    bool DIGITAL_MODE_TRIGGERED;
    bool DIGITAL_SET_SAMPLE_FREQ;
    bool DIGITAL_SET_TIMER;

    bool DEVICE_STOP;
    bool DEVICE_RUN;
}device_configStatus_t;

typedef struct{
    bool ADC_PICO_CH1_VALID;
    bool ADC_PICO_CH2_VALID;
    bool ADC_ADS1115_CH1_VALID;
    bool ADC_ADS1115_CH2_VALID;
}ADC_validBufferStatus;


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