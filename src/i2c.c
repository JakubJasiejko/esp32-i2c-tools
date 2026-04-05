/**
 * @file i2c.c
 * @author Jakub Jasiejko
 * @date 2025-03-30
 * @brief Basic I2C master implementation for ESP32.
 *
 * @details
 * This module provides a small collection of I2C helpers for ESP32,
 * including bus initialization, byte and word transfers, 24-bit reads,
 * command writes, and PCA9548 channel selection.
 */


#include "i2c.h"
#include "uart.h"

/**
 * @brief Initializes the I2C peripheral in master mode.
 *
 * The function configures SDA/SCL, enables pull-ups, applies the clock
 * configuration from `i2c.h`, and installs the ESP-IDF I2C driver.
 */
void initI2C() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief Writes a single byte to a selected I2C register.
 *
 * The function creates an I2C transaction, sends the device address,
 * register address, and payload byte, and terminates the transfer with STOP.
 *
 * @param dev_addr 7-bit I2C device address without the R/W bit.
 * @param reg_addr Register address to write to.
 * @param data Byte value to be written.
 * @return 0x00 on success, 0xFF on error.
 */
uint8_t i2c_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    return (ret == ESP_OK) ? 0x00 : 0xFF;
}

/**
 * @brief Reads a single byte from a selected I2C register.
 *
 * The function first writes the register address and then starts a second
 * transaction to read back one byte from the device.
 *
 * @param dev_addr 7-bit I2C device address.
 * @param reg_addr Register address to read from.
 * @param data Pointer to the destination byte.
 * @return 0x00 on success, 0xFF on error or invalid pointer.
 */
uint8_t i2c_read_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data) {
    if (data == NULL) return 0xFF;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) return 0xFF;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return (ret == ESP_OK) ? 0x00 : 0xFF;
}

/**
 * @brief Writes a 16-bit value to an I2C device.
 *
 * The value is split into MSB and LSB and written in a single transaction.
 *
 * @param dev_addr 7-bit I2C device address.
 * @param reg_addr Starting register address for the MSB.
 * @param value 16-bit value to write.
 * @return 0x0000 on success, 0xFFFF on error.
 */
uint16_t i2c_write16(uint8_t dev_addr, uint8_t reg_addr, uint16_t value) {
    uint8_t msb = (value >> 8) & 0xFF;
    uint8_t lsb = value & 0xFF;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, msb, true);
    i2c_master_write_byte(cmd, lsb, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    return (ret == ESP_OK) ? 0x0000 : 0xAAAA;
}

/**
 * @brief Reads a 16-bit value from an I2C device.
 *
 * The function writes the starting register address and then reads two bytes
 * back as a big-endian 16-bit value.
 *
 * @param dev_addr 7-bit I2C device address.
 * @param reg_addr Starting register address for the MSB.
 * @return 16-bit value, or 0xFFFF on error.
 */
uint16_t i2c_read16(uint8_t dev_addr, uint8_t reg_addr) {
    uint8_t data[2] = {0};

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) return 0xAAAA;

    return ((uint16_t)data[0] << 8) | data[1];
}

/**
 * @brief Sends a single command byte to an I2C device.
 *
 * This helper is useful for parts such as ADS1219 that accept simple command
 * bytes without a register address.
 *
 * @param dev_addr 7-bit I2C device address without the R/W bit.
 * @param command Command byte to send.
 * @return 0x00 on success, 0xFF on error.
 */

uint8_t i2c_send_command(uint8_t dev_addr, uint8_t command) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, command, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    return (ret == ESP_OK) ? 0x00 : 0xFF;
}

/**
 * @brief Reads a 24-bit value (3 bytes) from an I2C device.
 *
 * The result is returned as a signed 32-bit integer with proper sign extension,
 * which makes it suitable for converters such as ADS1219.
 *
 * @param dev_addr 7-bit I2C device address without the R/W bit.
 * @return Signed 24-bit value expanded to int32_t, or 0x7FFFFFFF on error.
 */
int32_t i2c_read24(uint8_t dev_addr) {
    uint8_t data[3] = {0};

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 3, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) return 0x7FFFFFFF;  // error value

    int32_t result = (data[0] << 16) | (data[1] << 8) | data[2];
    if (result & 0x800000) result |= 0xFF000000;  // sign extension for negative values

    return result;
}




/**
 * @brief Selects the active channel on a PCA9548 multiplexer.
 *
 * The requested channel mask is written to the PCA9548 control register.
 * When the transfer fails, a diagnostic UART frame is sent.
 *
 * @param channel_addr 7-bit PCA9548 device address.
 * @param bitmask Channel mask, for example 0x01 for CH0 or 0x02 for CH1.
 */
void pca9548channel(uint8_t channel_addr, uint8_t bitmask) {
    uint8_t startByte = 0xAA;
    uint8_t error_byte = 0xBB;

    if (i2c_write_byte(channel_addr, 0x00, bitmask) == 0x00) {
        return; // Success: no diagnostic frame needed
    }

    uint8_t timeoutSum = checksum(startByte, error_byte);
    uart_write_bytes(UART_PORT, &startByte, 1);
    uart_write_bytes(UART_PORT, &error_byte, 1);
    uart_write_bytes(UART_PORT, &timeoutSum, 1);
}
