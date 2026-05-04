#ifndef VOICE_UART_H
#define VOICE_UART_H

#include <stdbool.h>
#include <stdint.h>

#define VOICE_UART       uart0
#define VOICE_UART_TX_PIN 0
#define VOICE_UART_RX_PIN 1
#define VOICE_UART_BAUD   9600
#define VOICE_BUF_SIZE    64

void voice_uart_init(void);
void voice_uart_poll(void);
bool voice_uart_has_command(void);
uint8_t voice_uart_get_command_id(void);

#endif // VOICE_UART_H
