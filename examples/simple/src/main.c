/**
 *---------------------------------------------------------------------------
 * @brief    A simple LEF test for Arduino
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

/*
 * Hardvare connections.

 * Button      = PD7
 * Buzzer      = PD3
 * LED green   = PB3
 * LED red     = PB4
 * Rotary clk  = PC0
 * Rotary data = PC1
 * Pot         = ADC5
 * Systick     = PB2
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "main.h"
#include "uart.h"
#include "def.h"
#include "def_avr.h"

#include "LEF.h"

static const uint32_t UART_BAUD_RATE = 57600;

#define POT_ADC 5

#define PIN_BUZZER D,3
#define PIN_BUTTON D,7
#define PIN_LED_RED B,4
#define PIN_LED_GREEN B,3
#define PIN_ROT_CLK C, 0
#define PIN_ROT_DATA C,1
#define PIN_SYSTICK D,4

typedef enum {
  EVENT_Timer1 = 0,
  EVENT_Timer2,
  EVENT_Button,
  EVENT_Rotary,
  EVENT_Pot
} Events;


LEF_Timer  timer1;
LEF_Timer  timer2;
LEF_Led    led1;
LEF_LedRG  ledrg;
LEF_LedA   leda;
LEF_Button button1;
LEF_Rotary rotary;
LEF_Pot    pot;

char evOn = 0;

static FILE mystdout = FDEV_SETUP_STREAM((void*)uart_putc, NULL, _FDEV_SETUP_WRITE);
 
static void cmd_brp(char* args) {
    printf("Brp args = %s\n", args);
    LEF_Buzzer_set(LEF_BUZZER_BRP);
}

static void cmdDisk(char* args) {
    UNUSED(args);
    LEF_Buzzer_beep(100, 10, 3);
}

static void cmdTBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_TRIPPLE_BEEP);
}

static void cmdDBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_DOUBLE_BEEP);
}

static void cmd_buz_on(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_ON);
}

static void cmdBuzoff(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_OFF);
}

static void cmdBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_BEEP);
}

static void cmdSBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_SHORT_BEEP);
}

static void cmdLBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_LONG_BEEP);
}

static void cmd_blink(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_SINGLE_BLINK);
}

static void cmdLedOn(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_ON);
}

static void cmdLedOff(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_OFF);
}

static void cmdEvOn(char* args) {
    UNUSED(args);
    evOn = 1;
}

static void cmdEvOff(char* args) {
    UNUSED(args);
    evOn = 0;
}

static void cmdHelp(char* args) {
    UNUSED(args);
    LEF_Cli_print();
}

static void cmd_adc(char* args) {
    UNUSED(args);
    uint16_t val;

    ADC_ID();
    ADC_ENABLE();
    ADC_REF_AVCC();

    printf("ADC:  ");
    for (int i = 0; i < 8; i++) {
        ADC_MUX(i);
        _delay_ms(1);
        ADC_START();
        ADC_WAIT_COMPLETION();
        //_delay_ms(1);
        val = ADC_VALUE();
        printf("%d  ", val);
    }
    printf("\n");
    ADC_MUX(POT_ADC);
    ADC_IE();
}

ISR(TIMER1_COMPA_vect) {
    
    LEF_Timer_update(&timer1);
    LEF_Timer_update(&timer2);
    
    ARDUINO_LED_SET(LEF_Led_update(&led1));
    
    uint8_t ch = LEF_LedRG_update(&ledrg);
    gpio_write(PIN_LED_RED, (ch & 0x01));
    gpio_write(PIN_LED_GREEN, (ch & 0x02));
    
    TIMER0_OCA(255 - LEF_LedA_update(&leda));
    
    LEF_Button_update(&button1, !gpio_read(PIN_BUTTON));
    
    gpio_write(PIN_BUZZER, !LEF_Buzzer_update());
    
    gpio_toggle(PIN_SYSTICK);
    
    ADC_START();
}

ISR(ADC_vect) {
    uint16_t val = ADC_VALUE();
    LEF_Pot_update(&pot, val);
}

ISR(TIMER2_COMPA_vect) {
    LEF_Rotary_update(&rotary, gpio_read(PIN_ROT_CLK), gpio_read(PIN_ROT_DATA));
}

const PROGMEM LEF_CliCmd cmdTable[] = {
    LEF_CLI_LABEL("Buzzer"),
    {cmd_buz_on, "buzon", "Buzzer on"},
    {cmdBuzoff, "buzoff", "Buzzer off"},
    {cmdBeep, "beep", "Make a beep"},
    {cmdSBeep, "sbeep", "Make a short beep"},
    {cmdLBeep, "lbeep", "Make a long beep"},
    {cmdDBeep, "dbeep", "Make a double beep"},
    {cmdTBeep, "tbeep", "Make a tripple beep"},
    {cmdDisk, "disk", "Sound as dishwasher"},
    {cmd_brp, "brp", "BRP sound"},
    LEF_CLI_LABEL("Led"),
    {cmdLedOn, "ledon", "Turn led on"},
    {cmdLedOff, "ledoff", "Turn led off"},
    {cmd_blink, "blink", "Make led blink once"},
    LEF_CLI_LABEL("Misc"),
    {cmdEvOn, "evon", "Turn event on"},
    {cmdEvOff, "evoff", "Turn event off"},
    {cmd_adc, "adc", "Read adc inputs"},
    {cmdHelp, "help", "Show help"},
};

static void hw_init(void) {
    stdout = &mystdout;
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    ARDUINO_LED_INIT();

    // Timer 1 OCA at 10 ms intervall (systick)
    TIMER1_CLK_PRES_256();  // set prescaler to 1/1024
    TIMER1_OCA(625);
    TIMER1_WGM_CTC();
    TIMER1_OCA_INT(true);  // enable output compare A interrupt

    // set_sleep_mode(SLEEP_MODE_IDLE);
    // sleep_enable();

    // Activate pullup for click button
    gpio_init(PIN_BUTTON, 0, true);

    // Configure buzzer port
    gpio_init(PIN_BUZZER, 1, 1);

    // Red and Green LED
    gpio_init(PIN_LED_GREEN, 1, 0);
    gpio_init(PIN_LED_RED, 1, 0);

    gpio_init(PIN_SYSTICK, true, false);

    // 5mm Green LED
    DDRD |= (0x01 << PD6);

    // Rotary encoder input pullup activation
    gpio_init(PIN_ROT_CLK, 0, true);
    gpio_init(PIN_ROT_DATA, 0, true);

    // Pin Change Mask Register 1
    // PCMSK1 |= (1 << PCINT8);

    // // Enable Pin change interrupt
    // PCICR |= (1 << PCIE1);

    ADC_ENABLE();
    ADC_REF_AVCC();
    ADC_PRESCALER_128();
    ADC_MUX(POT_ADC);
    ADC_IE();

    TIMER0_CLK_PRES_1();
    TIMER0_OCA(250);
    //	TIMER0_MODE_PWM();
    TIMER0_WGM_FAST_PWM();
    TIMER0_OCA_COM_CLEAR();
    // TIMER0_OCA_COM_TOGGLE();

    // Timer 2 OCA at 1ms intervall
    TIMER2_CLK_PRES_64();
    TIMER2_OCA(249);
    TIMER2_WGM_CTC();
    TIMER2_OCA_INT(true);

    sei();
}

static bool main_event_handler(LEF_Event *event) {
    uint8_t ls;
    ls = LEDRG_OFF;
    uint16_t ch, val;

    if (evOn) LEF_Print_event(event);

    switch (event->id) {
        case EVENT_Button:  // Handle button press event
            LEF_Print_event(event);
            if (event->func == 1) {
                ls++;
                if (ls >= LEDRG_LAST) ls = LEDRG_OFF;

                LEF_Led_set(&led1, LED_SINGLE_BLINK);
                LEF_LedRG_set(&ledrg, ls);

                LEF_Buzzer_set(LEF_BUZZER_SHORT_BEEP);
            }
            if (event->func == 3) {
                LEF_Buzzer_set(LEF_BUZZER_BRP);
            }

            printf("Clk = %d   Dt = %d\n", gpio_read(PIN_ROT_CLK),
                   gpio_read(PIN_ROT_DATA));

            break;
        case EVENT_Rotary:  // Handle rotary event
            LEF_Buzzer_set(LEF_BUZZER_BLIP);
            LEF_Print_event(event);
            break;

        case EVENT_Timer2:  // Handle data from uart to Cli
            ch = uart_getc();
            while ((ch & 0xff00) != UART_NO_DATA) {
                LEF_Cli_putc(ch);
                ch = uart_getc();
            }
            break;

        case LEF_EVENT_CLI:
            LEF_Cli_exec(event);
            break;

        case EVENT_Pot:
            val = LEF_Pot_state(&pot);
            printf("Pot changed %d\n", val);
            TIMER0_OCA(val / 4);
            break;

        case LEF_EVENT_TEST:
            printf("Testevent\n");
            break;
    }
    return true;
}

int main() {

    LEF_init();

    LEF_Timer_init(&timer1, EVENT_Timer1);
    LEF_Timer_start_repeat(&timer1, 100);
    LEF_Timer_init(&timer2, EVENT_Timer2);
    LEF_Timer_start_repeat(&timer2, 10);

    LEF_Led_init(&led1, LED_FAST_BLINK);
    LEF_LedRG_init(&ledrg, LEDRG_RED_DOUBLE_BLINK);
    //	LEF_LedA_init(&leda, LEDA_OFF);
    //	LEF_LedA_init(&leda, LEDA_SOFT);
    LEF_LedA_init(&leda, LEDA_ON);
    LEF_LedA_init(&leda, LEDA_OFF_SOFT);

    LEF_Button_init(&button1, EVENT_Button);
    LEF_Rotary_init(&rotary, EVENT_Rotary);
    LEF_Pot_init(&pot, EVENT_Pot);

    LEF_Buzzer_init();

    hw_init();

    LEF_Buzzer_set(LEF_BUZZER_BEEP);
    LEF_CLI_INIT(cmdTable);

    printf("\n\nStarting LEF simple test\n\n");

    LEF_Run(main_event_handler, NULL);

    return 0;
}
