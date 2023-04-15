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


void hw_init(void);

LEF_Timer  timer1;
LEF_Timer  timer2;
LEF_Led    led;
LEF_Button button;

static FILE mystdout = FDEV_SETUP_STREAM((void*)uart_putc, NULL, _FDEV_SETUP_WRITE);


ISR(TIMER1_COMPA_vect) {
	char ch;
  TIMER1_RELOAD(0);

	LEF_TimerUpdate(&timer1);
	LEF_TimerUpdate(&timer2);
	
	if (LEF_LedUpdate(&led)) {
		ARDUINO_LED_ON();
	} else {
		ARDUINO_LED_OFF();
	}

	LEF_Button_update(&button, (PIND & (1<<PIN7))==0  );

	ch = LEF_Buzzer_update();
 if (ch > 0) {
//	if (LEF_Buzzer_update() > 0) {
		BUZZER_ON();
	} else {
		BUZZER_OFF();
	}
}


//ISR(TIMER2_COMPA_vect) {
	//static int cnt;
	//static LEF_queue_element qe;
	//timer2_ticks++;
	
//	LEF_TimerUpdate(&timer1, 1);

//}


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

	sei();
}


int main() {
	LEF_queue_element dst;
  uint8_t ls;
	ls = LED_STATE_OFF;
	 
	LEF_Init();
	LEF_TimerInit(&timer1, 1);
	LEF_TimerStartRepeat(&timer1, 100);
	LEF_TimerInit(&timer2, 2);
	LEF_TimerStartRepeat(&timer2, 400);
	
	LEF_LedInit(&led);
	LEF_LedSetState(&led, LED_STATE_FAST_BLINK);
	
	LEF_Button_init(&button, 10);

	LEF_Buzzer_init();
	
	hw_init();

	LEF_Buzzer_set(LEF_BUZZER_BEEP);
	
	printf("\n\nStarting LEF simple test\n");

	while(1) {

//	    DEBUGPRINT("LEF Cli test\n");
//	    char *str = "test\n";
//	    LEF_CliPutChar('\n');

		LEF_QueueWait(&StdQueue, &dst);
		printf("Event id: %3d   func: %3d   PIND: %x\n", dst.id, dst.func, PIND);

		if ((dst.id==10) && (dst.func==1)) {
			ls++;
			if (ls >= LED_STATE_LAST)
				ls = LED_STATE_OFF;
//			LEF_LedSetState(&led, ls);
			LEF_LedSetState(&led, LED_STATE_SINGLE_BLINK);
			LEF_Buzzer_set(LEF_BUZZER_SHORT_BEEP);
		}


		//sleep_cpu();
	}
	return 0;
}
