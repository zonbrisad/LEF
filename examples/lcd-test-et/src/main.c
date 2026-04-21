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
 * - AVR Atmega 128
 *
 *
 *
 *
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

#define LED_TEST B,5

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

#include "LEF/LEF.h"
#include "def.h"
#include "def_avr.h"
#include "uart.h"
#include "u8x8.h"


// Macros -------------------------------------------------------------------

static const uint32_t UART_BAUD_RATE = 57600;

typedef enum {
    EVENT_Timer1 = 0,
    EVENT_Timer2,
    EVENT_TimerA,
    EVENT_Button1,
    EVENT_Button2,
    EVENT_Button3,
} Events;

// void hw_init(void);

// void cmd_help(char* args);
// void cmd_disk(char* args);
// void cmd_reset(char* args);
// void cmd_lcd_on(char* args);
// void cmd_lcd_off(char* args);
// void cmd_lcd_clear(char* args);
// void cmd_lcd_home(char* args);
// void cmd_lcd_test(char* args);
// void cmd_lcd_test_move(char *args);
// void cmd_lcd_move_right(char *args);
// void cmd_lcd_move_left(char *args);
// void cmd_lcd_init(char *args);

// void cmd_lcd_ks0108_init(char *args);


LEF_Timer timer1;
LEF_Timer timer2;
LEF_Timer timer_a;
LEF_Button button1;
LEF_Button button2;
LEF_Led led_test;

bool evOn = false;
u8x8_t u8x8;

static FILE mystdout =
    FDEV_SETUP_STREAM((void*)uart1_putc, NULL, _FDEV_SETUP_WRITE);



// ============== PIN DEFINITIONS (change if you want) ==============
#define LCD_CS B,0
#define LCD_CLOCK B,1
#define LCD_DATA B,2
#define LCD_DC E,6
#define LCD_RESET E,7
// ==================================================================



// GPIO + Delay callback for Software SPI on AVR
uint8_t u8x8_gpio_and_delay(u8x8_t* u8x8x, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t u8x8_gpio_and_delay(u8x8_t* u8x8x, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(arg_ptr);
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            gpio_init(LCD_CLOCK, true, false);
            gpio_init(LCD_DATA, true, false);
            gpio_init(LCD_DC, true, false);
            gpio_init(LCD_CS, true, false);
            gpio_init(LCD_RESET, true, false);
            break;

        case U8X8_MSG_GPIO_D0:  // SPI Clock
            gpio_write(LCD_CLOCK, arg_int);
            break;
        case U8X8_MSG_GPIO_D1:  // SPI Data (MOSI)
            gpio_write(LCD_DATA, arg_int);
            break;
        case U8X8_MSG_GPIO_DC:
            gpio_write(LCD_DC, arg_int);
            break;
        case U8X8_MSG_GPIO_CS:
            gpio_write(LCD_CS, arg_int);
            break;
        case U8X8_MSG_GPIO_RESET:
            gpio_write(LCD_RESET, arg_int);
            break;
        case U8X8_MSG_DELAY_MILLI:
            while (arg_int--) _delay_ms(1);
            break;

        case U8X8_MSG_DELAY_10MICRO:
            while (arg_int--) _delay_us(10);
            break;

        default:
            u8x8_SetGPIOResult(u8x8x, 1);
            break;
    }
    return 1;
}


// Bolymin BG12864A 
#define BG_DB0 F,0
#define BG_DB1 F,1
#define BG_DB2 F,2
#define BG_DB3 F,3
#define BG_DB4 F,4
#define BG_DB5 F,5
#define BG_DB6 F,6
#define BG_DB7 F,7
#define BG_DC  A,5 // D/I
#define BG_RW  A,6 // Read write
#define BG_E   A,7  
#define BG_CS1 G,0 // chip select 1
#define BG_CS2 G,1 // chip select 2
#define BG_RST G,2 // Reset


/*
 * Simple example for ATmega128 + Bolymin BG12864A (KS0108/NT7108 controller)
 * Uses u8x8 library with pure software (bit-banged) 8-bit parallel interface.
 * 
 * HARDWARE CONNECTIONS (adjust if you want different pins):
 * 
 * LCD Pin  -->  ATmega128 Pin
 * DB0      -->  PA0
 * DB1      -->  PA1
 * DB2      -->  PA2
 * DB3      -->  PA3
 * DB4      -->  PA4
 * DB5      -->  PA5
 * DB6      -->  PA6
 * DB7      -->  PA7
 * E (Enable) --> PB0
 * RS/DC (A0) --> PB1
 * CS1      --> PB2   (left half of display)
 * CS2      --> PB3   (right half of display)
 * RESET    --> PB4
 * R/W      --> GND   (write-only mode - VERY IMPORTANT!)
 * 
 * Contrast: connect Vo (pin 3) to a 10k potentiometer between VDD and VSS (or -Vee if present).
 * Power:   VDD = +5V, VSS = GND. Backlight optional (LED+ / LED-).
 * 
 * You need the full u8g2 library[](https://github.com/olikraus/u8g2).
 * Add all files from csrc/ to your project (or the relevant u8x8_*.c files).
 * Compile with AVR-GCC (e.g. avr-gcc -mmcu=atmega128 ...).
 */

uint8_t ks0108_gpio_and_delay(u8x8_t *u8x8x, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t ks0108_gpio_and_delay(u8x8_t *u8x8x, uint8_t msg, uint8_t arg_int, void *arg_ptr){
    UNUSED(arg_ptr);
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            printf_P(PSTR("Initing gpio for ks0108\n"));
            
            gpio_init(BG_DC, true, false);
            gpio_init(BG_RW, true, false);
            gpio_init(BG_E, true, false);
            gpio_init(BG_CS1, true, false);
            gpio_init(BG_CS2, true, false);
            gpio_init(BG_DB0, true, false);
            gpio_init(BG_DB1, true, false);
            gpio_init(BG_DB2, true, false);
            gpio_init(BG_DB3, true, false);
            gpio_init(BG_DB4, true, false);
            gpio_init(BG_DB5, true, false);
            gpio_init(BG_DB6, true, false);
            gpio_init(BG_DB7, true, false);
            gpio_init(BG_RST, true, false);
            
            gpio_write(BG_RW, 1);  // set to low permanent
            gpio_write(BG_RST, 1);     // not reset
            
            break;
        /* Data bus D0..D7 (PORTA) */
        case U8X8_MSG_GPIO_D0: gpio_write(BG_DB0, arg_int); break; // if (arg_int) PORTA |= (1<<0); else PORTA &= ~(1<<0); break;
        case U8X8_MSG_GPIO_D1: gpio_write(BG_DB1, arg_int); break; // if (arg_int) PORTA |= (1<<1); else PORTA &= ~(1<<1); break;
        case U8X8_MSG_GPIO_D2: gpio_write(BG_DB2, arg_int); break; // if (arg_int) PORTA |= (1<<2); else PORTA &= ~(1<<2); break;
        case U8X8_MSG_GPIO_D3: gpio_write(BG_DB3, arg_int); break; // if (arg_int) PORTA |= (1<<3); else PORTA &= ~(1<<3); break;
        case U8X8_MSG_GPIO_D4: gpio_write(BG_DB4, arg_int); break; // if (arg_int) PORTA |= (1<<4); else PORTA &= ~(1<<4); break;
        case U8X8_MSG_GPIO_D5: gpio_write(BG_DB5, arg_int); break; // if (arg_int) PORTA |= (1<<5); else PORTA &= ~(1<<5); break;
        case U8X8_MSG_GPIO_D6: gpio_write(BG_DB6, arg_int); break; // if (arg_int) PORTA |= (1<<6); else PORTA &= ~(1<<6); break;
        case U8X8_MSG_GPIO_D7: gpio_write(BG_DB7, arg_int); break; // if (arg_int) PORTA |= (1<<7); else PORTA &= ~(1<<7); break;
        /* Control lines */
        case U8X8_MSG_GPIO_E:     gpio_write(BG_E, arg_int); break; //if (arg_int) PORTB |= (1<<PB0); else PORTB &= ~(1<<PB0); break;
        case U8X8_MSG_GPIO_DC:    gpio_write(BG_DC, arg_int); break; //if (arg_int) PORTB |= (1<<PB1); else PORTB &= ~(1<<PB1); break;

        // case U8X8_MSG_GPIO_CS1:   gpio_write(BG_CS1, arg_int); break; //if (arg_int) PORTB |= (1<<PB3); else PORTB &= ~(1<<PB3); break; // CS2 on LCD
        // case U8X8_MSG_GPIO_CS2:   gpio_write(BG_CS2, arg_int); break; //if (arg_int) PORTB |= (1<<PB4); else PORTB &= ~(1<<PB4); break; // usually not used
        case U8X8_MSG_GPIO_CS1:    gpio_write(BG_CS1, arg_int); break; //if (arg_int) PORTB |= (1<<PB3); else PORTB &= ~(1<<PB3); break; // CS2 on LCD
        case U8X8_MSG_GPIO_CS2:    gpio_write(BG_CS2, arg_int); break; //if (arg_int) PORTB |= (1<<PB4); else PORTB &= ~(1<<PB4); break; // usually not used

        // case U8X8_MSG_GPIO_RESET: gpio_write(BG_RST, arg_int); break; //if (arg_int) PORTB |= (1<<PB4); else PORTB &= ~(1<<PB4); break;
        
        /* Data bus D0..D7 (PORTA) */
        // case U8X8_MSG_GPIO_D0: if (arg_int) PORTA |= (1<<0); else PORTA &= ~(1<<0); break;
        // case U8X8_MSG_GPIO_D1: if (arg_int) PORTA |= (1<<1); else PORTA &= ~(1<<1); break;
        // case U8X8_MSG_GPIO_D2: if (arg_int) PORTA |= (1<<2); else PORTA &= ~(1<<2); break;
        // case U8X8_MSG_GPIO_D3: if (arg_int) PORTA |= (1<<3); else PORTA &= ~(1<<3); break;
        // case U8X8_MSG_GPIO_D4: if (arg_int) PORTA |= (1<<4); else PORTA &= ~(1<<4); break;
        // case U8X8_MSG_GPIO_D5: if (arg_int) PORTA |= (1<<5); else PORTA &= ~(1<<5); break;
        // case U8X8_MSG_GPIO_D6: if (arg_int) PORTA |= (1<<6); else PORTA &= ~(1<<6); break;
        // case U8X8_MSG_GPIO_D7: if (arg_int) PORTA |= (1<<7); else PORTA &= ~(1<<7); break;

        /* Control lines */
        // case U8X8_MSG_GPIO_E:     if (arg_int) PORTB |= (1<<PB0); else PORTB &= ~(1<<PB0); break;
        // case U8X8_MSG_GPIO_DC:    if (arg_int) PORTB |= (1<<PB1); else PORTB &= ~(1<<PB1); break;
        // case U8X8_MSG_GPIO_CS1:   if (arg_int) PORTB |= (1<<PB3); else PORTB &= ~(1<<PB3); break; // CS2 on LCD
        // case U8X8_MSG_GPIO_CS2:   if (arg_int) PORTB |= (1<<PB4); else PORTB &= ~(1<<PB4); break; // usually not used
        // case U8X8_MSG_GPIO_RESET: if (arg_int) PORTB |= (1<<PB4); else PORTB &= ~(1<<PB4); break;

        /* Delays */
        case U8X8_MSG_DELAY_NANO: _delay_us(arg_int / 1000 + 1); break;   // rough approximation
        //case U8X8_MSG_DELAY_10MICRO: break; //_delay_us(10 * arg_int); break;
      //  case U8X8_MSG_DELAY_MILLI:   _delay_ms(arg_int); break;

        case U8X8_MSG_DELAY_MILLI:
            while (arg_int--) _delay_ms(1);
            break;

        case U8X8_MSG_DELAY_10MICRO:
            while (arg_int--) _delay_us(10);
            break;

        default:
            u8x8_SetGPIOResult(u8x8x, 1);
            // return 0;   // not handled
    }
    return 1;
}
static void lcd_test() {
    u8x8_ClearDisplay(&u8x8);
    u8x8_SetFont(&u8x8, u8x8_font_5x7_f);
    u8x8_DrawString(&u8x8, 0, 0, "Hello World");  // col 0, row 0
    u8x8_SetFont(&u8x8, u8x8_font_5x8_f);
    u8x8_DrawString(&u8x8, 0, 1, "Hello World");  // col 0, row 1
    u8x8_SetFont(&u8x8, u8x8_font_7x14_1x2_f);
    u8x8_DrawString(&u8x8, 0, 2, "Hello World");  // col 0, row 1
    u8x8_SetFont(&u8x8, u8x8_font_chroma48medium8_r);
    u8x8_DrawString(&u8x8, 0, 3, "Hello World");  // col 0, row 1
}

static void cmd_lcd_ks0108_init(char* args) {
    UNUSED(args);
    printf_P(PSTR("Initializing LCD with ks0108\n"));
    /* Setup u8x8 with KS0108 driver + software 8-bit parallel (bit-banged) */
    u8x8_Setup(
        &u8x8,
        u8x8_d_ks0108_128x64,    // display type (matches Bolymin BG12864A)
        u8x8_cad_001,            // command/address procedure
        u8x8_byte_ks0108,        // special byte handler for KS0108 (handles dual CS)
        ks0108_gpio_and_delay);  // our AVR bit-bang + delay callback

    u8x8_InitDisplay(&u8x8);      // performs reset + basic init
    u8x8_SetPowerSave(&u8x8, 0);  // turn display on
    // u8x8_ClearDisplay(&u8x8);
    // u8x8_SetFont(&u8x8, u8x8_font_chroma48medium8_r);
    // u8x8_DrawString(&u8x8, 0, 0, "Hello World");  // col 0, row 1
    // u8x8_DrawString(&u8x8, 10, 0, "Hello World");  // col 0, row 1
    lcd_test();
}

static void cmd_help(char* args) {
    UNUSED(args);
    LEF_Cli_print();
}



static void cmd_lcd_init(char* args) {
    UNUSED(args);
    
    printf_P(PSTR("cmd_lcd_init\n"));
    // Setup the display (Nokia 5110 = PCD8544 84x48)
    u8x8_Setup(&u8x8,
               u8x8_d_pcd8544_84x48,    // display driver
               u8x8_cad_001,            // command/address mode
               u8x8_byte_4wire_sw_spi,  // 4-wire software SPI
               u8x8_gpio_and_delay);    // our callback

    u8x8_InitDisplay(&u8x8);
    u8x8_SetPowerSave(&u8x8, 0);  // turn display on
    lcd_test();
}

static void cmd_lcd_on(char* args) {
    UNUSED(args);
    u8x8_SetPowerSave(&u8x8, 0);  // turn display on
}

static void cmd_lcd_off(char* args) {
    UNUSED(args);
    u8x8_SetPowerSave(&u8x8, 1);  // turn display off
}

static void cmd_lcd_clear(char* args) {
    UNUSED(args);
    u8x8_ClearDisplay(&u8x8);
    _delay_ms(1);
}
static void cmd_lcd_home(char* args) {
    UNUSED(args);
    // u8x8_SetCursor(&u8x8, 0, 0);
    _delay_ms(1);
}

static void cmd_lcd_move_right(char* args) {
    UNUSED(args);
    _delay_ms(1);
}

static void cmd_lcd_move_left(char* args) {
    UNUSED(args);
    _delay_ms(1);
}

static void cmd_lcd_test(char* args) {
    UNUSED(args);
    lcd_test();
}

static void cmd_lcd_test_move(char* args) {
    UNUSED(args);
    _delay_ms(1);
}

static void cmd_reset(char* args) {
    UNUSED(args);
    RESET();
}

static void cmd_data_1(char* args) {
    UNUSED(args);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D0, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D1, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D2, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D3, 1, NULL);

    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D4, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D5, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D6, 1, NULL);
    ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D7, 1, NULL);

    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_E, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_RESET, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_DC, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_CS1, 1, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_CS2, 1, NULL);
}

static void cmd_data_0(char* args) {
    UNUSED(args);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D0, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D1, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D2, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D3, 0, NULL);

    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D4, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D5, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D6, 0, NULL);
    ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_D7, 0, NULL);

    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_E, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_RESET, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_DC, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_CS1, 0, NULL);
    // ks0108_gpio_and_delay(&u8x8, U8X8_MSG_GPIO_CS2, 0, NULL);
    
}

const PROGMEM LEF_CliCmd cmdTable[] = {
    LEF_CLI_LABEL("LCD"),
    {cmd_lcd_init, "lcdinit", "Initialize Nokia 5110 LCD"},
    {cmd_lcd_ks0108_init, "ks0108", "Initialize LCD with ks0108"},
    {cmd_lcd_on, "lcdon", "Turn LCD on"},
    {cmd_lcd_off, "lcdoff", "Turn LCD off"},
    {cmd_lcd_clear, "lcdclr", "Clear LCD"},
    {cmd_lcd_home, "lcdh", "Move cursor to home pos"},
    {cmd_lcd_move_right, "lcdr", "Move text right"},
    {cmd_lcd_move_left, "lcdl", "Move text left"},
    {cmd_lcd_test, "lcdt1", "Run LCD test"},
    {cmd_lcd_test_move, "lcdtm", "Run LCD move test"},
    {cmd_data_1, "data1", "Test data 1"},
    {cmd_data_0, "data0", "Test data 0"},

    LEF_CLI_LABEL("Misc"),
    {cmd_reset, "reset", "Reset the system"},
    LEF_CLI_CMD(cmd_help, "help", "Show help")};

ISR(TIMER1_COMPA_vect) {

    LEF_Timer_update(&timer1);
    LEF_Timer_update(&timer2);
    LEF_Timer_update(&timer_a);

}

static void hw_init(void) {
    stdout = &mystdout;
    uart1_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    ARDUINO_LED_INIT();

   

    // Timer 1 (16 bit) 10ms intervall on OCA1 interrupt 
    // (used for LEF system timer)
    TIMER1_CLK_PRES_64(); // alternative for 10ms that gives good accuracy
    TIMER1_OCA(2499);
    TIMER1_WGM_CTC();       // Clear timer on compare (on OC1A)
    // TIMER1_COM_OC1A_TOGGLE(); // toggle OC1A pin when compare
    // gpio_init(TIMER1_PIN_OC1A, true, false);
    TIMER1_OCA_INT(true);  // enable output compare A interrupt

    sei();
}


static bool main_event_handler(LEF_Event* event) {
    uint16_t ch;
    switch (event->id) {
        case EVENT_Button1:  // Handle button press event
            break;
        case EVENT_Button2:  // Handle button press event
            break;
        case EVENT_Button3:  // Handle button press event
            break;
        case EVENT_Timer2:  // Handle data from uart to Cli
            ch = uart1_getc();
            while ((ch & 0xff00) != UART_NO_DATA) {
                LEF_Cli_putc(ch);
                ch = uart1_getc();
            }
            break;

        case LEF_EVENT_CLI:
            LEF_Cli_exec(event);
            break;
    }
    return false; // return true if event was handled
}

int main(void) {
  
    LEF_init();

    LEF_Timer_init(&timer1, EVENT_Timer1);
    LEF_Timer_start_repeat(&timer1, 100);
    LEF_Timer_init(&timer2, EVENT_Timer2);
    LEF_Timer_start_repeat(&timer2, 10);
    LEF_Timer_init(&timer_a, EVENT_TimerA);
    
    LEF_CLI_INIT(cmdTable);
    
    hw_init();
    // cmd_lcd_ks0108_init("");

    printf_P(PSTR("\n\nLCD testprogram (ET-AVR-STAMP)\n\n"));

    LEF_Run(main_event_handler, NULL);

    return 0;
}
