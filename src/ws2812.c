#include "ws2812.h"
#include "ws2812.pio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"

static ws2812_color_t pixels[WS2812_COUNT] = {0};
static int dma_chan = -1;

void ws2812_init(void) {
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm, true));
    dma_channel_configure(dma_chan, &cfg,
                          &pio->txf[sm],
                          NULL,
                          0,
                          false);
}

void ws2812_set_pixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= WS2812_COUNT) return;
    pixels[index].r = r;
    pixels[index].g = g;
    pixels[index].b = b;
}

void ws2812_show(void) {
    // WS2812B uses GRB order
    uint8_t buf[WS2812_COUNT * 3];
    for (uint8_t i = 0; i < WS2812_COUNT; i++) {
        buf[i * 3 + 0] = pixels[i].g;
        buf[i * 3 + 1] = pixels[i].r;
        buf[i * 3 + 2] = pixels[i].b;
    }

    dma_channel_wait_for_finish_blocking(dma_chan);
    dma_channel_transfer_from_buffer_now(dma_chan, buf, sizeof(buf));

    // Wait for transmission to complete (24 bits per LED + ~50us reset)
    busy_wait_us(WS2812_COUNT * 30 + 60);
    dma_channel_wait_for_finish_blocking(dma_chan);
}

void ws2812_fill(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < WS2812_COUNT; i++) {
        pixels[i].r = r;
        pixels[i].g = g;
        pixels[i].b = b;
    }
}

void ws2812_clear(void) {
    for (uint8_t i = 0; i < WS2812_COUNT; i++) {
        pixels[i].r = 0;
        pixels[i].g = 0;
        pixels[i].b = 0;
    }
}
