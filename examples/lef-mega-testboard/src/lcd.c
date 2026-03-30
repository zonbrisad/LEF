/****************************************************************************
 Title:     HD44780U LCD library
 Author:    Peter Fleury <pfleury@gmx.ch>  http://tinyurl.com/peterfleury
 File:	    $Id: lcd.c,v 1.15.2.2 2015/01/17 12:16:05 peter Exp $
 Software:  AVR-GCC 3.3
 Target:    any AVR device, memory mapped mode only for AT90S4414/8515/Mega

 DESCRIPTION
       Basic routines for interfacing a HD44780U-based text lcd display

       Originally based on Volker Oth's lcd library,
       changed lcd_init(), added additional constants for lcd_command(),
       added 4-bit I/O mode, improved and optimized code.

       Library can be operated in memory mapped mode (LCD_IO_MODE=0) or in
       4-bit IO port mode (LCD_IO_MODE=1). 8-bit IO port mode not supported.

       Memory mapped mode compatible with Kanda STK200, but supports also
       generation of R/W signal through A8 address line.

 USAGE
       See the C include lcd.h file for a description of each function

*****************************************************************************/
#include "lcd.h"

#include <inttypes.h>
#include <stdbool.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#include <util/delay.h>
#endif

/*************************************************************************
delay for a minimum of <us> microseconds
the number of loops is calculated at compile-time from MCU clock frequency
*************************************************************************/

hd4470_callback lcd_callback;
volatile bool lcd_wrap = true;

#if HD44780_CONTROLLER_KS0073
// const uint8_t line_offsets[] = {0x00, 0x20, 0x40, 0x60};
const uint8_t line_offsets[] = {00, 32, 64, 96};
#else
// const uint8_t line_offsets[] = {0x00, 0x40, 0x14, 0x54};
const uint8_t line_offsets[] = {00, 64, 20, 84};
#endif

static inline void lcd_delay(uint16_t us)  { _delay_us(us); }
inline void lcd_e_delay(void)              { lcd_callback(HD44780_MSG_DELAY_E, 0); }
inline void lcd_init_gpio(void)            { lcd_callback(HD44780_MSG_INIT, 0); }  // initiate gpio pins
inline void lcd_e_set(bool state)          { lcd_callback(HD44780_MSG_GPIO_E, state); }
inline void lcd_e_toggle(void)             { lcd_callback(HD44780_MSG_GPIO_E_TOGGLE, 0); }
inline void lcd_rw_set(bool state)         { lcd_callback(HD44780_MSG_GPIO_RW, state); }
inline void lcd_rs_set(bool state)         { lcd_callback(HD44780_MSG_GPIO_RS, state); }
inline void lcd_data_direction(bool write) { lcd_callback(HD44780_MSG_GPIO_DATA_DIRECTION, write);}
inline void lcd_data_write(uint8_t data)   { lcd_callback(HD44780_MSG_GPIO_DATA_WRITE, data); }
inline uint8_t lcd_data_read(void)         { return lcd_callback(HD44780_MSG_GPIO_DATA_READ, 0); }


#define HD44780_START_LINE1  line_offsets[0]
#define HD44780_START_LINE2  line_offsets[1]
#define HD44780_START_LINE3  line_offsets[2]
#define HD44780_START_LINE4  line_offsets[3]


#if HD44780_LINES == 1
#define LCD_FUNCTION_DEFAULT HD44780_FUNC_4BIT_LINE
#else
#define LCD_FUNCTION_DEFAULT HD44780_FUNC_4BIT_2LINES
#endif

#if HD44780_CONTROLLER_KS0073
#if LCD_LINES == 4

#define KS0073_EXTENDED_FUNCTION_REGISTER_ON \
    0x2C /* |0|010|1100 4-bit mode, extension-bit RE = 1 */
#define KS0073_EXTENDED_FUNCTION_REGISTER_OFF \
    0x28 /* |0|010|1000 4-bit mode, extension-bit RE = 0 */
#define KS0073_4LINES_MODE 0x09 /* |0|000|1001 4 lines mode */

#endif
#endif


/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data
                 0: write instruction
Returns:  none
*************************************************************************/
static void lcd_write(uint8_t data, uint8_t rs) {

    lcd_rs_set(rs);
    lcd_rw_set(0);  /* RW=0  write mode      */

    lcd_data_direction(true);
    lcd_data_write(data);
    lcd_e_toggle();
        
    lcd_data_write(data<<4);
    lcd_e_toggle();

    lcd_data_write(0b11110000);
}

/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     1: read data
                 0: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/
static uint8_t lcd_read(uint8_t rs) {
    uint8_t data = 0;

    lcd_rs_set(rs);
    lcd_rw_set(1);    /* RW=1  read mode      */

    lcd_data_direction(false);  // set data pins as inputs

    /* read high nibble first */
    lcd_e_set(1);
    lcd_e_delay();
    data = lcd_data_read();
    lcd_e_set(0);

    data = data << 4;

    lcd_e_delay();  /* Enable 500ns low       */

    /* read low nibble */
    lcd_e_set(1);
    lcd_e_delay();
    data |= lcd_data_read();
    lcd_e_set(0);
    return data;
}

/*************************************************************************
loops while lcd is busy, returns address counter
*************************************************************************/
static uint8_t lcd_waitbusy(void) {
    register uint8_t c;
    uint8_t cnt = 0;

    /* wait until busy flag is cleared */
    while ((c = lcd_read(0)) & HD44780_BUSY_FLAG) {
        cnt++;
        if (cnt > 3)
            break;
    }

    /* the address counter is updated 4us after the busy flag is cleared */
    lcd_delay(HD44780_DELAY_BUSY_FLAG);

    /* now read the address counter */
    return (lcd_read(0));  // return address counter

}

/*************************************************************************
Move cursor to the start of next line or to the first line if the cursor
is already on the last line.
*************************************************************************/
static inline void lcd_newline(uint8_t pos) {
    uint8_t addressCounter;
    // register uint8_t addressCounter;

#if HD44780_LINES == 1
    addressCounter = 0;
#endif
#if HD44780_LINES == 2
    if (pos < (HD44780_START_LINE2))
        addressCounter = HD44780_START_LINE2;
    else
        addressCounter = HD44780_START_LINE1;
#endif
#if HD44780_LINES == 4
#if KS0073_4LINES_MODE
    if (pos < HD44780_START_LINE2)
        addressCounter = HD44780_START_LINE2;
    else if ((pos >= HD44780_START_LINE2) && (pos < HD44780_START_LINE3))
        addressCounter = HD44780_START_LINE3;
    else if ((pos >= HD44780_START_LINE3) && (pos < HD44780_START_LINE4))
        addressCounter = HD44780_START_LINE4;
    else
        addressCounter = HD44780_START_LINE1;
#else
    if (pos < HD44780_START_LINE3)
        addressCounter = HD44780_START_LINE2;
    else if ((pos >= HD44780_START_LINE2) && (pos < HD44780_START_LINE4))
        addressCounter = HD44780_START_LINE3;
    else if ((pos >= HD44780_START_LINE3) && (pos < HD44780_START_LINE2))
        addressCounter = HD44780_START_LINE4;
    else
        addressCounter = HD44780_START_LINE1;
#endif
#endif
    // printf("Newline: %d\n", addressCounter);
    lcd_command(HD44780_INST_DDRAM + addressCounter);
} 


/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_command(uint8_t cmd) {
    lcd_waitbusy();
    lcd_write(cmd, 0);
}

/*************************************************************************
Send data byte to LCD controller
Input:   data to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_data(uint8_t data) {
    lcd_waitbusy();
    lcd_write(data, 1);
}

/*************************************************************************
Set cursor to specified position
Input:    x  horizontal position  (0: left most position)
          y  vertical position    (0: first line)
Returns:  none
*************************************************************************/
void lcd_gotoxy(uint8_t x, uint8_t y) {
    lcd_command(HD44780_INST_DDRAM + line_offsets[y] + x);
}

/*************************************************************************
*************************************************************************/
int lcd_getxy(void) { return lcd_waitbusy(); }

/*************************************************************************
Display character at current cursor position
Input:    character to be displayed
Returns:  none
*************************************************************************/
void lcd_putc(char c) {
    //static bool newline = false;
    uint8_t pos;

    pos = lcd_waitbusy();  // read busy-flag and address counter
    if (c == '\n') {
        lcd_newline(pos);
        //newline=true;
        return;
    }

    uint8_t new_pos = 255;
    if (lcd_wrap == true) {
        if (pos == (HD44780_START_LINE1 + HD44780_DISP_LENGTH-1)) {
            new_pos = HD44780_START_LINE2;
        } else if (pos == (HD44780_START_LINE2 + HD44780_DISP_LENGTH-1)) {
            new_pos = HD44780_START_LINE3;
        } else if (pos == (HD44780_START_LINE3 + HD44780_DISP_LENGTH-1)) {
            new_pos = HD44780_START_LINE4;
        } else if (pos == (HD44780_START_LINE4 + HD44780_DISP_LENGTH-1)) {
            new_pos = HD44780_START_LINE1;
        }
        //printf("%d ",pos);
        //printf("Wrap pos=%d  new_pos=%d\n", pos, new_pos);
    }

write:    
    lcd_write(c, 1);
    //newline = false;
    // if ((new_pos != 255) || (newline)) {
    if (new_pos != 255) {
        lcd_waitbusy();
        lcd_write(HD44780_INST_DDRAM + new_pos, 0);
    }
} 

/*************************************************************************
Display string without auto linefeed
Input:    string to be displayed
Returns:  none
*************************************************************************/
/* print string on lcd (no auto linefeed) */
void lcd_puts(const char* s) {
    register char c;

    while ((c = *s++)) {
        lcd_putc(c);
    }

} 

#if defined(__AVR__)
/*************************************************************************
Display string from program memory without auto linefeed
Input:     string from program memory be be displayed
Returns:   none
*************************************************************************/
void lcd_puts_p(const char* progmem_s) {
    /* print string from program memory on lcd (no auto linefeed) */
    register char c;

    while ((c = pgm_read_byte(progmem_s++))) {
        lcd_putc(c);
    }

} 
#endif 

extern void lcd_init(hd4470_callback callback, uint8_t dispAttr) {
    lcd_callback = callback;

    lcd_init_gpio();            // initiate gpio pins
    lcd_data_direction(true);
    lcd_data_write(0b11110000);

    lcd_delay(HD44780_DELAY_BOOTUP); /* wait 16ms or more after power-on */

    lcd_data_write(HD44780_INIT_SEQ); /* initial write to lcd is 8bit */

    lcd_e_toggle();
    lcd_delay(HD44780_DELAY_INIT); /* delay, busy flag can't be checked here */

    lcd_e_toggle();
    lcd_delay(HD44780_DELAY_INIT_REP); /* repeat last command, delay, busy flag can't be checked here */

    lcd_e_toggle();
    lcd_delay(HD44780_DELAY_INIT_REP); /* repat third time. delay, busy flag can't be checked here */

    lcd_data_write(HD44780_4BIT_MODE); /* now configure for 4bit mode */
    lcd_e_toggle();
    lcd_delay(HD44780_DELAY_INIT_4BIT); /* some displays need this additional delay */

    /* from now the LCD only accepts 4 bit I/O, we can use lcd_command() */

#if KS0073_4LINES_MODE
    /* Display with KS0073 controller requires special commands for enabling 4
     * line mode */
    lcd_command(KS0073_EXTENDED_FUNCTION_REGISTER_ON);
    lcd_command(KS0073_4LINES_MODE);
    lcd_command(KS0073_EXTENDED_FUNCTION_REGISTER_OFF);
#else
    lcd_command(LCD_FUNCTION_DEFAULT); /* function set: display lines  */
#endif
    lcd_command(HD44780_OFF);     /* display off                  */
    lcd_clear();                   /* display clear                */
    lcd_command(HD44780_MODE_DEFAULT); /* set entry mode               */
    lcd_command(dispAttr);         /* display/cursor control       */
}

/*************************************************************************
Initialize display and select type of cursor
Input:    dispAttr LCD_DISP_OFF            display off
                   LCD_DISP_ON             display on, cursor off
                   LCD_DISP_ON_CURSOR      display on, cursor on
                   LCD_DISP_CURSOR_BLINK   display on, cursor on flashing
Returns:  none
*************************************************************************/