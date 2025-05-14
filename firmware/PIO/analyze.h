#ifndef __ANALYZE_H__
#define __ANALYZE_H__

#include <pico/stdlib.h>
#include <hardware/pio.h>

#define TRIGGER_GPIO    16

#define LSB_GPIO        0
#define PIO_NUM_PIN     16


static enum ANALYZE_MODE_t {
    NONE_ANALYZE,
    TRIGGERED_ANALYZE,
    FREE_RUN_ANALYZE,
    COUNT_ANALYZE
} ANALYZE_MODE;

void ANALYZE_init();

uint ANALYZE_getSampleFreq();

void ANALYZE_enable(bool run);
bool ANALYZE_timerIsOn();
void ANALYZE_setTrigger(uint pin);


void ANALYZE_triggeredInit(uint timeFreq);
void ANALYZE_continueInit(uint sampleFreq);
void ANALYZE_countInit(uint countSample, uint sampleFreq);

void ANALYZE_deinit();


#endif