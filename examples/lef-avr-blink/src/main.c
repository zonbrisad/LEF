/**
 *---------------------------------------------------------------------------
 * @brief    Blink LED on arduino using LEF
 *
 * @file     main.c
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2026-02-25
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */

// Include ------------------------------------------------------------------

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "main.h"
#include "LEF.h"
#include "LEF_Led.h"
#include "LEF_Timer.h"
#include "LEF_Button.h"
#include "def_avr.h"

// Macros -------------------------------------------------------------------

#define UART_BAUD_RATE 9600

// Prototypes ---------------------------------------------------------------

void hw_init(void);

// Datatypes ----------------------------------------------------------------


// Variables ----------------------------------------------------------------

static FILE mystdout = FDEV_SETUP_STREAM((void*)uart_putc, NULL, _FDEV_SETUP_WRITE);

LEF_Timer  timer;
LEF_Led    led;

// Code ---------------------------------------------------------------------

ISR(TIMER1_COMPA_vect) {

  TIMER1_RELOAD(0);

  if (LEF_Led_update(&led)) {
		ARDUINO_LED_ON();
	} else {
		ARDUINO_LED_OFF();
	}


}    

void hw_init(void) {

  ARDUINO_LED_INIT();

  stdout = &mystdout;
  uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
  
  // Timer 1 (16 bit)
  TIMER1_CLK_PRES_256(); // set prescaler to 1/1024
  TIMER1_OCA_SET(625);
  TIMER1_OCA_IE();        // enable output compare A interrupt
  sei();  // Enable all interrupts
} 
int main() {
  LEF_Event event;
  
  hw_init();
	printf("\n\nLEF AVR blink LED demo\n\n");
  
  LEF_init();
  LEF_Led_init(&led, LED_FAST_BLINK);
  
	while (1) {
    LEF_Wait(&event);
	}
  return 0;
}
