
#include <stdio.h>
//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <util/delay.h>

#include "def.h"
#include "avrsimul.h"

#include "LEF.h"

// Variables --------------------------------------------------------------
static FILE mystdout = FDEV_SETUP_STREAM(simul_putchar, NULL, _FDEV_SETUP_WRITE);

volatile int timer2_ticks;

LEF_Timer Timer1;
LEF_Timer Timer2;
LEF_Timer Timer3;

LEF_Led   Led1;

typedef enum {
	EVENT_TIMER1,
	EVENT_TIMER2,
	EVENT_TIMER3
} MyEvents;

// Prototypes -------------------------------------------------------------
void hw_init(void);

// Code -------------------------------------------------------------------

/* Every ~ms */
ISR(TIMER2_COMPA_vect) {
	//static int cnt;
	//static LEF_queue_element qe;
    timer2_ticks++;

    /*
    cnt++;
    qe.id=78;
    if (cnt>1000) {
    	LEF_QueueSend(&StdQueue, &qe);
    	cnt = 0;
    }
    */

    LEF_TimerUpdate(&Timer1, EVENT_TIMER1);
    LEF_TimerUpdate(&Timer2, EVENT_TIMER2);
    LEF_TimerUpdate(&Timer3, EVENT_TIMER3);

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


void LEF_QueueTest1(void);
void LEF_QueueTest1(void) {
	LEF_queue_element src;
	LEF_queue_element dst;

	printf("LEF Queue test 1\n");

	src.id = 42;
	LEF_QueueStdSend(&src);
	src.id = 43;
	LEF_QueueStdSend(&src);
	src.id = 44;
	LEF_QueueStdSend(&src);

	dst.id = 111;
	LEF_QueueStdWait(&dst);
	printf("Event = %d\n",dst.id);
	dst.id = 111;
	LEF_QueueStdWait(&dst);
	printf("Event = %d\n",dst.id);
	dst.id = 111;
	LEF_QueueStdWait(&dst);
	printf("Event = %d\n",dst.id);
	dst.id = 111;


}
void LEF_QueueTest2(void);
void LEF_QueueTest2(void) {
	LEF_queue_element dst;
	int i;

	printf("LEF Queue test 2\n");

	LEF_TimerStartSingle(&Timer1, 1000);
	LEF_TimerStartSingle(&Timer2, 2000);
	i = 0;
	while (1) {
		LEF_QueueStdWait(&dst);
		printf("Event = %d\n",dst.id);
		i++;
		if (i==2) {
			break;
			}
		}

	LEF_TimerStartRepeat(&Timer1, 500);
	LEF_TimerStartRepeat(&Timer2, 250);
	i=0;
	while (1) {
		LEF_QueueStdWait(&dst);
		printf("Event = %d\n",dst.id);
		i++;
		if (i>10) {
			LEF_TimerStop(&Timer1);

			break;
		}
	}

}

void LEF_LedTest(void);
void LEF_LedTest(void) {
	int i;
	uint8_t l, state;
	LEF_queue_element dst;
	DEBUGPRINT("LEF LED test\n");

	state = LED_STATE_OFF;
	i=0;
	l=0;
	LEF_TimerStartRepeat(&Timer1, 100);
	LEF_TimerStartRepeat(&Timer2, 10);
	LEF_TimerStartRepeat(&Timer3, 5000);
	while (1) {

		LEF_QueueStdWait(&dst);

		if (dst.id == EVENT_TIMER1) {
			printf("LED state %3d\n",l);
			printf(E_CUR_RETURN);
		}
		if (dst.id == EVENT_TIMER2) {
			l = LEF_LedUpdate(&Led1);
			i++;
		}

		if (dst.id == EVENT_TIMER3) {
			state++;
			if (state>=LED_STATE_LAST)
				break;
			LEF_LedSetState(&Led1, state);
			i++;
		}
	}
}

void xx(void) {
	printf("XX\n");
}

const PROGMEM LEF_CliCmd cmdTable[] = {
  {xx,       "test",      "Test av kommando"},
  {xx,       "test",      "Test av kommando"},
  {xx,       "test",      "Test av kommando"},
/*  {printInfo,         "info",         "Print basic info"},
  {printTemperature,  "temperature",  "Read temperature cont."},
  {Moist,             "moist",        "Read moist cont."},
  {TimerPotTest,      "timer",        "Test timer pot"},
  */
  {NULL,              "",            ""}
};

void LEF_CliTest(void);
void LEF_CliTest(void) {
	int i;
	LEF_queue_element dst;
	DEBUGPRINT("LEF Cli test\n");
	char *str = "test\n";

	LEF_CliPutChar('\n');

	i=0;
	LEF_TimerStartRepeat(&Timer1, 300);
	while (1) {

		LEF_QueueWait(&StdQueue, &dst);

		if (dst.id == EVENT_TIMER1) {
			if (str[i]!= '\0') {
				LEF_CliPutChar(str[i]);
				i++;
			}

		}
		if (dst.id == 250) {
			LEF_CliExec();
		}
	}
}


//#define WEAK __attribute__((weak, alias("weakFunction")))

void weakTest(void) {
  printf("My strong function\n");
}

int main(void) {
	
	hw_init();

	printf("LEF Test program\n");
	printf("\x1B\x63"); // vt100 init
	printf("\x1B[2J");  // clear screen


	LEF_Init();
//	LEF_TimerInit(&Timer1);
//	LEF_TimerInit(&Timer2);
//	LEF_TimerInit(&Timer3);
//	LEF_LedInit(&Led1);
	
	LEF_CliInit(cmdTable);
	LEF_CliPrintCommands(cmdTable);


//	weakTest();

	LEF_QueueTest1();
	LEF_QueueTest2();
	//LEF_LedTest();
	LEF_CliTest();

	while (1) {};

  return 0;
}
