#include "pico/stdlib.h"
#include "st7735.h"
#include "ws2812.h"
#include "matrix.h"
#include "encoder.h"
#include "voice_uart.h"

int main(void) {
    stdio_init_all();

    st7735_init();
    ws2812_init();
    matrix_init();
    encoder_init();
    voice_uart_init();
    buzzer_init();

    printf("Keyboard initialized\n");

    st7735_fill_screen(COLOR_BLACK);
    st7735_draw_string(10, 10, "Ready", COLOR_WHITE, COLOR_BLACK);

    uint8_t anim_frame = 0;
    uint32_t last_matrix_scan = 0;
    uint32_t last_anim = 0;
    char buf[24];

    while (true) {
        uint32_t now = time_us_32() / 1000;

        // Matrix scan at ~100Hz
        if (now - last_matrix_scan >= 10) {
            last_matrix_scan = now;
            matrix_scan();

            uint8_t key = matrix_just_pressed();
            if (key != 255) {
                buzzer_beep(50);
                st7735_fill_rect(10, 30, 108, 8, COLOR_BLACK);
                snprintf(buf, sizeof(buf), "Key: %d", key);
                st7735_draw_string(10, 30, buf, COLOR_GREEN, COLOR_BLACK);
                printf("Key pressed: %d\n", key);
            }
        }

        // Encoder
        int8_t delta = encoder_get_delta();
        if (delta != 0) {
            st7735_fill_rect(10, 50, 108, 8, COLOR_BLACK);
            snprintf(buf, sizeof(buf), "Enc: %+d", delta);
            st7735_draw_string(10, 50, buf, COLOR_CYAN, COLOR_BLACK);
            printf("Encoder: %+d\n", delta);
        }
        if (encoder_button_pressed()) {
            buzzer_beep(30);
            st7735_fill_rect(10, 60, 108, 8, COLOR_BLACK);
            st7735_draw_string(10, 60, "Enc Btn", COLOR_YELLOW, COLOR_BLACK);
            printf("Encoder button pressed\n");
        }

        // Voice UART
        voice_uart_poll();
        if (voice_uart_has_command()) {
            uint8_t cmd = voice_uart_get_command_id();
            st7735_fill_rect(10, 70, 108, 8, COLOR_BLACK);
            snprintf(buf, sizeof(buf), "Voice: 0x%02X", cmd);
            st7735_draw_string(10, 70, buf, COLOR_MAGENTA, COLOR_BLACK);
            printf("Voice command: 0x%02X\n", cmd);
        }

        // LED animation at ~30Hz
        if (now - last_anim >= 33) {
            last_anim = now;
            anim_frame++;
            ws2812_clear();
            uint8_t led = anim_frame % WS2812_COUNT;
            ws2812_set_pixel(led, 0, 0, 200);
            ws2812_show();
        }

        sleep_ms(1);
    }
}
