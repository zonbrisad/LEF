
#include <util/delay.h>
#include "def_avr.h"



#define I2C_READ  1
#define I2C_WRITE 0

#define I2C_SCL_PIN D, 0
#define I2C_SDA_PIN D, 1

inline void i2c_pin_init(void) {
    gpio_init(I2C_SCL_PIN, GPIO_INPUT, GPIO_NO_PULLUP);
    gpio_init(I2C_SDA_PIN, GPIO_INPUT, GPIO_NO_PULLUP);

    gpio_write(I2C_SCL_PIN, 0);
    gpio_write(I2C_SDA_PIN, 0);
}

inline void sda_set(uint8_t state) { gpio_direction(I2C_SDA_PIN, (state ? GPIO_INPUT : GPIO_OUTPUT) ); }
inline void scl_set(uint8_t state) { gpio_direction(I2C_SCL_PIN, (state ? GPIO_INPUT : GPIO_OUTPUT) ); }
inline bool sda_read(void) { return gpio_read(I2C_SDA_PIN); }

inline void i2c_delay(void) { _delay_us(5); }


/**
 * Initiate i2c bus
 */
void i2c_init(void);


/**
 * Send i2c start condition.
 * 
 * Pull SDA low, while SCL high.
 */
void i2c_start(void);

/**
 * Send i2c stop condition.
 * 
 * Pull SDA high while SCL is high.
 */
void i2c_stop(void);

/**
 * Write a byte to bus.
 * 
 * @return acknowlge 1 if true, 0 if not
 */
uint8_t i2c_write(uint8_t data);

/** 
 * Read a byte from bus.
 * 
 * @param ack Acknowledge to slave 1=yes, 0=no
 */
uint8_t i2c_read(uint8_t ack);

/**
 * Initiate write data sequence to slave
 * 
 * @param address address to slave 
 * @return acknowlge 1 if true, 0 if not
 */
uint8_t i2c_write_init(uint8_t address);

/**
 * Initiate read data sequence to slave
 * 
 * @param address address to slave 
 * @return acknowlge 1 if true, 0 if not
 */
uint8_t i2c_read_init(uint8_t address);

/**
 * Set gpio pins on PCF8574 chip.
 * 
 * @param address Address to chip
 * @param data    Output gpio values
 */
uint8_t pcf_write(uint8_t address, uint8_t data);

/**
 * Read gpio pins on PCF8574 chip
 * 
 * @param address Address to chip
 * @param mask Bitmask for gpios to bee read
 * @return status of gpio pins
 */
uint8_t pcf_read(uint8_t address, uint8_t mask);