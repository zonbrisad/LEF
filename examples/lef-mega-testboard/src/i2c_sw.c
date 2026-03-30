
#include "i2c_sw.h"

#include "def_avr.h"

void i2c_init(void) {
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
    // scl_set(0);
    // i2c_delay();
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

    scl_set(0);
    i2c_delay();

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

    // release SDA for reading
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
    sda_set(!ack);
    
    scl_set(1);
    i2c_delay();
    scl_set(0);
    sda_set(1);

    return data;
}

uint8_t i2c_write_init(uint8_t address) {
    i2c_start();
    return i2c_write((address << 1) | I2C_WRITE);
}

uint8_t i2c_read_init(uint8_t address) {
    i2c_start();
    return i2c_write((address << 1) | I2C_READ);
}


uint8_t pcf_write(uint8_t address, uint8_t data) {
    i2c_write_init(address);
    i2c_write(data);
    i2c_stop();
}

uint8_t pcf_read(uint8_t address, uint8_t mask) {
    uint8_t data;
    pcf_write(address, mask);

    i2c_read_init(address);
    data = i2c_read(0);
    i2c_stop();
    return data;
}