#include "st7735.h"
#include "font5x7.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#define CS_LOW  gpio_put(ST7735_CS_PIN, 0)
#define CS_HIGH gpio_put(ST7735_CS_PIN, 1)
#define DC_CMD  gpio_put(ST7735_DC_PIN, 0)
#define DC_DATA gpio_put(ST7735_DC_PIN, 1)

static void st7735_write_cmd(uint8_t cmd) {
    CS_LOW;
    DC_CMD;
    spi_write_blocking(ST7735_SPI_PORT, &cmd, 1);
    CS_HIGH;
}

static void st7735_write_data(const uint8_t *data, size_t len) {
    CS_LOW;
    DC_DATA;
    spi_write_blocking(ST7735_SPI_PORT, data, len);
    CS_HIGH;
}

static void st7735_set_window(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    // ST7735 1.77" panel has 1px column offset and 2px row offset
    x0 += 1; x1 += 1;
    y0 += 2; y1 += 2;

    uint8_t caset[4] = {
        (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF),
        (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF)
    };
    uint8_t raset[4] = {
        (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF),
        (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF)
    };

    st7735_write_cmd(0x2A); st7735_write_data(caset, 4); // CASET
    st7735_write_cmd(0x2B); st7735_write_data(raset, 4); // RASET
    st7735_write_cmd(0x2C);                               // RAMWR
}

void st7735_init(void) {
    gpio_init(ST7735_CS_PIN);
    gpio_init(ST7735_RES_PIN);
    gpio_init(ST7735_DC_PIN);
    gpio_set_dir(ST7735_CS_PIN, GPIO_OUT);
    gpio_set_dir(ST7735_RES_PIN, GPIO_OUT);
    gpio_set_dir(ST7735_DC_PIN, GPIO_OUT);
    CS_HIGH;

    spi_init(ST7735_SPI_PORT, ST7735_SPI_FREQ);
    gpio_set_function(ST7735_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ST7735_CLK_PIN, GPIO_FUNC_SPI);

    // Hardware reset
    gpio_put(ST7735_RES_PIN, 0);
    sleep_ms(10);
    gpio_put(ST7735_RES_PIN, 1);
    sleep_ms(120);

    // Software reset
    st7735_write_cmd(0x01); // SWRESET
    sleep_ms(120);

    // Frame rate control
    uint8_t frmctr1[] = {0x01, 0x2C, 0x2D};
    st7735_write_cmd(0xB1); st7735_write_data(frmctr1, 3);

    // Display inversion
    st7735_write_cmd(0x21); // INVON

    // Power control
    uint8_t pwctr1[] = {0xA2, 0x02, 0x84};
    st7735_write_cmd(0xC0); st7735_write_data(pwctr1, 3);

    uint8_t pwctr2[] = {0xC5};
    st7735_write_cmd(0xC1); st7735_write_data(pwctr2, 1);

    uint8_t pwctr3[] = {0x0A, 0x00};
    st7735_write_cmd(0xC2); st7735_write_data(pwctr3, 2);

    uint8_t pwctr4[] = {0x8A, 0x2A};
    st7735_write_cmd(0xC3); st7735_write_data(pwctr4, 2);

    uint8_t pwctr5[] = {0x8A, 0xEE};
    st7735_write_cmd(0xC4); st7735_write_data(pwctr5, 2);

    uint8_t pwctr6[] = {0x0E};
    st7735_write_cmd(0xC5); st7735_write_data(pwctr6, 1);

    // Column/row address mode
    uint8_t madctl[] = {0xC0}; // BGR, MX=1, MY=1
    st7735_write_cmd(0x36); st7735_write_data(madctl, 1);

    // Gamma
    st7735_write_cmd(0x26); // GAMSET
    uint8_t gamma[] = {0x02};
    st7735_write_data(gamma, 1);

    // Display on
    st7735_write_cmd(0x11); // SLPOUT
    sleep_ms(120);
    st7735_write_cmd(0x29); // DISPON
    sleep_ms(100);

    // Backlight PWM
    uint slice_num = pwm_gpio_to_slice(ST7735_BL_PIN);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, 255);
    pwm_init(slice_num, &cfg, true);
    gpio_set_function(ST7735_BL_PIN, GPIO_FUNC_PWM);
    pwm_set_gpio_level(ST7735_BL_PIN, 200);
}

void st7735_fill_screen(uint16_t color) {
    uint8_t hi = (uint8_t)(color >> 8);
    uint8_t lo = (uint8_t)(color & 0xFF);
    uint16_t pixel_count = (uint16_t)ST7735_WIDTH * ST7735_HEIGHT;

    st7735_set_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    CS_LOW;
    DC_DATA;

    // Send 256 pixels at a time
    uint8_t buf[512]; // 256 pixels * 2 bytes
    for (int i = 0; i < 256; i++) {
        buf[i * 2] = hi;
        buf[i * 2 + 1] = lo;
    }

    while (pixel_count > 0) {
        size_t n = pixel_count > 256 ? 256 : pixel_count;
        spi_write_blocking(ST7735_SPI_PORT, buf, n * 2);
        pixel_count -= n;
    }
    CS_HIGH;
}

void st7735_draw_pixel(int16_t x, int16_t y, uint16_t color) {
    if (x < 0 || x >= ST7735_WIDTH || y < 0 || y >= ST7735_HEIGHT) return;
    st7735_set_window(x, y, x, y);
    uint8_t buf[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    st7735_write_data(buf, 2);
}

void st7735_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t dx = abs(x1 - x0), dy = abs(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;

    while (true) {
        st7735_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

void st7735_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    st7735_draw_line(x, y, x + w - 1, y, color);
    st7735_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    st7735_draw_line(x, y, x, y + h - 1, color);
    st7735_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    for (int16_t i = 0; i < h; i++) {
        for (int16_t j = 0; j < w; j++) {
            st7735_draw_pixel(x + j, y + i, color);
        }
    }
}

void st7735_draw_string(int16_t x, int16_t y, const char *str, uint16_t color, uint16_t bg) {
    if (!str) return;

    while (*str) {
        uint8_t idx = (uint8_t)(*str - FONT_FIRST_CHAR);
        if (idx < FONT_CHAR_COUNT) {
            for (uint8_t cx = 0; cx < FONT_CHAR_WIDTH; cx++) {
                uint8_t col = font5x7[idx][cx];
                for (uint8_t cy = 0; cy < FONT_CHAR_HEIGHT; cy++) {
                    if (col & (1 << cy)) {
                        st7735_draw_pixel(x + cx, y + cy, color);
                    } else {
                        st7735_draw_pixel(x + cx, y + cy, bg);
                    }
                }
            }
        }
        x += FONT_CHAR_WIDTH + 1;
        str++;
    }
}
