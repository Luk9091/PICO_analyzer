#ifndef _ADC_
#define _ADC_

#include <stdio.h>
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "ring_buffer.h"
#include "hardware/sync.h"
#include "hardware/structs/i2c.h"

#define ADS1115_SDAPin 20
#define ADS1115_SCLPin 21
#define ADS1115_I2CInstance i2c0
#define ADS1115_alertRdyGpio 22

#define ADS1115_Address         0x48
#define ADS1115_conversionReg   0b00000000
#define ADS1115_configReg       0b00000001
#define ADS1115_LOThreshReg     0b00000010
#define ADS1115_HiThreshReg     0b00000011

#define ADS1115_OS_0  0     // 0 : Device is currently performing a conversion
#define ADS1115_OS_1  1     // 1 : Device is not currently performing a conversion

#define ADS1115_channel_0 0
#define ADS1115_channel_1 1
#define ADS1115_channel_2 2 
#define ADS1115_channel_3 3

#define ADS1115_PGA_1 1     // 101b : FSR = ±0.256V
#define ADS1115_PGA_2 2     // 100b : FSR = ±0.512V
#define ADS1115_PGA_3 3     // 011b : FSR = ±1.024V
#define ADS1115_PGA_4 4     // 010b : FSR = ±2.048V (default)
#define ADS1115_PGA_5 5     // 001b : FSR = ±4.096V
#define ADS1115_PGA_6 6     // 6.144V

#define ADS1115_modeCircular   0
#define ADS1115_modeSingleShot 1

#define ADS1115_dataRate8      0 // 000 : 8 SPS
#define ADS1115_dataRate16     1 // 001 : 16 SPS
#define ADS1115_dataRate32     2 // 010 : 32 SPS
#define ADS1115_dataRate64     3 // 011 : 64 SPS
#define ADS1115_dataRate128    4 // 100 : 128 SPS (default)
#define ADS1115_dataRate250    5 // 101 : 250 SPS
#define ADS1115_dataRate475    6 // 110 : 475 SPS
#define ADS1115_dataRate860    7 // 111 : 860 SPS

#define ADS1115_compModeTraditional    0
#define ADS1115_compModeWindow         1

#define ADS1115_compPolActiveLow       0
#define ADS1115_compPolActiveHigh      1

#define ADS1115_compLatNoLatching      0
#define ADS1115_compLatLatching        1

#define ADS1115_compQueAfterOne        0
#define ADS1115_compQueAfterTwo        1
#define ADS1115_compQueAfterFour       2
#define ADS1115_compQueDisable         3

#define ADC_ADS1115SampleNumber 50 

typedef struct{
    uint8_t OS_state;
    uint8_t MUX_state;
    uint8_t PGA_state;
    uint8_t MODE_state;
    uint8_t DR_state;
    uint8_t COMP_MODE_state;
    uint8_t COMP_POL_state;
    uint8_t COMP_LAT_state;
    uint8_t COMP_QUE_state;

    bool is_convReadyMode;
}ADS1115_configState;


typedef struct{
    uint32_t data_counter;      // valid data counter
    uint16_t channel_number;    // channel number
    uint16_t current_buffer;    // current buffer where new samples are saved
    ring_buffer buffer_0;       // first swapping buffer
    ring_buffer buffer_1;       // second swapping buffer
}ADS1115_channelConfig;

typedef enum{
    I2C_READ_PHASE_IDLE     = 0,
    I2C_READ_PHASE_WRITE    = 1,
    I2C_READ_PHASE_READ     = 2,
    I2C_READ_PHASE_DONE     = 3,
    I2C_READ_PHASE_ERROR    = -1
}I2C_irqReadPhase;

typedef enum{
    I2C_WRITE_PHASE_IDLE    = 0,
    I2C_WRITE_PHASE_WRITE   = 1,
    I2C_WRITE_PHASE_DONE    = 2,
    I2C_WRITE_PHASE_ERROR   = -1
}I2C_irqWritePhase;

typedef struct{
    /// ADS1115_writeRegAsync variable part ///
    uint8_t user_configRegTypeW;     // ADS1115 config register type           
    uint8_t user_dataToWriteW[2];    // uint16_t = [uint8_t | uint8_t]
    uint8_t user_dataToWriteIndexW;  // point's to next dataToWrite array element
    /// ********* ///

    /// ADS1115_readRegAsync variable part ///
    uint16_t *user_dataReceivedR;   // data received from ADS1115
    bool user_dataReceivedValid;    // true - received full frame(uint16_t)
    uint8_t user_configRegTypeR;    // uint16_t = [uint8_t | uint8_t]
    uint8_t user_dataReceivedIndexR;// indicates received data from device
    /// ********* ///

    bool I2C_transmissionError;     // true - error occurred, 0 otherwise
    I2C_irqReadPhase  I2C_irqReadPhase_t;
    I2C_irqWritePhase I2C_irqWritePhase_t;
}I2C_irqTransmissionState;

/// @brief ADS1115 standard initialize 
void ADS1115_init(void);

/// @brief ADS1115 interrupt working mode
/// @param - --
void ADS1115_initWithIrqMode(void);

/// @brief ADC1115 set operation Mode Circular or one shot
/// @param mode - Circular/One shot
void ADS1115_setOperationMode(uint8_t mode);

/// @brief ADS11115 set Programmable Gain Amplifier
/// @param  PGA gain value
void ADS1115_setPGA(uint8_t PGA_val);

/// @brief ADS1115 set measured channel number
/// @param channel_number   - --
void ADS1115_setChannel(uint8_t channel_number);

/// @brief double buffering mode initialization 
/// @param channel_number   - --
/// @param buffer_size      - --
/// @param BufferState - data structure containing double buffering mode data 
/// @return true - Error occurred, false otherwise
void ADS1115_setChannelDoubleBuffering(uint8_t channel_number, uint32_t buffer_size, ADS1115_channelConfig *BufferState);

/// @brief double buffering mode callback
/// @param channel_number - --
/// @param new_adcSample - -- 
void ADS1115_doubleBufferingSaveData(ADS1115_channelConfig *buffer_state, uint16_t new_adcSample);

/// @brief Enables the ADS1115 alert pin to indicate when new conversion data is ready
/// @warning This function is not finished. It is strongly recommended not to use it.
/// @param enable - true - enable, false - disable
/// @param buffer_stateConfig0 - first channel config. data struct
/// @param buffer_stateConfig1 - second channel config. data struct
void ADS1115_setModeWithGpioAlert(bool enable, ADS1115_channelConfig *buffer_stateConfig0, ADS1115_channelConfig *buffer_stateConfig1);

/// @brief READ ADC one sample from selected channel
/// @param channel  - --
/// @return         - ADC single sample   
uint16_t ADS1115_getSample(uint8_t channel);

/// @brief ADC received raw data converter -> convert from raw data to voltages
/// @warning This method is not recommended to use on rp2040 !!!, is ONLY for debugging 
/// @param data - raw ADC value
/// @return     - voltage value
float ADS1115_dataConvert(int16_t data);

#endif