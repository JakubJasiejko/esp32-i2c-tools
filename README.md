# esp32-i2c-tools

A compact set of I2C helpers for ESP32: bus initialization, 8/16/24-bit read and write operations, and PCA9548 channel switching.

## Features
- `initI2C()`
- `i2c_write_byte()` i `i2c_read_byte()`
- `i2c_write16()` i `i2c_read16()`
- `i2c_read24()`
- `i2c_send_command()`
- `pca9548channel()`

## Files
- `include/i2c.h`
- `src/i2c.c`

## Quick start
```c
#include "i2c.h"

void app_main(void)
{
    initI2C();
    pca9548channel(PCA_ADDR, PCA_CH0);
    i2c_write_byte(0x50, 0x0C, 0x01);
}
```

## Default configuration
- I2C port: `I2C_NUM_0`
- SDA: `GPIO21`
- SCL: `GPIO22`
- clock: `400 kHz`

See `examples/basic_usage/basic_usage.c` for a complete example.
