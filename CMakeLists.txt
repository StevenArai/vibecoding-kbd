cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
if(NOT PICO_SDK_PATH)
    message(FATAL_ERROR "PICO_SDK_PATH environment variable is not set")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(vibecoding-kbd C CXX ASM)

pico_sdk_init()

add_executable(vibecoding-kbd
    src/main.c
    src/st7735.c
    src/ws2812.c
    src/matrix.c
    src/encoder.c
    src/voice_uart.c
    src/buzzer.c
)

pico_generate_pio_header(vibecoding-kbd ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio)

target_include_directories(vibecoding-kbd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(vibecoding-kbd PRIVATE
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_pio
    hardware_pwm
    hardware_uart
    hardware_dma
    hardware_clocks
    hardware_irq
    hardware_gpio
)

pico_add_extra_outputs(vibecoding-kbd)

pico_enable_stdio_usb(vibecoding-kbd 1)
pico_enable_stdio_uart(vibecoding-kbd 0)
