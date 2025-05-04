#include "timer.h"





uint TIMER_init(uint slice, uint tickPeriod_ns){
    assert(slice < 8);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_output_polarity(&config, true, false);


    pwm_init(slice, &config, false);
    TIMER_setTickPeriod(slice, tickPeriod_ns);
}


void TIMER_setTickPeriod(uint slice, uint tick_period_ns){
    uint div = tick_period_ns / (5);
    pwm_set_enabled(slice, false);
    pwm_set_clkdiv_int_frac4(slice, div, 0);
    pwm_set_enabled(slice, true);
}

void TIMER_setFreq(uint slice, uint freq){
    uint div = 200 * MHz / freq;
    pwm_set_enabled(slice, false);
    pwm_set_clkdiv_int_frac4(slice, div, 0);
    pwm_set_enabled(slice, true);
}