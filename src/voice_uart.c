#include "voice_uart.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

static uint8_t uart_buf[VOICE_BUF_SIZE];
static volatile uint8_t buf_head = 0;
static volatile uint8_t buf_tail = 0;
static uint8_t last_command_id = 0;
static bool has_new_command = false;

static void uart_rx_byte(void) {
    if (uart_is_readable(VOICE_UART)) {
        uint8_t byte = uart_getc(VOICE_UART);
        uint8_t next = (buf_head + 1) % VOICE_BUF_SIZE;
        if (next != buf_tail) {
            uart_buf[buf_head] = byte;
            buf_head = next;
        }
    }
}

void voice_uart_init(void) {
    uart_init(VOICE_UART, VOICE_UART_BAUD);
    gpio_set_function(VOICE_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(VOICE_UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(VOICE_UART, false, false);
    uart_set_format(VOICE_UART, 8, 1, UART_PARITY_NONE);
}

// Call this from main loop to poll UART
void voice_uart_poll(void) {
    while (uart_is_readable(VOICE_UART)) {
        uint8_t byte = uart_getc(VOICE_UART);
        uint8_t next = (buf_head + 1) % VOICE_BUF_SIZE;
        if (next != buf_tail) {
            uart_buf[buf_head] = byte;
            buf_head = next;
        }
    }

    // Parse: look for 0xAA 0xXX pattern (common VC-02 protocol)
    // AA is header, XX is command ID
    // If no header found, treat any byte as a raw command for debugging
    while (buf_tail != buf_head) {
        uint8_t byte = uart_buf[buf_tail];
        buf_tail = (buf_tail + 1) % VOICE_BUF_SIZE;

        // Simple protocol: byte value IS the command ID (0 reserved for none)
        if (byte != 0) {
            last_command_id = byte;
            has_new_command = true;
            break;
        }
    }
}

bool voice_uart_has_command(void) {
    return has_new_command;
}

uint8_t voice_uart_get_command_id(void) {
    if (has_new_command) {
        has_new_command = false;
        return last_command_id;
    }
    return 0;
}
