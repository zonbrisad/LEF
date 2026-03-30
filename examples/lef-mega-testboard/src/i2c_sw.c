
#include "i2c_sw.h"

#include "def_avr.h"

void i2c_init(void) {
    // gpio_init(I2C_SCL_PIN, GPIO_INPUT, GPIO_NO_PULLUP);
    // gpio_init(I2C_SDA_PIN, GPIO_INPUT, GPIO_NO_PULLUP);
    
    // gpio_write(I2C_SCL_PIN, 0);
    // gpio_write(I2C_SDA_PIN, 0);
    i2c_pin_init();
    scl_set(1);
    sda_set(1);
}

void i2c_start(void) {
    sda_set(1);
    scl_set(1);
    i2c_delay();
    sda_set(0);
    i2c_delay();
    scl_set(0);
    i2c_delay();
}

void i2c_stop(void) {
    sda_set(0);
    i2c_delay();
    scl_set(1);
    i2c_delay();
    sda_set(1);
    i2c_delay();
}

uint8_t i2c_write(uint8_t data) {
    uint8_t ack;

    for (uint8_t i = 0; i < 8; i++) {
        sda_set(data & 0x80);

        data <<= 1;
        i2c_delay();

        scl_set(1);
        i2c_delay();
        scl_set(0);
        i2c_delay();
    }

    // Read ACK bit
    sda_set(1);
    i2c_delay();
    scl_set(1);
    i2c_delay();
    ack = (sda_read() == 0);  // ACK = 0 (low) from slave
    scl_set(0);

    return ack ? 0 : 1;  // 0 = success (ACK), 1 = NACK
}

uint8_t i2c_read(uint8_t ack) {
    uint8_t data = 0;

    // SDA_HIGH();  // release SDA for reading
    sda_set(1);

    for (uint8_t i = 0; i < 8; i++) {
        scl_set(1);
        i2c_delay();
        data <<= 1;
        if (sda_read()) data |= 1;
        scl_set(0);
        i2c_delay();
    }

    // Send ACK or NACK
    if (ack)
        sda_set(0);
    else
        sda_set(1);

    scl_set(1);
    i2c_delay();
    scl_set(0);
    sda_set(1);

    return data;
}
