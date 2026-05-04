#ifndef WS2812_H
#define WS2812_H

#include <stdint.h>

#define WS2812_PIN   16
#define WS2812_COUNT 8

typedef struct {
    uint8_t r, g, b;
} ws2812_color_t;

void ws2812_init(void);
void ws2812_set_pixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void ws2812_show(void);
void ws2812_fill(uint8_t r, uint8_t g, uint8_t b);
void ws2812_clear(void);

#endif // WS2812_H
