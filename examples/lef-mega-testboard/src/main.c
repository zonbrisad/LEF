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
 * Button1 = PG5 (PWM4)
 * Button2 = PE5 (PWM3)
 * Button3 = PE4 (PWM2)
 *
 * Pot = PK0 (ADC8)
 * Buzzer = PK1 (ADC9)
 *
 * LED1 = PK7 (ADC15)
 * LED2 = PK6 (ADC14)
 * LED3 = PK5 (ADC13)
 * LED4 = PK4 (ADC12)
 * LED Red = PK3 (ADC11)
 * LED Green = PK2 (ADC10)
 *
 * OC1A (sys timer tick) = PB5
 *
 * Rotary Clk  = NC
 * Rotary Data = NC
 *
 * LCD
 * - RS = PF7 (ADC7)
 * - RW = PF6 (ADC6)
 * - E  = PF5 (ADC5)
 * - D4 = PF4 (ADC4)
 * - D5 = PF3 (ADC3)
 * - D6 = PF2 (ADC2)
 * - D7 = PF1 (ADC1)
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

#define BUZZER_PIN K,1
#define LED1_PIN K,7
#define LED2_PIN K,6
#define LED3_PIN K,5
#define LED4_PIN K,4
#define LED_RED_PIN K,3
#define LED_GREEN_PIN K,2
#define BUTTON1_PIN G,5
#define BUTTON2_PIN E,5
#define BUTTON3_PIN E,4

#define POT_ADC 8

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

bool evOn = false;

static FILE mystdout =
    FDEV_SETUP_STREAM((void*)uart_putc, NULL, _FDEV_SETUP_WRITE);

static void cmd_brp(char* args) {
    printf_P(PSTR("Brp args = %s\n"), args);
    LEF_Buzzer_set(LEF_BUZZER_BRP);
}

static void cmd_disk(char* args) {
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

static void cmd_dblink(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_DOUBLE_BLINK);
}

static void cmd_tblink(char* args) {
    UNUSED(args);
    LEF_Led_set(&led1, LED_TRIPPLE_BLINK);
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
    evOn = true;
}

static void cmdEvOff(char* args) {
    UNUSED(args);
    evOn = false;
}

static void cmdHelp(char* args) {
    UNUSED(args);
    LEF_Cli_print();
}

static void cmd_lcd_on(char* args) {
    UNUSED(args);
    lcd_on();
}

static void cmd_lcd_off(char* args) {
    UNUSED(args);
    lcd_off();
}

static void cmd_lcd_cursor_on(char* args) {
    UNUSED(args);
    lcd_on_cursor();
}

static void cmd_lcd_clear(char* args) {
    UNUSED(args);
    lcd_clear();
}

static void cmd_lcd_home(char* args) {
    UNUSED(args);
    lcd_home();
}

static void cmd_lcd_move_right(char* args) {
    UNUSED(args);
    lcd_move_right();
}

static void cmd_lcd_move_left(char* args) {
    UNUSED(args);
    lcd_move_left();
}

static void cmd_lcd_test(char* args) {
    UNUSED(args);
    lcd_clear();
    lcd_puts_P("Line 1\n");
    lcd_puts_P("Line 2\n");
    lcd_puts_P("Line 3\n");
    lcd_puts_P("Line 4\n");
}

static void cmd_lcd_test_move(char* args) {
    UNUSED(args);
    lcd_clear();
    lcd_puts_P("Some text to move");
    _delay_ms(300);
    lcd_home();
    lcd_command(LCD_MOVE_DISP_RIGHT);
}

static inline void adc_print_all(bool print_header) {
    if (print_header) {
        printf_P(PSTR("\e[4m")); // Underline
        for (int i = 0; i < 16; i++) {
            printf("%4d ", i);
        }
        printf_P(PSTR("\e[0m\n"));
        return;
    }

    for (uint8_t i = 0; i < 16; i++) {
        ADC_MUX(i);
        _delay_us(5);
        ADC_START();
        ADC_WAIT_COMPLETION();
        printf("%4d ", ADC_VALUE());
    }
    printf("\n");
}

inline void wait_event(LEF_Event *event) {
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

static void cmd_adc(char* args) {
    UNUSED(args);

    LEF_Pot_enable(&pot, false);

    adc_print_all(true);
    adc_print_all(false);

    ADC_MUX(POT_ADC);
    LEF_Pot_enable(&pot, true);
}

static void cmd_adc_mon(char* args) {
    UNUSED(args);
    LEF_Event event;
    
    LEF_Pot_enable(&pot, false);

    printf_P(PSTR("Monitoring ADC (press any button to stop)\n"));
    adc_print_all(true);
    LEF_Timer_start_repeat(&timer_a, 75);
    
    LEF_Cli_WaitKeyPressed();
    while (true) {
        wait_event(&event);
        if (event.id == LEF_EVENT_CLI) {
            LEF_Cli_exec(&event);
            printf_P(PSTR("Stopping ADC monitor...\n"));
            break;
        }
        if (event.id == EVENT_TimerA) {
            adc_print_all(false);
        }
    }
    ADC_MUX(POT_ADC);
    LEF_Pot_enable(&pot, true);
}

static void cmd_reset(char* args) {
    UNUSED(args);
    RESET();
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
    {cmd_lcd_home, "lcdh", "Move cursor to home pos"},
    {cmd_lcd_move_right, "lcdr", "Move text right"},
    {cmd_lcd_move_left, "lcdl", "Move text left"},
    {cmd_lcd_test, "lcdt1", "Run LCD test"},
    {cmd_lcd_test_move, "lcdtm", "Run LCD move test"},

    LEF_CLI_LABEL("Misc"),
    {cmdEvOn, "evon", "Turn event on"},
    {cmdEvOff, "evoff", "Turn event off"},
    {cmd_adc, "adc", "Read adc inputs"},
    {cmd_adc_mon, "adcmon", "Monitor adc inputs"},
    {cmd_reset, "reset", "Reset the system"},
    LEF_CLI_CMD(cmdHelp, "help", "Show help")
};

ISR(PCINT1_vect) {
    char ch = PINC;
    LEF_Rotary_update(&rotary, (ch & (1 << PC0)), (ch & (1 << PC1)));
}

ISR(TIMER1_COMPA_vect) {
    char ch;

    LEF_Timer_update(&timer1);
    LEF_Timer_update(&timer2);
    LEF_Timer_update(&timer_a);

    gpio_write(LED1_PIN, LEF_Led_update(&led1));
    gpio_write(LED2_PIN, LEF_Led_update(&led2));
    gpio_write(LED3_PIN, LEF_Led_update(&led3));
    gpio_write(LED4_PIN, LEF_Led_update(&led4));
    
    ch = LEF_LedRG_update(&ledrg);
    gpio_write(LED_RED_PIN, (ch & 0x01));
    gpio_write(LED_GREEN_PIN, (ch & 0x02));

    TIMER0_OCA(255 - LEF_LedA_update(&leda));

    LEF_Button_update(&button1, !gpio_read(BUTTON1_PIN));
    LEF_Button_update(&button2, !gpio_read(BUTTON2_PIN));
    LEF_Button_update(&button3, !gpio_read(BUTTON3_PIN));

    //	ch = PINC;
    //	LEF_Rotary_update(&rotary, (ch & (1<<PC0)), (ch & (1<<PC1)));

    gpio_write(BUZZER_PIN, LEF_Buzzer_update());

    if (LEF_Pot_is_enabled(&pot))
        ADC_START();
}

ISR(ADC_vect) {
    LEF_Pot_update(&pot, ADC_VALUE());
}


static uint16_t LCD_Handler(HD44780_LCD msg, uint16_t data_arg) {
    uint16_t result = 0;
    switch (msg) {
        case HD44780_MSG_INIT:
            gpio_init(LCD_E_PIN, true, false);
            gpio_init(LCD_RW_PIN, true, false);
            gpio_init(LCD_RS_PIN, true, false);
            break;
        case HD44780_MSG_GPIO_DATA_DIRECTION:
            gpio_direction(LCD_DATA4_PIN, data_arg);
            gpio_direction(LCD_DATA5_PIN, data_arg);
            gpio_direction(LCD_DATA6_PIN, data_arg);
            gpio_direction(LCD_DATA7_PIN, data_arg);
            break;
        case HD44780_MSG_GPIO_DATA_READ:
            if (gpio_read(LCD_DATA4_PIN)) result |= 0x01;
            if (gpio_read(LCD_DATA5_PIN)) result |= 0x02;
            if (gpio_read(LCD_DATA6_PIN)) result |= 0x04;
            if (gpio_read(LCD_DATA7_PIN)) result |= 0x08;
            break;    
        case HD44780_MSG_GPIO_DATA_WRITE:
            gpio_write(LCD_DATA7_PIN, data_arg & 0x80);
            gpio_write(LCD_DATA6_PIN, data_arg & 0x40);
            gpio_write(LCD_DATA5_PIN, data_arg & 0x20);
            gpio_write(LCD_DATA4_PIN, data_arg & 0x10);
            break;    
        case HD44780_MSG_GPIO_E:
            gpio_write(LCD_E_PIN, data_arg);
            break;
        case HD44780_MSG_GPIO_E_TOGGLE:
            gpio_write(LCD_E_PIN, 1);
            _delay_us(LCD_DELAY_ENABLE_PULSE);
            gpio_write(LCD_E_PIN, 0);
            break;
        case HD44780_MSG_GPIO_RW:
            gpio_write(LCD_RW_PIN, data_arg);
            break;
        case HD44780_MSG_GPIO_RS:
            gpio_write(LCD_RS_PIN, data_arg);
            break;
        case HD44780_MSG_DELAY_E:
            _delay_us(LCD_DELAY_ENABLE_PULSE);
            break;
        default: break;
    }
    return result;
}

static void hw_init(void) {
    stdout = &mystdout;
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    ARDUINO_LED_INIT();

    gpio_init(LED1_PIN, 1, 0);
    gpio_init(LED2_PIN, 1, 0);
    gpio_init(LED3_PIN, 1, 0);
    gpio_init(LED4_PIN, 1, 0);
    gpio_init(LED_RED_PIN, 1, 0);
    gpio_init(LED_GREEN_PIN, 1, 0);

    gpio_init(BUTTON1_PIN, false , true);
    gpio_init(BUTTON2_PIN, false , true);
    gpio_init(BUTTON3_PIN, false , true);

    gpio_init(BUZZER_PIN, 1, 0);

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

    // Timer 1 (16 bit) 10ms intervall on OCA1 interrupt 
    // (used for LEF system timer)
    TIMER1_CLK_PRES_64(); // alternative for 10ms that gives good accuracy
    TIMER1_OCA(2499);
    TIMER1_MODE_CTC();        // Clear timer on compare (on OC1A)
    TIMER1_COM_OC1A_TOGGLE(); // toggle OC1A pin when compare
    gpio_init(PIN_OC1A, true, false);
    TIMER1_OCA_INT(true);  // enable output compare A interrupt

    //   TIMER0_CLK_PRES_1();
    //   TIMER0_OCA_SET(250);
    // //	TIMER0_WGM_PWM();
    //   TIMER0_WGM_FAST_PWM();
    //   TIMER0_OM_CLEAR();

    // lcd_init(LCD_DISP_ON);
    lcd_init(LCD_Handler, LCD_DISP_ON);
    lcd_clear();
    lcd_puts_P("   LEF Mega Test\n");

    sei();
}

int main(void) {
    LEF_Event event;
    uint16_t ch, val;
    uint8_t ls = LEDRG_OFF;

    LEF_init();

    LEF_Timer_init(&timer1, EVENT_Timer1);
    LEF_Timer_start_repeat(&timer1, 100);
    LEF_Timer_init(&timer2, EVENT_Timer2);
    LEF_Timer_start_repeat(&timer2, 10);
    LEF_Timer_init(&timer_a, EVENT_TimerA);

    LEF_Led_init(&led1, LED_SLOW_BLINK);
    LEF_Led_init(&led2, LED_BLINK);
    LEF_Led_init(&led3, LED_FAST_BLINK);
    LEF_Led_init(&led4, LED_BLIP);
    LEF_LedRG_init(&ledrg, LEDRG_GREEN_DOUBLE_BLINK);
    LEF_Button_init(&button1, EVENT_Button1);
    LEF_Button_init(&button2, EVENT_Button2);
    LEF_Button_init(&button3, EVENT_Button3);
    LEF_Rotary_init(&rotary, EVENT_Rotary);
    LEF_Pot_init(&pot, EVENT_Pot);
    LEF_Buzzer_init();
    LEF_CLI_INIT(cmdTable);
    
    hw_init();

    printf_P(PSTR("\n\nStarting LEF Arduino mega test\n\n"));

    LEF_Buzzer_set(LEF_BUZZER_BEEP);

    while (true) {
        LEF_Wait(&event);

        if (evOn) LEF_Print_event(&event);

        switch (event.id) {
            case EVENT_Button1:  // Handle button press event
                printf_P(PSTR("Button 1 event: func = %d\n"), event.func);
                LEF_Led_set(&led1, LED_SLOW_BLINK);
                LEF_Led_set(&led2, LED_BLINK);
                LEF_Led_set(&led3, LED_FAST_BLINK);
                LEF_Led_set(&led4, LED_BLIP);
                break;
            case EVENT_Button2:  // Handle button press event
                printf_P(PSTR("Button 2 event: func = %d\n"), event.func);
                break;
            case EVENT_Button3:  // Handle button press event
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

                printf_P(PSTR("Port C: %s  Clk = %d   Dt = %d\n"), int2bin8(PINE),
                       (PINC & (1 << PC0)), (PINC & (1 << PC1)));

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
                //printf("Pot changed %d\n", val);
                TIMER0_OCA(val / 4);
                char buf[41];
                LEF_Led_set(&led1, (val > 100));
                LEF_Led_set(&led2, (val > 300));
                LEF_Led_set(&led3, (val > 500));
                LEF_Led_set(&led4, (val > 800));
                
                lcd_gotoxy(0,2);
                for (uint16_t i=0;i<20;i++){
                    if (1023/20*i < val)
                        lcd_putc('#');
                    else
                        lcd_putc(' ');
                }
                lcd_gotoxy(0,3);
                sprintf_P(buf,PSTR("   Pot value %4d"), val);
                lcd_puts(buf);

                break;

            case LEF_EVENT_TEST:
                printf_P(PSTR("Testevent\n"));
                break;
        }
    }

    return 0;
}
