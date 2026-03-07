/*
 * GccApplication1.c
 *
 * Created: 2013-10-09 20:23:58
 *  Author: Peter
 */ 

/**
 * Includes
 * -----------------------------------------------------------------------
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdio.h>
#include "uart/uart.h"
#include "def.h"
#include "commandInterpreter.h"
#include "DHTxx/DHTxx.h"


/**
 * Defs.
 * -----------------------------------------------------------------------
 */

#define VERSION           "0.1"
#define UART_BAUD_RATE    57600



/**
 *  Macros
 * -----------------------------------------------------------------------
 */

#define IS_KEYPRESSED()  (uart_getc()!=UART_NO_DATA)
#define WAIT_FOR_KEYPRESSED() while(!IS_KEYPRESSED){}


/**
 * Prototypes
 * -----------------------------------------------------------------------
 */

void cmdReset();
void cmdWdtReset();
void ledOn();
void ledOff();
void readMoist();
/**
 * Variables
 * -----------------------------------------------------------------------
 */

static int uart_putchar(char c, FILE *stream) {
  uart_putc(c);
  return 0;
}

static FILE mystdio = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_RW);

const PROGMEM commandStruct cmdTable[] = {
//	{printCommands,     "help",         "This help info"},
	{cmdReset,			"reset",		"Reset MCU"},
	{ledOn,				"ledon",		"LED on"},
	{ledOff,			"ledoff",		"LED off"},	
	{readMoist,         "moist",        "Read moist from DHTxx sensor"},
	{NULL,              "",             ""}
};

/**
 * Functions
 * -----------------------------------------------------------------------
 */


void cmdWdtReset() {
	wdt_enable(WDTO_120MS);
}

void ledOn() {
	ARDUINO_LED_ON();
}

void ledOff() {
	ARDUINO_LED_OFF();
}

void cmdReset() {
	RESET();
}


void readMoist()  {
	int16_t moist;
	int16_t temperature;
	while (1) {
		dht_init();

		dht_read(&temperature, &moist);
		printf_P(PSTR("Temperature %5d Moist %5d\n"), temperature, moist);
		_delay_ms(200);

		if (IS_KEYPRESSED()) {
			break;
		}
	}
}

/**
 * Initiate hardware peripherials.
 */
void init_hw() {

  wdt_disable();
  
  
  
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));      // init uart
  stdout = &mystdio;

  dht_init();
  ARDUINO_LED_INIT();

  sei();                                                   // enable interrupts

}

int main(void) {
	
	init_hw();
	ARDUINO_LED_ON();
	
	printf("\n\nArduino Mega Component Tester\n");
	printf("--------------------------------------------\n");
	printf("Ver "__DATE__ " " __TIME__ "\n");
	
	if (EXTERNAL_RESET()) {
		printf("External reset\n");
	}
	if (POWER_ON_RESET()) {
		printf("Power on reset\n");
	}
	if (WATCH_DOG_RESET()) {
		printf("Watchdog reset\n");
	}
	CLEAR_RESETS();
	
	commandInterpreter(cmdTable);
}