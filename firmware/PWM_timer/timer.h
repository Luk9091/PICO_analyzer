#ifndef __PWM_TIMER_H__
#define __PWM_TIMER_H__
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include "util.h"

#define TIMER_SLICE (0)

uint TIMER_init(uint slice, uint tickPeriod);



/*!
 * \brief Set tick freq of pwm timer
 * \ingroup pwm
 * 
 * \param slice number of pwm slice
 * \param tick_period time in ns between two tick, change step: 5 (ns)
 *                     - max 1275ns
*/
void TIMER_setTickPeriod(uint slice, uint tick_period_ns);

void TIMER_setFreq(uint slice, uint freq);

#endif //__PWM_TIMER_H__