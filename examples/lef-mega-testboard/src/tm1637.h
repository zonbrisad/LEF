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

#include "def_avr.h"

/**
 * GPIO pins 
 */
#define TM_CLK D,0
#define TM_DIO D,1

inline void tm_clk_high() { gpio_write(TM_CLK, 1); }
inline void tm_clk_low() { gpio_write(TM_CLK, 0); }
inline void dm_dio_high() { gpio_write(TM_DIO, 1); }
inline void tm_dio_low() { gpio_write(TM_DIO, 0); }

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