#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>
#include <stdint.h>

#define MATRIX_ROW_COUNT 2
#define MATRIX_COL_COUNT 4
#define MATRIX_DEBOUNCE_MS 10

void matrix_init(void);
uint16_t matrix_scan(void);
bool matrix_is_pressed(uint8_t row, uint8_t col);
uint8_t matrix_just_pressed(void);

#endif // MATRIX_H
