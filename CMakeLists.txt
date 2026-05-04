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
)

target_include_directories(vibecoding-kbd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(vibecoding-kbd PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_add_extra_outputs(vibecoding-kbd)
