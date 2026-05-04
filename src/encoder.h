#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include <stdint.h>

#define ENC_A_PIN  10
#define ENC_B_PIN  11
#define ENC_SW_PIN 12
#define ENC_DEBOUNCE_MS 20

void encoder_init(void);
int8_t encoder_get_delta(void);
bool encoder_button_pressed(void);
bool encoder_button_held(void);

#endif // ENCODER_H
