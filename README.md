# esp32-i2c-tools

Zestaw prostych narzędzi I2C dla ESP32: inicjalizacja magistrali, odczyt i zapis 8/16/24 bitów oraz przełączanie kanałów PCA9548.

## Co zawiera
- `initI2C()`
- `i2c_write_byte()` i `i2c_read_byte()`
- `i2c_write16()` i `i2c_read16()`
- `i2c_read24()`
- `i2c_send_command()`
- `pca9548channel()`

## Pliki
- `include/i2c.h`
- `src/i2c.c`

## Szybki start
```c
#include "i2c.h"

void app_main(void)
{
    initI2C();
    pca9548channel(PCA_ADDR, PCA_CH0);
    i2c_write_byte(0x50, 0x0C, 0x01);
}
```

## Domyślna konfiguracja
- I2C port: `I2C_NUM_0`
- SDA: `GPIO21`
- SCL: `GPIO22`
- clock: `400 kHz`

Pełny przykład znajdziesz w `examples/basic_usage/basic_usage.c`.
