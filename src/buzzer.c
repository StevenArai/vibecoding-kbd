#include "buzzer.h"
#include "hardware/clocks.h"

static uint16_t buzzer_wrap = 31250; // 125MHz / (2000 * 2) = 31250

void buzzer_init(void) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, buzzer_wrap);
    pwm_init(slice_num, &cfg, true);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void buzzer_set_freq(uint16_t freq) {
    if (freq == 0) return;
    buzzer_wrap = clock_get_hz(clk_sys) / (freq * 2);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, buzzer_wrap);
    pwm_init(slice_num, &cfg, true);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
}

void buzzer_on(void) {
    pwm_set_gpio_level(BUZZER_PIN, buzzer_wrap / 2);
}

void buzzer_off(void) {
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void buzzer_beep(uint16_t duration_ms) {
    buzzer_on();
    sleep_ms(duration_ms);
    buzzer_off();
}
