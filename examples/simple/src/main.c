/**
 *---------------------------------------------------------------------------
 * @brief    A simple LEF test
 *
 * @file     main.c
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2023-03-31
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "uart.h"
#include "def.h"

#include "LEF.h"



#define UART_BAUD_RATE 57600

#define BUZZER_PIN PD3

#define BUZZER_ON() PORTD &= ~(1<<BUZZER_PIN)
#define BUZZER_OFF() PORTD |= (1<<BUZZER_PIN)

#define LED_RED_ON() PORTB |= (0x01 << PB4)
#define LED_RED_OFF() PORTB &= ~(0x01 << PB4)
#define LED_GREEN_ON() PORTB |= (0x01 << PB3)
#define LED_GREEN_OFF() PORTB &= ~(0x01 << PB3)

#define ROT_CLK()  PINC | (1 << PC0)
#define ROT_DATA() PINC | (1 << PC1)


typedef enum {
	EVENT_Timer1 = 0,
	EVENT_Timer2,
	EVENT_Button,
	EVENT_Rotary
} Events;

void hw_init(void);

void cmdBrp(void);
void cmdDBeep(void);
void cmdTBeep(void);
void cmdBuzon(void);
void cmdBuzoff(void);
void cmdBeep(void);
void cmdSBeep(void);
void cmdLBeep(void);
void cmdBlink(void);
void cmdLedOn(void);
void cmdLedOff(void);
void cmdEvOn(void);
void cmdEvOff(void);
void cmdHelp(void);



LEF_Timer  timer1;
LEF_Timer  timer2;
LEF_Led    led;
LEF_LedRG  ledrg;
LEF_Button button;
LEF_Rotary rotary;

char evOn = 0;

static FILE mystdout = FDEV_SETUP_STREAM((void*)uart_putc, NULL, _FDEV_SETUP_WRITE);

const PROGMEM LEF_CliCmd cmdTable[] = {
	{cmdBuzon,   "buzon",    "Buzzer on"},
	{cmdBuzoff,  "buzoff",   "Buzzer off"},
	{cmdBeep,    "beep",     "Make a beep"},	
	{cmdSBeep,   "sbeep",    "Make a short beep"},
	{cmdLBeep,   "lbeep",    "Make a long beep"},
	{cmdDBeep,   "dbeep",    "Make a double beep"},
	{cmdTBeep,   "tbeep",    "Make a tripple beep"},
	{cmdBrp,     "brp",      "BRP sound"},

	{cmdLedOn,  "ledon",     "Turn led on"},
	{cmdLedOff, "ledoff",    "Turn led off"}, 
	{cmdBlink,  "blink",     "Make led blink once"},

	{cmdEvOn,   "evon",      "Turn event on"},
	{cmdEvOff,  "evoff",     "Turn event off"},
	
	{cmdHelp,   "help",      "Show help"},
};


void cmdBrp(void) {
	LEF_Buzzer_set(LEF_BUZZER_BRP);
}

void cmdTBeep(void) {
	LEF_Buzzer_set(LEF_BUZZER_TRIPPLE_BEEP);
}

void cmdDBeep(void) {
	LEF_Buzzer_set(LEF_BUZZER_DOUBLE_BEEP);
}

void cmdBuzon(void) {
	LEF_Buzzer_set(LEF_BUZZER_ON);
}

void cmdBuzoff(void) {
	LEF_Buzzer_set(LEF_BUZZER_OFF);
}

void cmdBeep(void) {
	LEF_Buzzer_set(LEF_BUZZER_BEEP);
}

void cmdSBeep(void) {
	LEF_Buzzer_set(LEF_BUZZER_SHORT_BEEP);
}

void cmdLBeep(void) {
	LEF_Buzzer_set(LEF_BUZZER_LONG_BEEP);
}

void cmdBlink(void) {
  LEF_LedSetState(&led, LED_SINGLE_BLINK);
}

void cmdLedOn(void) {
  LEF_LedSetState(&led, LED_ON);
}

void cmdLedOff(void) {
  LEF_LedSetState(&led, LED_OFF);
}

void cmdEvOn(void) {
	evOn = 1;
}

void cmdEvOff(void) {
	evOn = 0;
}

void cmdHelp(void) {
	LEF_CliPrintCommands(cmdTable);
}


ISR(PCINT1_vect) {
//	LEF_Event event;
	char ch;
	
	ch = PINC;
	LEF_Rotary_update(&rotary, (ch & (1<<PC0)), (ch & (1<<PC1)));

	//event.id = LEF_EVENT_TEST;
	//LEF_Send(&event);
}

ISR(TIMER1_COMPA_vect) {
	char ch;
//	LEF_Event event;
	
  TIMER1_RELOAD(0);

	//	event.id = LEF_SYSTICK_EVENT;
	//	LEF_QueueWait(&StdQueue, &event);
	
	
	LEF_TimerUpdate(&timer1);
	LEF_TimerUpdate(&timer2);
	
	if (LEF_LedUpdate(&led)) {
		ARDUINO_LED_ON();
	} else {
		ARDUINO_LED_OFF();
	}

	ch = LEF_LedRGUpdate(&ledrg);
	if (ch & 0x01) 
		LED_RED_ON();
	else
		LED_RED_OFF();

	if (ch & 0x02) 
		LED_GREEN_ON();
	else
		LED_GREEN_OFF();
	
	
	LEF_Button_update(&button, (PIND & (1<<PIN7))==0  );

//	ch = PINC;
//	LEF_Rotary_update(&rotary, (ch & (1<<PC0)), (ch & (1<<PC1)));
	
	if (LEF_Buzzer_update() > 0) {
		BUZZER_ON();
	} else {
		BUZZER_OFF();
	}

}

void hw_init(void) {
	stdout = &mystdout;
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

	ARDUINO_LED_INIT();
	
	// Timer 1 (16 bit)
	TIMER1_CLK_PRES_256(); // set prescaler to 1/1024
	TIMER1_OCA_SET(625);
	TIMER1_OCA_IE();        // enable output compare A interrupt
	
	//set_sleep_mode(SLEEP_MODE_IDLE);
	//sleep_enable();

	// Activate pullup for click button
	PORTD = (0x01 << PIN7);

	// Configure buzzer port
	DDRD |= (0x01 << BUZZER_PIN);
	PORTD |= (0x01 << BUZZER_PIN);

	// Red Green LED
	DDRB |= (0x01 << PB4) | (0x01 << PB3);

	// Rotary encoder input pullup activation
	PORTC |= (1<<PC0) | (1<<PC1);

	// Pin Change Mask Register 1 
	PCMSK1 |= (1<<PCINT8);

	// Enable Pin change interruåt
	PCICR |= (1<<PCIE1);
	
	sei();
}

int main() {
	LEF_Event event;
  uint8_t ls;
	ls = LEDRG_OFF;
	uint16_t ch;
	 
	LEF_Init();

	LEF_TimerInit(&timer1, EVENT_Timer1);
	LEF_TimerStartRepeat(&timer1, 100);
	LEF_TimerInit(&timer2, EVENT_Timer2);
	LEF_TimerStartRepeat(&timer2, 10);

	LEF_LedInit(&led, LED_FAST_BLINK);
//	LEF_LedSetState(&led, LED_FAST_BLINK);

	LEF_LedRGInit(&ledrg, LEDRG_RED_DOUBLE_BLINK);
//	LEF_LedRGSetState(&ledrg, LEDRG_RED_DOUBLE_BLINK);

	LEF_Button_init(&button, EVENT_Button);
	LEF_Rotary_init(&rotary, EVENT_Rotary);

	LEF_Buzzer_init();
	
	hw_init();

	LEF_Buzzer_set(LEF_BUZZER_BEEP);
	LEF_CliInit(cmdTable, sizeof(cmdTable) / sizeof((cmdTable)[0]) );
		
	//_delay_ms(1000);
	printf("\n\nStarting LEF simple test\n\n");

	while(1) {

		LEF_Wait(&event);

		if (evOn)
			LEF_Print_event(&event);

		switch (event.id) {
/*		 case LEF_SYSTICK_EVENT:
				ch = LEF_LedRGUpdate(&ledrg);
			if (ch & 0x01) 
				LED_RED_ON();
			else
				LED_RED_OFF();
			
			if (ch & 0x02) 
				LED_GREEN_ON();
			else
				LED_GREEN_OFF();

			break;
*/
		 case EVENT_Button:           // Handle button press event
			LEF_Print_event(&event);
			if  (event.func==1) {
				ls++;
				if (ls >= LEDRG_LAST)
					ls = LEDRG_OFF;

				LEF_LedSetState(&led, LED_SINGLE_BLINK);
				LEF_LedRGSetState(&ledrg, ls);
				
				LEF_Buzzer_set(LEF_BUZZER_SHORT_BEEP);
			}
			if (event.func==3) {
				LEF_Buzzer_set(LEF_BUZZER_BRP);
			}

			printf("Port C: %x  Clk = %d   Dt = %d\n", PINC, (PINC & (1<<PC0)), (PINC & (1<<PC1)));

			//event.id = LEF_EVENT_TEST;
		//	LEF_Send(&event);
		break;
		 case EVENT_Rotary:           // Handle rotary event
			LEF_Buzzer_set(LEF_BUZZER_BLIP);
			LEF_Print_event(&event);
			
			break;
			
	 case EVENT_Timer2:                  // Handle data from uart to Cli
			ch = uart_getc();
			while ((ch & 0xff00) != UART_NO_DATA ) {
				LEF_CliPutChar(ch);
				ch = uart_getc();
			}
			break;
		
		 case LEF_EVENT_CLI:
			LEF_CliExec();
			break;
			
		 case LEF_EVENT_TEST:
			printf("Testevent\n");
			break;
		}
		//sleep_cpu();
	}
	return 0;
}
