/**
 *---------------------------------------------------------------------------
 * @brief    Simple app to blink LED on digispark (ATTiny85)
 *
 * @file     main.c
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2026-03-16
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */

/* Hardware description
 *
 * LED   PB1
 * TIMER PB0
 */

// Include ------------------------------------------------------------------

#include "main.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "def_avr.h"
#include "LEF.h"

// Macros -------------------------------------------------------------------

#define TIMER_PIN B,0 
#define BUTTON_PIN B,5

// Prototypes ---------------------------------------------------------------

void hw_init(void);

// Datatypes ----------------------------------------------------------------

typedef enum {
    EVENT_SYSTICK = 0,
    EVENT_E,
    EVENT_BUTTON
} lef_event_t;

// Variables ----------------------------------------------------------------

LEF_Timer  timer_sys;
LEF_Timer  timer_e;
LEF_Led    led;
LEF_Button button;

// Code ---------------------------------------------------------------------

ISR(TIMER0_COMPA_vect) {
    ARDUINO_LED_SET(LEF_Led_update(&led));
    LEF_Timer_update(&timer_sys);
    LEF_Timer_update(&timer_e);
    LEF_Button_update(&button, !gpio_read(BUTTON_PIN));
}

void hw_init(void) {
    TIMER0_CLK_PRES_1024();
    TIMER0_OCA_SET(160);
    TIMER0_WGM_CTC();
    TIMER0_OCA_IE();
    
    ARDUINO_LED_INIT();
    gpio_init(TIMER_PIN, true, false);
    gpio_init(BUTTON_PIN, false, true);
    
    sei();  // Enable all interrupts
}
int main(void) {
    LEF_Event event;
    
    LEF_Timer_init(&timer_sys, EVENT_SYSTICK);
    LEF_Timer_start_repeat(&timer_sys, 10);
    LEF_Timer_init(&timer_e, EVENT_E);

    LEF_Led_init(&led, LED_BLIP);
    LEF_Button_init(&button, EVENT_BUTTON);
    LEF_init();
    
    hw_init();
    gpio_write(TIMER_PIN, true);
    
    while (true) {
        LEF_Wait(&event);
        switch (event.id) {
            case EVENT_BUTTON:
                LEF_Led_set(&led, LED_DOUBLE_BLINK);
                LEF_Timer_start_single(&timer_e, 300);
                break;
            case EVENT_E:
                LEF_Led_set(&led, LED_SLOW_BLIP);

            case EVENT_SYSTICK:
                gpio_toggle(TIMER_PIN);
                break;

        }
    }
}
