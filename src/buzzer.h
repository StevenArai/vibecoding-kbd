#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define BUZZER_PIN        13
#define BUZZER_DEFAULT_HZ 2000

void buzzer_init(void);
void buzzer_set_freq(uint16_t freq);
void buzzer_on(void);
void buzzer_off(void);
void buzzer_beep(uint16_t duration_ms);

#endif // BUZZER_H
