#include "i2c.h"

void app_main(void)
{
    initI2C();
    pca9548channel(PCA_ADDR, PCA_CH1);
    i2c_write_byte(0x70, 0x00, PCA_CH1);
}
