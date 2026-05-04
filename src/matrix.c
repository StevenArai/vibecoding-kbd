#include "matrix.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

static const uint8_t matrix_rows[MATRIX_ROW_COUNT] = {14, 15};
static const uint8_t matrix_cols[MATRIX_COL_COUNT] = {6, 7, 8, 9};

// Debounce state
typedef enum { KEY_IDLE, KEY_MAYBE_PRESSED, KEY_PRESSED, KEY_MAYBE_RELEASED } key_state_t;
static key_state_t key_states[MATRIX_ROW_COUNT][MATRIX_COL_COUNT] = {0};
static uint32_t key_state_times[MATRIX_ROW_COUNT][MATRIX_COL_COUNT] = {0};
static uint16_t debounced_state = 0;
static uint16_t previous_state = 0;
static uint16_t just_pressed_cache = 0;

static uint8_t key_index(uint8_t row, uint8_t col) {
    return row * MATRIX_COL_COUNT + col;
}

void matrix_init(void) {
    for (uint8_t i = 0; i < MATRIX_ROW_COUNT; i++) {
        gpio_init(matrix_rows[i]);
        gpio_set_dir(matrix_rows[i], GPIO_OUT);
        gpio_put(matrix_rows[i], 1); // idle high
    }
    for (uint8_t i = 0; i < MATRIX_COL_COUNT; i++) {
        gpio_init(matrix_cols[i]);
        gpio_set_dir(matrix_cols[i], GPIO_IN);
        gpio_pull_up(matrix_cols[i]);
    }
}

uint16_t matrix_scan(void) {
    uint16_t raw_state = 0;
    uint32_t now = time_us_32() / 1000;

    for (uint8_t row = 0; row < MATRIX_ROW_COUNT; row++) {
        gpio_put(matrix_rows[row], 0);
        busy_wait_us(1);

        for (uint8_t col = 0; col < MATRIX_COL_COUNT; col++) {
            bool pressed = !gpio_get(matrix_cols[col]); // active-low
            uint8_t idx = key_index(row, col);

            switch (key_states[row][col]) {
            case KEY_IDLE:
                if (pressed) {
                    key_states[row][col] = KEY_MAYBE_PRESSED;
                    key_state_times[row][col] = now;
                }
                break;
            case KEY_MAYBE_PRESSED:
                if (pressed) {
                    if (now - key_state_times[row][col] >= MATRIX_DEBOUNCE_MS) {
                        key_states[row][col] = KEY_PRESSED;
                        raw_state |= (1 << idx);
                    }
                } else {
                    key_states[row][col] = KEY_IDLE;
                }
                break;
            case KEY_PRESSED:
                if (!pressed) {
                    key_states[row][col] = KEY_MAYBE_RELEASED;
                    key_state_times[row][col] = now;
                } else {
                    raw_state |= (1 << idx);
                }
                break;
            case KEY_MAYBE_RELEASED:
                if (!pressed) {
                    if (now - key_state_times[row][col] >= MATRIX_DEBOUNCE_MS) {
                        key_states[row][col] = KEY_IDLE;
                    }
                } else {
                    key_states[row][col] = KEY_PRESSED;
                    raw_state |= (1 << idx);
                }
                break;
            }
        }
        gpio_put(matrix_rows[row], 1);
        busy_wait_us(100);
    }

    just_pressed_cache = (raw_state & ~debounced_state) & 0xFF;
    previous_state = debounced_state;
    debounced_state = raw_state;
    return raw_state;
}

bool matrix_is_pressed(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROW_COUNT || col >= MATRIX_COL_COUNT) return false;
    return debounced_state & (1 << key_index(row, col));
}

uint8_t matrix_just_pressed(void) {
    if (just_pressed_cache == 0) return 255;
    uint8_t idx = 0;
    while (!(just_pressed_cache & 1)) {
        just_pressed_cache >>= 1;
        idx++;
    }
    just_pressed_cache = 0;
    return idx;
}
