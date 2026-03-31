/**
 * Driver for TM1637 7-segment LED display.
 * 
 * The TM1637 controlls four 7-segment LED displays. 
 * 
 * 
 * IMORTANT!
 * The databuss used by the controller is NOT compatible with I2C and can NOT be used on the same bus with other
 * I2C units. A separate bus is needed for the TM1637
 * 
 * Todo:
 * Add support for ":"
 * Add support for ","
 * Add support for letters
 */

#pragma once

#include <util/delay.h>

#include "def_avr.h"
/**
 * GPIO pins 
 */
#define TM_CLK D,0
#define TM_DIO D,1

/**
 * Timing
 */
inline static void tm_delay() { _delay_us(5); }


inline void tm_clk(bool val) { gpio_write(TM_CLK, val); }
inline void tm_dio(bool val) { gpio_write(TM_DIO, val); }

inline void tm_init() {
    gpio_init(TM_CLK, GPIO_OUTPUT, GPIO_NO_PULLUP);
    gpio_init(TM_DIO, GPIO_OUTPUT, GPIO_NO_PULLUP);
}

/**
 * Write value do display.
 * 
 * @param num Value to be displayed
 */
void tm_display_number(int num);

/**
 * Set display brightness.
 * 
 * @param brightness 0 = off, 7 = max brightness
 */
void tm_display_brightness(uint8_t brightness);