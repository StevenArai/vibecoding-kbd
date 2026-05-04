#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>

#define ST7735_CS_PIN   17
#define ST7735_MOSI_PIN 18
#define ST7735_CLK_PIN  19
#define ST7735_RES_PIN  20
#define ST7735_DC_PIN   21
#define ST7735_BL_PIN   22
#define ST7735_SPI_PORT spi0
#define ST7735_SPI_FREQ 30000000
#define ST7735_WIDTH    128
#define ST7735_HEIGHT   160

#define RGB565(r, g, b) (((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F))

#define COLOR_BLACK   RGB565(0, 0, 0)
#define COLOR_WHITE   RGB565(31, 63, 31)
#define COLOR_RED     RGB565(31, 0, 0)
#define COLOR_GREEN   RGB565(0, 63, 0)
#define COLOR_BLUE    RGB565(0, 0, 31)
#define COLOR_YELLOW  RGB565(31, 63, 0)
#define COLOR_CYAN    RGB565(0, 63, 31)
#define COLOR_MAGENTA RGB565(31, 0, 31)

void st7735_init(void);
void st7735_fill_screen(uint16_t color);
void st7735_draw_pixel(int16_t x, int16_t y, uint16_t color);
void st7735_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void st7735_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void st7735_draw_string(int16_t x, int16_t y, const char *str, uint16_t color, uint16_t bg);

#endif // ST7735_H
