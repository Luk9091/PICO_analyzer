#ifndef __READ_H__
#define __READ_H__

#include <pico/stdio.h>
#include <hardware/pio.h>

#include <read.pio.h>
static inline float PIO_get_division(uint sampleFreq);

static inline void triggered_read_program_init(PIO pio, uint sm, uint offset, uint firstPin, uint countPins, uint triggerPin){
    pio_sm_config c = triggered_read_program_get_default_config(offset);
    sm_config_set_in_pins(&c, firstPin);

    for (uint i = 0; i < countPins; i++){
        pio_gpio_init(pio, firstPin + i);
    }
    pio_sm_set_consecutive_pindirs(pio, sm, firstPin, countPins, false);

    sm_config_set_jmp_pin(&c, triggerPin);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    sm_config_set_in_shift(&c, false, true, countPins);

    pio_sm_init(pio, sm, offset, &c);
    // pio_sm_set_enabled(pio, sm, true);
}


static inline void continue_read_program_init(PIO pio, uint sm, uint offset, uint firstPin, uint countPins, uint sampleFreq){
    pio_sm_config c = continue_read_program_get_default_config(offset);
    sm_config_set_in_pins(&c, firstPin);

    for (uint i = 0; i < countPins; i++){
        pio_gpio_init(pio, firstPin + i);
    }
    pio_sm_set_consecutive_pindirs(pio, sm, firstPin, countPins, false);

    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    sm_config_set_in_shift(&c, false, true, countPins);



    sm_config_set_clkdiv(&c, PIO_get_division(sampleFreq));

    pio_sm_init(pio, sm, offset, &c);
    pio_interrupt_clear(pio, 0);
    // pio_sm_set_enabled(pio, sm, true);
}




static inline float PIO_get_division(uint sampleFreq){
    if (sampleFreq < 5 * kHz){
        sampleFreq = 5 * kHz;
    }
    float div = (200.f * MHz) / sampleFreq;
    if (div < 1){
        div = 1;
    }

    return div;
}



#endif