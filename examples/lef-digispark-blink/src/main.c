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
 * LED     PB1
 * TIMER0  PB0 (OC0A)
 * Button  PB2
 */

// Include ------------------------------------------------------------------

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "def_avr.h"
#include "LEF.h"

// Macros -------------------------------------------------------------------

#define PIN_TIMER B,0 
#define PIN_BUTTON B,2
#define PIN_SYSTICK B,1

// Datatypes ----------------------------------------------------------------

typedef enum {
    EVENT_SYSTICK = 0,
    EVENT_E,
    EVENT_BUTTON,
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
    LEF_Button_update(&button, !gpio_read(PIN_BUTTON));
}

static void hw_init(void) {
    
    ARDUINO_LED_INIT();
    gpio_init(PIN_BUTTON, false, true);	
    gpio_init(TIMER0_PIN_OC0A, true, false);
	
    TIMER0_CLK_PRES_1024();
    TIMER0_OCA(160);
    TIMER0_WGM_CTC();
    TIMER0_OCA_COM_TOGGLE();
    TIMER0_OCA_INT(true);
	
    sei();  // Enable all interrupts
}
int main(void) {
    LEF_Event event;

    LEF_Timer_init(&timer_e, EVENT_E);
    LEF_Timer_init(&timer_sys, EVENT_SYSTICK);
    LEF_Timer_start_repeat(&timer_sys, 10);

    LEF_Led_init(&led, LED_TRIPPLE_BLINK);
    LEF_Button_init(&button, EVENT_BUTTON);
   	LEF_Timer_start_single(&timer_e, 100);
    LEF_init();
    
    hw_init();
    
    while (true) {
        LEF_Wait(&event);
        switch (event.id) {
            case EVENT_BUTTON:
                if (event.func != 1)
                    break;
                LEF_Led_set(&led, LED_DOUBLE_BLINK);
                LEF_Timer_start_single(&timer_e, 100);
                break;
            case EVENT_E:
                LEF_Led_set(&led, LED_BLIP);
                break;
                
            case EVENT_SYSTICK:
                //gpio_toggle(PIN_TIMER);
                break;
                
        }
    }
}
