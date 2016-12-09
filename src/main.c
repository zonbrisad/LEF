
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "def.h"
#include "avrsimul.h"

#include "LEF.h"

// Variables --------------------------------------------------------------
static FILE mystdout = FDEV_SETUP_STREAM(simul_putchar, NULL, _FDEV_SETUP_WRITE);

volatile int timer2_ticks;

LEF_Timer SecTimer;

// Prototypes -------------------------------------------------------------
void hw_init(void);

// Code -------------------------------------------------------------------

/* Every ~ms */
ISR(TIMER2_COMPA_vect) {
	static int cnt;
	static LEF_queue_element qe;
    timer2_ticks++;

    cnt++;
    qe.id=78;
    if (cnt>1000) {
    	LEF_QueueSend(&StdQueue, &qe);
    	cnt = 0;
    }

    LEF_TimerUpdate(&SecTimer,&StdQueue, 34);

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


void simulTest(void) {
	volatile int tmp;

	printf("Makeplate AVR example\n");
	while (1) {
		tmp = timer2_ticks;
		printf("Timer counter %d\n", tmp);
		_delay_ms(200);
	}
}

void LEF_QueueTest1(void) {
	LEF_queue_element src;
	LEF_queue_element dst;

	printf("LEF Queue test 1\n");

	src.id = 42;
	LEF_QueueSend(&StdQueue, &src);
	src.id = 43;
	LEF_QueueSend(&StdQueue, &src);
	src.id = 44;
	LEF_QueueSend(&StdQueue, &src);

	dst.id = 111;
	LEF_QueueWait(&StdQueue, &dst);
	printf("Event = %d\n",dst.id);
	dst.id = 111;
	LEF_QueueWait(&StdQueue, &dst);
	printf("Event = %d\n",dst.id);
	dst.id = 111;
	LEF_QueueWait(&StdQueue, &dst);
	printf("Event = %d\n",dst.id);
	dst.id = 111;
	LEF_QueueWait(&StdQueue, &dst);
	printf("Event = %d\n",dst.id);

}

void LEF_QueueTest2(void) {
	LEF_queue_element dst;
	int i;

	printf("LEF Queue test 2\n");

	i=0;
	while (1) {
		LEF_QueueWait(&StdQueue, &dst);
		printf("Event = %d\n",dst.id);
		i++;
		if (i>4)
			break;
	}

	LEF_TimerStartRepeat(&SecTimer, 500);
	while (1) {
		LEF_QueueWait(&StdQueue, &dst);
		printf("Event = %d\n",dst.id);
	}

}



int main(void) {
	
	hw_init();
	LEF_QueueInit(&StdQueue);
	LEF_TimerInit(&SecTimer);
	
	printf("LEF Test program\n");

	LEF_QueueTest1();
	LEF_QueueTest2();


	while (1) {};

  return 0;
}
