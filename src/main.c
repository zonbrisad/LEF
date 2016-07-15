
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "def.h"
#include "avrsimul.h"

// Variables --------------------------------------------------------------
static FILE mystdout = FDEV_SETUP_STREAM(simul_putchar, NULL, _FDEV_SETUP_WRITE);

volatile int timer2_ticks;

// Prototypes -------------------------------------------------------------
void hw_init(void);

// Code -------------------------------------------------------------------

/* Every ~ms */
ISR(TIMER2_COMPA_vect) {
	   timer2_ticks++;
}

void hw_init(void) {
	stdout = &mystdout;
	
	timer2_ticks = 0;
	/* Set up our timers and enable interrupts */
	TCNT2 = 0;    /* Timer 1 by CLK/64 */
	OCR2A = 115;   /* ~1ms */
	TIMSK2 = _BV(OCIE2A);
	TCCR2B |= (1 << CS11) | (1 << CS10) ; 
	sei();
}

int main(void) {
	volatile int tmp;
	
	hw_init();
	
	printf("Makeplate AVR example\n");
	while (1) {
		tmp = timer2_ticks;
		printf("Timer counter %d\n", tmp);
		_delay_ms(200);
	}

  return 0;
}
