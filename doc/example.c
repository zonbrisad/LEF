
#include "LEF.h"

typedef enum {            // declare events
	EVENT_Timer1 = 0,
	EVENT_Button,
	EVENT_Rotary
} Events;

LEF_Timer  timer;    // declare resources
LEF_Led    led;
LEF_Button button;
LEF_Rotary rotary;

const PROGMEM LEF_CliCmd cmdTable[] = {
	{myCmdHandler ,  "mycmd",    "My command"},
};


ISR(TIMER1_COMPA_vect) {
	char ch;

  TIMER1_RELOAD(0);
	
	LEF_TimerUpdate(&timer1);
	
	if (LEF_LedUpdate(&led)) {
		ARDUINO_LED_ON();
	} else {
		ARDUINO_LED_OFF();
	}
	
	LEF_Button_update(&button, (PIND & (1<<PIN7))==0  );

	if (LEF_Buzzer_update() > 0) {
		BUZZER_ON();
	} else {
		BUZZER_OFF();
	}
}

int main() {
	LEF_Event event;
	 
	LEF_Init();

	LEF_TimerInit(&timer1, EVENT_Timer1);
	LEF_TimerStartRepeat(&timer1, 100);
	L7EF_LedInit(&led);
	LEF_LedSetState(&led, LED_STATE_FAST_BLINK);

	LEF_LedRGInit(&ledrg);
	LEF_LedRGSetState(&ledrg, LEDRG_RED_DOUBLE_BLINK);

	LEF_Button_init(&button, EVENT_Button);
	LEF_Rotary_init(&rotary, EVENT_Rotary);

	LEF_Buzzer_init();
	
	hw_init();

	LEF_Buzzer_set(LEF_BUZZER_BEEP);
	LEF_CliInit(cmdTable, sizeof(cmdTable) / sizeof((cmdTable)[0]) );
		
	while(1) {  // Main event loop

		LEF_Wait(&event);

		switch (event.id) {

		 case EVENT_Button:           // Handle button press event
			LEF_Print_event(&event);
			if  (event.func==1) {
				ls++;
				if (ls >= LEDRG_LAST)
					ls = LEDRG_OFF;

				LEF_LedSetState(&led, LED_STATE_SINGLE_BLINK);
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
