#include "encoder.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Gray code transition table: index = (prev << 2) | curr, value = direction
static const int8_t ec11_table[16] = {
     0, -1,  1,  0,  // 00 -> 00,01,10,11
     1,  0,  0, -1,  // 01 -> 00,01,10,11
    -1,  0,  0,  1,  // 10 -> 00,01,10,11
     0,  1, -1,  0,  // 11 -> 00,01,10,11
};

static volatile int8_t enc_delta = 0;
static volatile bool btn_pressed = false;
static volatile bool btn_held = false;
static uint8_t prev_state = 0;
static uint32_t btn_state_time = 0;
static bool btn_last_state = false;

void encoder_init(void) {
    gpio_init(ENC_A_PIN);
    gpio_init(ENC_B_PIN);
    gpio_init(ENC_SW_PIN);
    gpio_set_dir(ENC_A_PIN, GPIO_IN);
    gpio_set_dir(ENC_B_PIN, GPIO_IN);
    gpio_set_dir(ENC_SW_PIN, GPIO_IN);
    gpio_pull_up(ENC_A_PIN);
    gpio_pull_up(ENC_B_PIN);
    gpio_pull_up(ENC_SW_PIN);

    prev_state = (gpio_get(ENC_B_PIN) << 1) | gpio_get(ENC_A_PIN);
}

static void encoder_poll_phase(void) {
    uint8_t curr = (gpio_get(ENC_B_PIN) << 1) | gpio_get(ENC_A_PIN);
    if (curr != prev_state) {
        int8_t dir = ec11_table[(prev_state << 2) | curr];
        if (dir != 0) {
            enc_delta += dir;
        }
        prev_state = curr;
    }
}

static void encoder_poll_button(void) {
    bool pressed = !gpio_get(ENC_SW_PIN); // active-low
    uint32_t now = time_us_32() / 1000;
    btn_held = pressed;

    if (pressed != btn_last_state) {
        btn_state_time = now;
        btn_last_state = pressed;
    } else if (pressed && (now - btn_state_time >= ENC_DEBOUNCE_MS)) {
        btn_pressed = true;
    } else if (!pressed && (now - btn_state_time >= ENC_DEBOUNCE_MS)) {
        // released
    }
}

int8_t encoder_get_delta(void) {
    encoder_poll_phase();
    int8_t val = enc_delta;
    enc_delta = 0;
    return val;
}

bool encoder_button_pressed(void) {
    encoder_poll_button();
    bool val = btn_pressed;
    btn_pressed = false;
    return val;
}

bool encoder_button_held(void) {
    encoder_poll_button();
    return btn_held;
}
