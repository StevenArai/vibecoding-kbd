#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

// Keyboard matrix configuration
#define ROW_COUNT 5
#define COL_COUNT 15

// I2C for OLED display
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define I2C_BAUDRATE 400000

// Keyboard row/col GPIO pins (customize for your PCB)
static const uint8_t row_pins[ROW_COUNT] = {0, 1, 2, 3, 4};
static const uint8_t col_pins[COL_COUNT] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19, 20, 21};

// Key state: 1 = pressed, 0 = released
static bool key_state[ROW_COUNT][COL_COUNT] = {0};

void matrix_init(void) {
    // Rows as outputs, drive high
    for (uint8_t i = 0; i < ROW_COUNT; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], false);
    }

    // Columns as inputs with pull-down
    for (uint8_t i = 0; i < COL_COUNT; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_down(col_pins[i]);
    }
}

void matrix_scan(void) {
    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        gpio_put(row_pins[row], true);
        sleep_us(1);

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            key_state[row][col] = gpio_get(col_pins[col]);
        }

        gpio_put(row_pins[row], false);
        sleep_us(10); // debounce delay
    }
}

void i2c_init_display(void) {
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

int main(void) {
    // Enable USB serial for debugging
    stdio_init_all();

    matrix_init();
    i2c_init_display();

    printf("Keyboard initialized\n");

    while (true) {
        matrix_scan();

        // Print pressed keys to USB serial
        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            for (uint8_t col = 0; col < COL_COUNT; col++) {
                if (key_state[row][col]) {
                    printf("Key R%d C%d pressed\n", row, col);
                }
            }
        }

        sleep_ms(5); // 200Hz scan rate
    }
}
