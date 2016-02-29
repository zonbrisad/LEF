/**
 * @file main.c
 * @brief Testprogram for eventqueue library
 * 
 * Test different aspects of EventQueue library.
 * Hardware is standard Arduino UNO
 * Debugport at 19200 bit
 * A switch is connected on PB4 (DI 12) (active low)
 *
 *
 * @author Peter Malmberg <peter.malmberg@gmail.com>
 * @date 2012 05
 *---------------------------------------------------------------------------
 *
 * @todo support for command interpreter
 * @todo support for ADC
 * @todo support for rotary encoder
 * @todo support for small keyboard
 */

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h> 
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "def.h"
#include "uart/uart.h"
#include "commandInterpreter.h"
#include "queue.h"
#include "sev_timer.h"


/**
 * Defs.
 * -----------------------------------------------------------------------
 */

#define VERSION           "0.2"
#define UART_BAUD_RATE    19200


// Timer 0 10 ms setting at 16 MHz freq.
#define TIMER0_10MS       0x64

/**
 * Different 16 bit timer settings at 16MHz xtal.
 */
#define TIMER1_1HZ        0xC2F7
#define TIMER1_2HZ        0xE17B
#define TIMER1_4HZ        0xF0BE
#define TIMER1_8HZ        0xF85F

#define TIMER1_CNT        TIMER1_1HZ


#define BUTTON_BASE         30

#define EVENT_TIMER1        0
#define EVENT_TIMER0        1
#define EVENT_1S            2
#define EVENT_2S            3
#define EVENT_4S            4
#define EVENT_CMD           5
#define EVENT_BUT_DOWN      BUTTON_BASE + BUTTON_PRESS_EVENT
#define EVENT_BUT_UP        BUTTON_BASE + BUTTON_RELEASE_EVENT
#define EVENT_BUT_LONG      BUTTON_BASE + BUTTON_LONG_PRESS_EVENT




/**
 * Prototypes
 * -----------------------------------------------------------------------
 */


/**
 * Variables
 * -----------------------------------------------------------------------
 */


static FILE mystdio = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);

event_queue que;    // event queue
button but;         // button
uint8_t blink;      // LED blink state
sev_timer eTimer1;  // software timer 1
sev_timer eTimer2;  // software timer 2
sev_timer eTimer3;  // software timer 3


void test() {

}
const PROGMEM commandStruct cmdTable[] = {
  {test,       "test",      "test"},
//  {printInfo,         "info",         "Print basic info"},
//  {printTemperature,  "temperature",  "Read temperature cont."},
//  {Moist,             "moist",        "Read moist cont."},
//  {TimerPotTest,      "timer",        "Test timer pot"},
  {NULL,              NULL,            NULL}
};

/**
 * ISR's
 * -----------------------------------------------------------------------
 */

ISR(TIMER0_OVF_vect) {
  static queue_element qel;
  static uint8_t cnt;

  TIMER0_RELOAD(TIMER0_10MS);        // reload counter of timer 0

  qel.source = EVENT_TIMER0;

  button_que_update(&que, &but, BUTTON_BASE);

  if (blink) {
    if (cnt==30) {                    // blink every 500 ms
      ARD_LED_TOGGLE();
      cnt = 0;
    }
    cnt++;
  }

  // update software timers
  sev_timer_update(&eTimer1, &que, EVENT_1S);
  sev_timer_update(&eTimer2, &que, EVENT_2S);
  sev_timer_update(&eTimer3, &que, EVENT_4S);

  sev_cmd_update(&que, EVENT_CMD);
}


ISR(TIMER1_OVF_vect) {
  static queue_element qel;

  TIMER1_RELOAD(TIMER1_CNT);        // reload counter of timer 1

  //qel.source = EVENT_TIMER1;
  //queue_send(&que, &qel);
}

/**
 *
 * -----------------------------------------------------------------------
 */

/**
 * Initiate hardware peripherials.
 */
void init_hw() {

  ARD_LED_INIT();                   // init arduino LED port

  uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));		// init uarts

	stdout = &mystdio;                // configure stdout to UART 0

	TIMER0_PRES_1024();               // set prescaler of timer 0
	TIMER0_OVF_IE();                  // enable overflow interrupt of timer 0
	TIMER0_RELOAD(TIMER0_10MS);       // reload counter of timer 0

	TIMER1_PRES_1024();               // set prescaler of timer 1
	TIMER1_OVF_IE();                  // enable overflow interrupt of timer 1
	TIMER1_RELOAD(TIMER1_CNT);        // reload counter of timer 1


  but.port = &PINB;                 // initiate button
  but.bit  = PB4;
  button_init(&but, &PORTB);

  sev_timer_init(&eTimer1);
  sev_timer_init(&eTimer2);
  sev_timer_init(&eTimer3);
  sev_timer_start_repeat(&eTimer1, 100);  // set as 100 ticks repeating timer
  sev_timer_start_repeat(&eTimer2, 200);  // set as 100 ticks repeating timer

  sev_cmd_init(cmdTable);

	sei();														// enable interrupts
}



int main() {
  queue_element qel;
  queue_init(&que);

  init_hw();

  printf_P(PSTR("\n----- EventQueue/Button Tester ver %s -----\n"), VERSION);

  while(1) {
    queue_wait(&que, &qel);

    //printf_P(PSTR("Event received src %d\n"), qel.source);
    switch (qel.source) {
      case EVENT_CMD:       sev_cmd_exec(1);
                            break;
      case EVENT_TIMER0:    printf("Timer 0 event\n"); break;
      case EVENT_TIMER1:    printf("Timer 1 event\n"); break;
      case EVENT_BUT_DOWN:  printf("Button pressed event\n");
                            ARD_LED_TOGGLE();
                            blink = 0;
                            break;
      case EVENT_BUT_UP:    printf("Button released event\n"); break;
      case EVENT_BUT_LONG:  printf("Button hold down event\n");
                            blink = 1;                                // start blinking
                            sev_timer_start_single(&eTimer3, 400);    // start single timeout
                            break;
      case EVENT_1S:        printf_P(PSTR("Software timer 1s\n")); break;
      case EVENT_2S:        printf_P(PSTR("Software timer 2s\n")); break;
      case EVENT_4S:        printf_P(PSTR("Software timer 4s\n"));
                            blink = 0;
                            break;
    }
  }
}
