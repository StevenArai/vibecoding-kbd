# vibecoding-kbd

RP2040-based custom keyboard firmware using the Pico SDK.

## Prerequisites

- [Pico SDK 2.x](https://github.com/raspberrypi/pico-sdk) — path set via `PICO_SDK_PATH` environment variable
- CMake ≥ 3.13
- ARM GCC toolchain (`arm-none-eabi-gcc`)

## Build

```bash
mkdir build && cd build
cmake ..
make -j4
```

The output `.uf2` file can be flashed to an RP2040 board via USB boot mode.

## Pinout

Rows and columns are defined in `src/main.c` — adjust the `row_pins` and `col_pins` arrays to match your PCB layout.

I2C pins (for OLED display):
- SDA: GPIO 14
- SCL: GPIO 15
