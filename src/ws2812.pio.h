/* Pre-generated from ws2812.pio to avoid pioasm host build */
#ifndef _WS2812_PIO_H_
#define _WS2812_PIO_H_

#include "hardware/pio.h"

#define ws2812_wrap_target 0
#define ws2812_wrap 3
#define ws2812_T1 3
#define ws2812_T2 3
#define ws2812_T3 4

static const uint16_t ws2812_program_instructions[] = {
    0x6021,
    0x1123,
    0x1003,
    0xa023,
};

static const pio_program_t ws2812_program = {
    .instructions = ws2812_program_instructions,
    .length = 4,
    .origin = -1,
#if PICO_PIO_VERSION > 1 || PICO_RP2350
    .pio_version = 0,
#endif
};

static inline pio_sm_config ws2812_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + ws2812_wrap_target, offset + ws2812_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

#if !PICO_NO_HARDWARE
#include "hardware/clocks.h"

static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
#endif

#endif // _WS2812_PIO_H_
