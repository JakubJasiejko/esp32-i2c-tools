/**
 * @file i2c.h
 * @author Jakub Jasiejko
 * @date 2025-03-30
 * @brief I2C function declarations for ESP32.
 *
 * @details
 * This header contains macros and function declarations for using the ESP32
 * I2C peripheral in master mode. It exposes initialization, 8/16-bit read and
 * write helpers, and PCA9548 multiplexer control.
 *
 */

 #ifndef I2C_H
 #define I2C_H
 
 #include "driver/i2c.h"
 #include <stdint.h>

 #define I2C_MASTER_NUM              I2C_NUM_0
 #define I2C_MASTER_FREQ_HZ          400000
 #define I2C_MASTER_TX_BUF_DISABLE   0
 #define I2C_MASTER_RX_BUF_DISABLE   0
 #define I2C_MASTER_SDA_IO           GPIO_NUM_21
 #define I2C_MASTER_SCL_IO           GPIO_NUM_22

 #define PCA_CH0                     0b00000001
 #define PCA_CH1                     0b00000010
 #define PCA_CH2                     0b00000100
 #define PCA_CH3                     0b00001000
 #define PCA_CH4                     0b00010000
 #define PCA_CH5                     0b00100000
 #define PCA_CH6                     0b01000000
 #define PCA_CH7                     0b10000000

 #define PCA_ADDR                    0x70
 #define PCA_NUM_OF_SENSORS          32

 
 /**
  * @brief Initializes the I2C bus in master mode.
  *
  * Configures SDA/SCL pins, clock speed, and installs the I2C driver.
  */
 void initI2C(void);
 
 /**
  * @brief Writes a single byte to an I2C device.
  *
  * @param dev_addr  7-bit device address
  * @param reg_addr  target register address
  * @param data      byte to write
  * @return 0x00 on success, 0xFF on error
  */
 uint8_t i2c_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
 
 /**
  * @brief Reads a single byte from an I2C device.
  *
  * @param dev_addr  7-bit device address
  * @param reg_addr  register address to read from
  * @param data      destination buffer
  * @return 0x00 on success, 0xFF on error
  */
 uint8_t i2c_read_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data);
 
 /**
  * @brief Writes a 16-bit value to an I2C device in a single transaction.
  *
  * @param dev_addr  7-bit device address
  * @param reg_addr  starting register address (MSB)
  * @param value     16-bit value to write (MSB first, then LSB)
  * @return 0x0000 on success, 0xFFFF on transmission failure
  */
 uint16_t i2c_write16(uint8_t dev_addr, uint8_t reg_addr, uint16_t value);
 
 /**
  * @brief Reads a 16-bit value from an I2C device in a single transaction.
  *
  * @param dev_addr  7-bit device address
  * @param reg_addr  starting register address (MSB)
  * @return 16-bit value (MSB:LSB) or 0xFFFF on error
  */
 uint16_t i2c_read16(uint8_t dev_addr, uint8_t reg_addr);
 
 /**
  * @brief Selects the active channel on a PCA9548 multiplexer.
  *
  * @param channel_addr 7-bit PCA9548 address
  * @param bitmask      channel mask to activate, for example PCA_CH1 = 0x02
  *
  * @details
  * If the I2C transaction fails, the function sends a diagnostic UART frame
  * in the format [0xAA][0xBB][checksum].
  */
 void pca9548channel(uint8_t channel_addr, uint8_t bitmask);
 


int32_t i2c_read24(uint8_t dev_addr);

/**
  * @brief Sends a single command byte to an I2C device such as ADS1219.
  *
  * Starts an I2C transaction that only sends a command byte such as RESET,
  * START, POWERDOWN, or RDATA without a register address.
  *
  * @param dev_addr 7-bit I2C device address without the R/W bit
  * @param command command byte to send, for example 0x06 for RESET
  * @return 0x00 on success, 0xFF on error
  */

uint8_t i2c_send_command(uint8_t dev_addr, uint8_t command);
 
 
 #ifdef __cplusplus
 
 #endif
 
 #endif // I2C_H
 
