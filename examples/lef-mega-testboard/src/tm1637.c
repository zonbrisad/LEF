
#include "tm1637.h"

#include <avr/io.h>
#include <util/delay.h>
#include "def_avr.h"





// --- Start condition ---
static void tm_start() {
    tm_dio(1);
    tm_clk(1);
    tm_delay();
    tm_dio(0);
}

// --- Stop condition ---
static void tm_stop() {
    tm_clk(0);
    tm_delay();
    tm_dio(0);
    tm_delay();
    tm_clk(1);
    tm_delay();
    tm_dio(1);
}

// --- Write byte ---
static void tm_write(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        tm_clk(0);

        tm_dio(data & 0x01);

        tm_delay();
        tm_clk(1);
        tm_delay();

        data >>= 1;
    }

    // ACK (ignore for simplicity)
    tm_clk(0);
    tm_dio(1);
    tm_delay();
    tm_clk(1);
    tm_delay();
    tm_clk(0);
}

const uint8_t digit_map[] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F   // 9
};

void tm_display_number(int num) {
    uint8_t digits[4];

    digits[0] = digit_map[(num / 1000) % 10];
    digits[1] = digit_map[(num / 100) % 10];
    digits[2] = digit_map[(num / 10) % 10];
    digits[3] = digit_map[num % 10];

    // Command 1: auto increment
    tm_start();
    tm_write(0x40);
    tm_stop();

    // Command 2: set address + data
    tm_start();
    tm_write(0xC0);

    for (uint8_t i = 0; i < 4; i++) {
        tm_write(digits[i]);
    }
    tm_stop();
}

void tm_display_brightness(uint8_t brightness) {
    tm_start();
    tm_write(0x88 | (brightness & 0b0000111));  // display ON, max brightness
    tm_stop();
}