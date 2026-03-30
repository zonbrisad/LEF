
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



void i2c_init(void);


/**
 * Send I2C start condition.
 * 
 * Pull SDA low, while SCL high.
 */
void i2c_start(void);

/**
 * Send I2C stop condition.
 * 
 * Pull SDA high while SCL is high.
 */
void i2c_stop(void);

uint8_t i2c_write(uint8_t data);

uint8_t i2c_read(uint8_t ack);

uint8_t i2c_write_init(uint8_t address);

uint8_t i2c_read_init(uint8_t address);

uint8_t pcf_write(uint8_t address, uint8_t data);

uint8_t pcf_read(uint8_t address, uint8_t mask);