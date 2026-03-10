/**
 *---------------------------------------------------------------------------
 * @brief    Testprogram for Arduino Mega component testboard
 *
 * @file     main.c
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2026-03-02
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */

/*
 * Hardware description:
 * - Arduino Mega 1280
 *
 * PG5 = Button1
 * PE5 = Button2
 * PE4 = Button3
 *
 * PK0 = Potentiometer ADC input (ADC8)
 * PK1 = Buzzer
 *
 * PK7 = LED1
 * PK6 = LED2
 * PK5 = LED3
 * PK4 = LED4
 * PK3 = LED Red
 * PK2 = LED Green
 *
 * Pxx = Rotary
 * Pxx = Rotary
 *
 * LCD 
 * - RS = PD7
 * - RW = PD6
 * - E  = PD5
 * - D4 = PD4
 * - D5 = PD3
 * - D6 = PD2
 * - D7 = PD1
 */

// Include ------------------------------------------------------------------

#include "main.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "LEF.h"
#include "def.h"
#include "def_avr.h"
#include "uart.h"
#include "lcd.h"

// Macros -------------------------------------------------------------------

static const uint32_t UART_BAUD_RATE = 57600;

#define BUZZER_PIN PK1
#define POT_ADC 8

inline void BUZZER_INIT(void) { DDRK |= (1 << BUZZER_PIN); }
inline void BUZZER_OFF(void)  { PORTK &= ~(1 << BUZZER_PIN); }
inline void BUZZER_ON(void)   { PORTK |= (1 << BUZZER_PIN); }

inline void LED1_INIT(void) { DDRK |= (0x01 << PK7); }
inline void LED2_INIT(void) { DDRK |= (0x01 << PK6); }
inline void LED3_INIT(void) { DDRK |= (0x01 << PK5); }
inline void LED4_INIT(void) { DDRK |= (0x01 << PK4); }
inline void LED_RG_INIT(void) { DDRK |= (0x01 << PK3) | (0x01 << PK2); }

inline void LED1_ON(void) { PORTK |= (0x01 << PK7); }
inline void LED2_ON(void) { PORTK |= (0x01 << PK6); }
inline void LED3_ON(void) { PORTK |= (0x01 << PK5); }
inline void LED4_ON(void) { PORTK |= (0x01 << PK4); }
inline void LED_GREEN_ON(void) { PORTK |= (0x01 << PK3); }
inline void LED_RED_ON(void) { PORTK |= (0x01 << PK2); }

inline void LED1_OFF(void) { PORTK &= ~(0x01 << PK7); }
inline void LED2_OFF(void) { PORTK &= ~(0x01 << PK6); }
inline void LED3_OFF(void) { PORTK &= ~(0x01 << PK5); }
inline void LED4_OFF(void) { PORTK &= ~(0x01 << PK4); }
inline void LED_GREEN_OFF(void) { PORTK &= ~(0x01 << PK3); }
inline void LED_RED_OFF(void) { PORTK &= ~(0x01 << PK2); }

inline void BUTTON1_INIT(void) {
    DDRG &= ~(1 << PG5);
    PORTG |= (1 << PG5);
}
inline void BUTTON2_INIT(void) {
    DDRE &= ~(1 << PE5);
    PORTE |= (1 << PE5);
}
inline void BUTTON3_INIT(void) {
    DDRE &= ~(1 << PE4);
    PORTE |= (1 << PE4);
}
inline bool BUTTON1_PRESSED(void) { return (PING & (1 << PG5)) ? false : true; }
inline bool BUTTON2_PRESSED(void) { return (PINE & (1 << PE5)) ? false : true; }
inline bool BUTTON3_PRESSED(void) { return (PINE & (1 << PE4)) ? false : true; }

inline ROT_INIT(void) {
    // Configure rotary encoder pins as inputs with pull-ups
    DDRC &= ~((1 << PC0) | (1 << PC1)); // Clear bits to set as input
    PORTC |= (1 << PC0) | (1 << PC1);   // Enable pull-up resistors
}
inline uint8_t ROT_CLK(void)  { return PINC & (1 << PC0); }
inline uint8_t ROT_DATA(void) { return PINC & (1 << PC1); }

typedef enum {
    EVENT_Timer1 = 0,
    EVENT_Timer2,
    EVENT_TimerA,
    EVENT_Button1,
    EVENT_Button2,
    EVENT_Button3,
    EVENT_Rotary,
    EVENT_Pot
} Events;

void hw_init(void);

void cmd_brp(char* args);
void cmdDBeep(char* args);
void cmdTBeep(char* args);
void cmdBuzon(char* args);
void cmdBuzoff(char* args);
void cmdBeep(char* args);
void cmdSBeep(char* args);
void cmdLBeep(char* args);
void cmd_blink(char* args);
void cmd_dblink(char* args);
void cmd_tblink(char* args);
void cmdLedOn(char* args);
void cmdLedOff(char* args);
void cmdEvOn(char* args);
void cmdEvOff(char* args);
void cmdHelp(char* args);
void cmd_disk(char* args);
void cmd_adc(char* args);
void cmd_adc_mon(char* args);
void cmd_reset(char* args);
void cmd_lcd_on(char* args);
void cmd_lcd_off(char* args);
void cmd_lcd_cursor_on(char* args);
void cmd_lcd_clear(char* args);
void cmd_lcd_test(char* args);

LEF_Timer timer1;
LEF_Timer timer2;
LEF_Timer timer_a;
LEF_Led led1;
LEF_Led led2;
LEF_Led led3;
LEF_Led led4;
LEF_LedRG ledrg;
LEF_LedA leda;
LEF_Button button1;
LEF_Button button2;
LEF_Button button3;
LEF_Rotary rotary;
LEF_Pot pot;

char evOn = 0;

static FILE mystdout =
    FDEV_SETUP_STREAM((void*)uart_putc, NULL, _FDEV_SETUP_WRITE);

const PROGMEM LEF_CliCmd cmdTable[] = {
    LEF_CLI_LABEL("Buzzer"),
    {cmdBuzon, "buzon", "Buzzer on"},
    {cmdBuzoff, "buzoff", "Buzzer off"},
    {cmdBeep, "beep", "Make a beep"},
    {cmdSBeep, "sbeep", "Make a short beep"},
    {cmdLBeep, "lbeep", "Make a long beep"},
    {cmdDBeep, "dbeep", "Make a double beep"},
    {cmdTBeep, "tbeep", "Make a tripple beep"},
    {cmd_disk, "disk", "Sound as dishwasher"},
    {cmd_brp, "brp", "BRP sound"},
    LEF_CLI_LABEL("Led"),
    {cmdLedOn, "ledon", "Turn led on"},
    {cmdLedOff, "ledoff", "Turn led off"},
    {cmd_blink, "blink", "Make led blink once"},
    {cmd_dblink, "dblink", "Make led blink twice"},
    {cmd_tblink, "tblink", "Make led blink three times"},
    LEF_CLI_LABEL("LCD"),
    {cmd_lcd_on, "lcdon", "Turn LCD on"},
    {cmd_lcd_off, "lcdoff", "Turn LCD off"},
    {cmd_lcd_cursor_on, "lcdcuron", "Turn LCD cursor on"},
    {cmd_lcd_clear, "lcdclr", "Clear LCD"},
    {cmd_lcd_test, "lcdt1", "Run LCD test"},
    LEF_CLI_LABEL("Misc"),
    {cmdEvOn, "evon", "Turn event on"},
    {cmdEvOff, "evoff", "Turn event off"},
    {cmd_adc, "adc", "Read adc inputs"},
    {cmd_adc_mon, "adcmon", "Monitor adc inputs"},
    {cmd_reset, "reset", "Reset the system"},
    // {cmdHelp,     "help",     "Show help"},
    LEF_CLI_CMD(cmdHelp, "help", "Show help")};

void cmd_brp(char* args) {
    printf("Brp args = %s\n", args);
    LEF_Buzzer_set(LEF_BUZZER_BRP);
}

void cmd_disk(char* args) {
    UNUSED(args);
    LEF_Buzzer_beep(100, 10, 3);
}

void cmdTBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_TRIPPLE_BEEP);
}

void cmdDBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_DOUBLE_BEEP);
}

void cmdBuzon(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_ON);
}

void cmdBuzoff(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_OFF);
}

void cmdBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_BEEP);
}

void cmdSBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_SHORT_BEEP);
}

void cmdLBeep(char* args) {
    UNUSED(args);
    LEF_Buzzer_set(LEF_BUZZER_LONG_BEEP);
}

void cmd_blink(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_SINGLE_BLINK);
}

void cmd_dblink(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_DOUBLE_BLINK);
}

void cmd_tblink(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_TRIPPLE_BLINK);
}

void cmdLedOn(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_ON);
}

void cmdLedOff(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_OFF);
}

void cmdEvOn(char* args) {
    UNUSED(args);
    evOn = 1;
}

void cmdEvOff(char* args) {
    UNUSED(args);
    evOn = 0;
}

void cmdHelp(char* args) {
    UNUSED(args);
    LEF_Cli_print();
}

void cmd_lcd_on(char* args) {
    UNUSED(args);
    lcd_on();
}

void cmd_lcd_off(char* args) {
    UNUSED(args);
    lcd_off();
}

void cmd_lcd_cursor_on(char* args) {
    UNUSED(args);
    lcd_on_cursor();
}

void cmd_lcd_clear(char* args) {
    UNUSED(args);
    lcd_clear();
}

void cmd_lcd_test(char* args) {
    UNUSED(args);
    lcd_clear();
    lcd_puts("Line 1\n");
    lcd_puts("Line 2\n");
    lcd_puts("Line 3\n");
    lcd_puts("Line 4\n");
}

void adc_print_all(bool print_header) {
    uint16_t val;
    if (print_header) {
        printf("\e[4m"); // Underline
        for (int j = 0; j < 15; j++) {
            printf("%4d ", j);
        }
        printf("\e[0m\n");
        return;
    }
    for (int i = 0; i < 15; i++) {
        ADC_MUX(i);
        _delay_ms(1);
        ADC_START();
        ADC_WAIT_COMPLETION();
        val = ADC_VALUE();
        printf("%4d ", val);
    }
    printf("\n");
}

void cmd_adc(char* args) {
    UNUSED(args);
    uint16_t val;

    ADC_ID();
    ADC_ENABLE();
    ADC_REF_AVCC();
    adc_print_all(true);
    adc_print_all(false);  
    ADC_MUX(POT_ADC);
    ADC_IE();
}

void wait_event(LEF_Event *event) {
    uint16_t ch; 
    LEF_Wait(event);
    switch (event->id) {
        case EVENT_Timer2:  // Handle data from uart to Cli
            ch = uart_getc();
            while ((ch & 0xff00) != UART_NO_DATA) {
                LEF_Cli_putc(ch);
                ch = uart_getc();
            }
        break;
    }
    return;
}   


void cmd_adc_mon(char* args) {
    LEF_Event event;
    UNUSED(args);

    ADC_ID();
    ADC_ENABLE();
    ADC_REF_AVCC();

    printf("Monitoring ADC (press any button to stop)\n");
    adc_print_all(true);
    LEF_Timer_start_repeat(&timer_a, 75);

    LEF_Cli_WaitKeyPressed();
    while (1) {
        wait_event(&event);
        if (event.id == LEF_EVENT_CLI) {
            LEF_Cli_exec(&event);
            printf("Stopping ADC monitor...\n");
            return;
            break;
        }
        if (event.id == EVENT_TimerA) {
            adc_print_all(false);
        }
    }
    ADC_MUX(POT_ADC);
    ADC_IE();
}

void cmd_reset(char* args) {
    UNUSED(args);
    RESET();
}

ISR(PCINT1_vect) {
    char ch = PINC;
    LEF_Rotary_update(&rotary, (ch & (1 << PC0)), (ch & (1 << PC1)));
}

ISR(TIMER1_COMPA_vect) {
    char ch;

    TIMER1_RELOAD(0);

    // ARDUINO_LED_TOGGLE();
    //	event.id = LEF_SYSTICK_EVENT;
    //	LEF_QueueWait(&StdQueue, &event);

    LEF_Timer_update(&timer1);
    LEF_Timer_update(&timer2);
    LEF_Timer_update(&timer_a);

    LEF_Led_update(&led1) ? LED1_ON() : LED1_OFF();
    LEF_Led_update(&led2) ? LED2_ON() : LED2_OFF();
    LEF_Led_update(&led3) ? LED3_ON() : LED3_OFF();
    LEF_Led_update(&led4) ? LED4_ON() : LED4_OFF();

    ch = LEF_LedRG_update(&ledrg);
    (ch & 0x01) ? LED_RED_ON() : LED_RED_OFF();
    (ch & 0x02) ? LED_GREEN_ON() : LED_GREEN_OFF();

    TIMER0_OCA_SET(255 - LEF_LedA_update(&leda));

    LEF_Button_update(&button1, BUTTON1_PRESSED());
    LEF_Button_update(&button2, BUTTON2_PRESSED());
    LEF_Button_update(&button3, BUTTON3_PRESSED());

    //	ch = PINC;
    //	LEF_Rotary_update(&rotary, (ch & (1<<PC0)), (ch & (1<<PC1)));

    LEF_Buzzer_update() ? BUZZER_ON() : BUZZER_OFF();
    ADC_MUX(POT_ADC);
    ADC_START();
}

ISR(ADC_vect) {
    // uart_putc('a');
    uint16_t val = ADC_VALUE();
    LEF_Pot_update(&pot, val);
}

void hw_init(void) {
    stdout = &mystdout;
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    ARDUINO_LED_INIT();

    LED1_INIT();
    LED2_INIT();
    LED3_INIT();
    LED4_INIT();
    LED_RG_INIT();

    BUTTON1_INIT();
    BUTTON2_INIT();
    BUTTON3_INIT();

    BUZZER_INIT();

    // set_sleep_mode(SLEEP_MODE_IDLE);
    // sleep_enable();

    // Pin Change Mask Register 1
    //PCMSK1 |= (1 << PCINT8);

    // Enable Pin change interrupt
    //PCICR |= (1 << PCIE1);

    ADC_ENABLE();
    ADC_REF_AVCC();
    
    ADC_PRESCALER_128();
    ADC_MUX(POT_ADC);
    ADC_IE();

    // Timer 1 (16 bit) (used for LEF timing)
    TIMER1_CLK_PRES_256();  // set prescaler to 1/1024
    TIMER1_OCA_SET(625);
    TIMER1_OCA_IE();  // enable output compare A interrupt

    //   TIMER0_CLK_PRES_1();
    //   TIMER0_OCA_SET(250);
    // //	TIMER0_WGM_PWM();
    //   TIMER0_WGM_FAST_PWM();
    //   TIMER0_OM_CLEAR();

    lcd_init(LCD_DISP_ON);
    lcd_clear();
    lcd_puts("   LEF Mega Test\n");

    sei();
}

static inline char* int2bits8(uint8_t x) {
    static char str[9];
    for (int i = 0; i < 8; i++) {
        str[7 - i] = (x & (1 << i)) ? '1' : '0';
    }
    str[8] = '\0';
    return str;
}

static inline char* int2bits16(uint16_t x) {
    static char str[17];
    for (int i = 0; i < 16; i++) {
        str[15 - i] = (x & (1 << i)) ? '1' : '0';
    }
    str[16] = '\0';
    return str;
}

int main(void) {
    LEF_Event event;
    uint8_t ls;
    ls = LEDRG_OFF;
    uint16_t ch, val;

    LEF_init();

    LEF_Timer_init(&timer1, EVENT_Timer1);
    LEF_Timer_start_repeat(&timer1, 100);
    LEF_Timer_init(&timer2, EVENT_Timer2);
    LEF_Timer_start_repeat(&timer2, 10);
    LEF_Timer_init(&timer_a, EVENT_TimerA);
    //LEF_Timer_start_repeat(&timer_a, 1000);

    LEF_Led_init(&led1, LED_SLOW_BLINK);
    LEF_Led_init(&led2, LED_BLINK);
    LEF_Led_init(&led3, LED_FAST_BLINK);
    LEF_Led_init(&led4, LED_BLIP);

    LEF_LedRG_init(&ledrg, LEDRG_GREEN_DOUBLE_BLINK);

    //	LEF_LedA_init(&leda, LEDA_OFF);
    //	LEF_LedA_init(&leda, LEDA_SOFT);
    LEF_LedA_init(&leda, LEDA_ON);
    LEF_LedA_init(&leda, LEDA_OFF_SOFT);

    LEF_Button_init(&button1, EVENT_Button1);
    LEF_Button_init(&button2, EVENT_Button2);
    LEF_Button_init(&button3, EVENT_Button3);

    LEF_Rotary_init(&rotary, EVENT_Rotary);
    LEF_Pot_init(&pot, EVENT_Pot);

    LEF_Buzzer_init();

    hw_init();

    printf("\n\nStarting LEF Arduino mega test\n\n");
    LEF_Buzzer_set(LEF_BUZZER_BEEP);
    LEF_Cli_init(cmdTable, sizeof(cmdTable) / sizeof((cmdTable)[0]));

    while (1) {
        LEF_Wait(&event);

        if (evOn) LEF_Print_event(&event);

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
            case EVENT_Button1:  // Handle button press event
                printf("Button 1 event: func = %d\n", event.func);
                LEF_Led_set(&led1, LED_SLOW_BLINK);
                LEF_Led_set(&led2, LED_BLINK);
                LEF_Led_set(&led3, LED_FAST_BLINK);
                LEF_Led_set(&led4, LED_BLIP);
                break;
            case EVENT_Button2:  // Handle button press event
                printf("Button 2 event: func = %d\n", event.func);
                break;
            case EVENT_Button3:  // Handle button press event
                // LEF_Print_event(&event);
                if (event.func == 1) {
                    ls++;
                    if (ls >= LEDRG_LAST) ls = LEDRG_OFF;

                    LEF_Led_set(&led1, LED_SINGLE_BLINK);
                    LEF_LedRG_set(&ledrg, ls);

                    LEF_Buzzer_set(LEF_BUZZER_SHORT_BEEP);
                }
                if (event.func == 3) {
                    LEF_Buzzer_set(LEF_BUZZER_BRP);
                }

                // printf("Port C: %x  Clk = %d   Dt = %d\n", PINC, (PINC &
                // (1<<PC0)), (PINC & (1<<PC1)));
                printf("Port C: %s  Clk = %d   Dt = %d\n", int2bits8(PINE),
                       (PINC & (1 << PC0)), (PINC & (1 << PC1)));

                // event.id = LEF_EVENT_TEST;
                //	LEF_Send(&event);
                break;
            case EVENT_Rotary:  // Handle rotary event
                LEF_Buzzer_set(LEF_BUZZER_BLIP);
                LEF_Print_event(&event);
                break;

            case EVENT_Timer2:  // Handle data from uart to Cli
                ch = uart_getc();
                while ((ch & 0xff00) != UART_NO_DATA) {
                    LEF_Cli_putc(ch);
                    ch = uart_getc();
                }
                break;

            case LEF_EVENT_CLI:
                LEF_Cli_exec(&event);
                break;

            case EVENT_Pot:
                val = LEF_Pot_state(&pot);
                // printf("Pot changed %d\n", val);
                TIMER0_OCA_SET(val / 4);

                if (val > 100)
                    LEF_Led_set(&led1, LED_ON);
                else
                    LEF_Led_set(&led1, LED_OFF);

                if (val > 250)
                    LEF_Led_set(&led2, LED_ON);
                else
                    LEF_Led_set(&led2, LED_OFF);

                if (val > 500)
                    LEF_Led_set(&led3, LED_ON);
                else
                    LEF_Led_set(&led3, LED_OFF);

                if (val > 750)
                    LEF_Led_set(&led4, LED_ON);
                else
                    LEF_Led_set(&led4, LED_OFF);

                break;

            case LEF_EVENT_TEST:
                printf("Testevent\n");
                break;
        }

        // sleep_cpu();
    }

    return 0;
}
