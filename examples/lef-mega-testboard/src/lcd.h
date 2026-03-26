/*************************************************************************
 Title	:   C include file for the HD44780U LCD library (lcd.c)
 Author:    Peter Fleury <pfleury@gmx.ch>  http://tinyurl.com/peterfleury
 File:	    $Id: lcd.h,v 1.14.2.4 2015/01/20 17:16:07 peter Exp $
 Software:  AVR-GCC 4.x
 Hardware:  any AVR device, memory mapped mode only for AVR with 
 memory mapped interface (AT90S8515/ATmega8515/ATmega128)
 ***************************************************************************/

/**
 @mainpage
 Collection of libraries for AVR-GCC
 @author Peter Fleury pfleury@gmx.ch http://tinyurl.com/peterfleury
 @copyright (C) 2015 Peter Fleury, GNU General Public License Version 3
 
 @file
 @defgroup pfleury_lcd LCD library <lcd.h>
 @code #include <lcd.h> @endcode
 
 @brief Basic routines for interfacing a HD44780U-based character LCD display
 
 LCD character displays can be found in many devices, like espresso machines, laser printers. 
 The Hitachi HD44780 controller and its compatible controllers like Samsung KS0066U have become an industry standard for these types of displays. 
 
 This library allows easy interfacing with a HD44780 compatible display and can be
 operated in memory mapped mode (LCD_IO_MODE defined as 0 in the include file lcd.h.) or in 
 4-bit IO port mode (LCD_IO_MODE defined as 1). 8-bit IO port mode is not supported.
 
 Memory mapped mode is compatible with old Kanda STK200 starter kit, but also supports
 generation of R/W signal through A8 address line.
 
 @see The chapter <a href=" http://homepage.hispeed.ch/peterfleury/avr-lcd44780.html" target="_blank">Interfacing a HD44780 Based LCD to an AVR</a>
 on my home page, which shows example circuits how to connect an LCD to an AVR controller. 
 
 @author Peter Fleury pfleury@gmx.ch http://tinyurl.com/peterfleury
 
 @version   2.0
 
 @copyright (C) 2015 Peter Fleury, GNU General Public License Version 3
 
 */

/** External references
 * https://github.com/damadmai/pfleury
 * https://github.com/efthymios-ks/AVR-HD44780
 * https://github.com/aostanin/avr-hd44780?tab=readme-ov-file
 * 
 */

#pragma once

#include <inttypes.h>
#include <stdbool.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#endif

typedef enum {
    HD44780_MSG_INIT,
    HD44780_MSG_GPIO_E,
    HD44780_MSG_GPIO_RS,
    HD44780_MSG_GPIO_RW,
    HD44780_MSG_GPIO_E_TOGGLE,
    HD44780_MSG_GPIO_DATA_READ,
    HD44780_MSG_GPIO_DATA_WRITE,
    HD44780_MSG_GPIO_DATA_DIRECTION,
    HD44780_MSG_delay_ms,
    HD44780_MSG_delay_us,
    HD44780_MSG_DELAY_E,
} HD44780_LCD;

typedef uint16_t (*hd4470_callback)(HD44780_LCD msg, uint16_t data);

/**@{*/

/**
 * @name  Definition for LCD controller type
 * Use 0 for HD44780 controller, change to 1 for displays with KS0073 controller.
 */
#define HD44780_CONTROLLER_KS0073 0  /**< Use 0 for HD44780 controller, 1 for KS0073 controller */

/** 
 * @name  Definitions for Display Size 
 * Change these definitions to adapt setting to your display
 *
 * These definitions can be defined in a separate include file \b lcd_definitions.h instead modifying this file by 
 * adding -D_LCD_DEFINITIONS_FILE to the CDEFS section in the Makefile.
 * All definitions added to the file lcd_definitions.h will override the default definitions from lcd.h
 *
 */
#define HD44780_LINES           4     /**< number of visible lines of the display */
#define HD44780_DISP_LENGTH    20     /**< visibles characters per line of the display */
#define HD44780_LINE_LENGTH  0x40     /**< internal line length of the display    */
#define HD44780_WRAP_LINES      1     /**< 0: no wrap, 1: wrap at end of visibile line */

/**
 * @name Definitions for 4-bit IO mode
 *
 * The four LCD data lines and the three control lines RS, RW, E can be on the 
 * same port or on different ports. 
 * Change LCD_RS_PORT, LCD_RW_PORT, LCD_E_PORT if you want the control lines on
 * different ports. 
 *
 * Normally the four data lines should be mapped to bit 0..3 on one port, but it
 * is possible to connect these data lines in different order or even on different
 * ports by adapting the LCD_DATAx_PORT and LCD_DATAx_PIN definitions.
 *
 * Adjust these definitions to your target.\n 
 * These definitions can be defined in a separate include file \b lcd_definitions.h instead modifying this file by 
 * adding \b -D_LCD_DEFINITIONS_FILE to the \b CDEFS section in the Makefile.
 * All definitions added to the file lcd_definitions.h will override the default definitions from lcd.h
 *  
 */



/**
 * @name Definitions of delays
 * Used to calculate delay timers.
 * Adapt the F_CPU define in the Makefile to the clock frequency in Hz of your target
 *
 * These delay times can be adjusted, if some displays require different delays.\n 
 * These definitions can be defined in a separate include file \b lcd_definitions.h instead modifying this file by 
 * adding \b -D_LCD_DEFINITIONS_FILE to the \b CDEFS section in the Makefile.
 * All definitions added to the file lcd_definitions.h will override the default definitions from lcd.h
 */
#define HD44780_DELAY_BOOTUP   16000      /**< delay in micro seconds after power-on  */
#define HD44780_DELAY_INIT      5000      /**< delay in micro seconds after initialization command sent  */
#define HD44780_DELAY_INIT_REP    64      /**< delay in micro seconds after initialization command repeated */
#define HD44780_DELAY_INIT_4BIT   64      /**< delay in micro seconds after setting 4-bit mode */ 
#define HD44780_DELAY_BUSY_FLAG    4      /**< time in micro seconds the address counter is updated after busy flag is cleared */
#define HD44780_DELAY_ENABLE_PULSE 1      /**< enable signal pulse width in micro seconds */

/**
 * @name Definitions for LCD command instructions
 * The constants define the various LCD controller instructions which can be passed to the 
 * function lcd_command(), see HD44780 data sheet for a complete description.
 */
/*
 * HD44780 instructions table
 *
 * Instruction         RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0  Description
 * Clear display       0   0   0   0   0   0   0   0   0   1   Clear entire display
 * Return home         0   0   0   0   0   0   0   0   1   -   Return cursor to home
 * Entry mode set      0   0   0   0   0   0   0   1  I/D  S   Set cursor move direction(I/D) and 
 *                                                             specifies display shift (S)
 * Display control     0   0   0   0   0   0   1   D   C   B   Sets entire display (D) on/off, 
 *                                                             cursor on/off (C), and 
 *                                                             blinking of cursor position character (B).
 * Cursor/display move 0   0   0   0   0   1  S/C R/L  -   -   Moves cursor and shifts display 
 *                                                             without changing DDRAM contents.
 * Function set        0   0   0   0   1   DL  N   F   -   -   Sets interface data length (DL), 
 *                                                             number of display lines (N), 
 *                                                             and character font (F).
 * Set CGRAM address   0   0   0   1  ACG ACG ACG ACG ACG ACG  Sets CGRAM address 
 * Set DDRAM address   0   0   1  ADD ADD ADD ADD ADD ADD ADD  Sets DDRAM address
 * Busyflag & address  0   1   BF  AC  AC  AC  AC  AC  AC  AC  Reads busy flag (BF), and
 *                                                             address counter (AC)
 * I/D = 1:  Increment
 * I/D = 0:  Decrement
 * S   = 1:  Accompanies display shift
 * S/C = 1:  Display shift
 * S/C = 0:  Cursor move
 * R/L = 1:  Shift to the right
 * R/L = 0:  Shift to the left
 * DL  = 1:  8 bits
 * DL  = 0:  4 bits
 * N   = 1:  2 lines
 * N   = 0:  1 line
 * F   = 1:  5x10 dots
 * F   = 0:  5x8 dots
 * BF  = 1:  Internaly operating (busy)
 * BF  = 0:  Instructions acceptable (not busy)
 *        
 * 
 * DDRAM: Display Data Ram
 * CGRAM: Character Generator RAM
 * ACG:   CGRAM address
 * ADD:   DDRAM address
 * AC: Address Counter                                                            
 */


#define HD44780_INST_CLEAR          0b00000001
#define HD44780_INST_HOME           0b00000010
#define HD44780_INST_ENTRY_MODE     0b00000100
#define HD44780_INST_CONTROL        0b00001000
#define HD44780_INST_MOVE           0b00010000
#define HD44780_INST_FUNCTION       0b00100000
#define HD44780_INST_CGRAM          0b01000000
#define HD44780_INST_DDRAM          0b10000000

#define HD44780_BUSY_FLAG          0b10000000

#define HD44780_ENTRY_DEC_NOSHIFT  (HD44780_INST_ENTRY_MODE | 0b00000000) // decrease cursor on move, display shift off
#define HD44780_ENTRY_DEC_SHIFT    (HD44780_INST_ENTRY_MODE | 0b00000001) // decrease cursor on move, display shift on
#define HD44780_ENTRY_INC_NOSHIFT  (HD44780_INST_ENTRY_MODE | 0b00000010) // decrease cursor on move, display shift off
#define HD44780_ENTRY_INC_SHIFT    (HD44780_INST_ENTRY_MODE | 0b00000011) // decrease cursor on move, display shift on

#define HD44780_OFF                (HD44780_INST_CONTROL | 0b00000000)
#define HD44780_ON                 (HD44780_INST_CONTROL | 0b00000100)
#define HD44780_ON_CURSOR          (HD44780_INST_CONTROL | 0b00000110)
#define HD44780_ON_CURSOR_BLINK    (HD44780_INST_CONTROL | 0b00000111)

#define HD44780_MOVE_CURSOR_LEFT   (HD44780_INST_MOVE | 0b00000000)
#define HD44780_MOVE_CURSOR_RIGHT  (HD44780_INST_MOVE | 0b00000100)
#define HD44780_MOVE_DISPLAY_LEFT  (HD44780_INST_MOVE | 0b00001000)
#define HD44780_MOVE_DISPLAY_RIGHT (HD44780_INST_MOVE | 0b00001100)



// #define LCD_FUNCTION          5      /* DB5: function set                   */
// #define LCD_FUNCTION_8BIT     4      /*   DB4: set 8BIT mode (0->4BIT mode) */
// #define LCD_FUNCTION_2LINES   3      /*   DB3: two lines (0->one line)      */
#define LCD_FUNCTION_10DOTS   2      /*   DB2: 5x10 font (0->5x7 font)      */
#define HD44780_FUNC_8BIT     0b00010000
#define HD44780_FUNC_2LINES   0b00001000
#define HD44780_FUNC_10DOTS   0b00000100   

#define HD44780_FUNC_4BIT_1LINE      (HD44780_INST_FUNCTION)
#define HD44780_FUNC_4BIT_2LINES     (HD44780_INST_FUNCTION | HD44780_FUNC_2LINES)
#define HD44780_FUNC_8BIT_1LINE      (HD44780_INST_FUNCTION | HD44780_FUNC_8BIT)
#define HD44780_FUNC_8BIT_2LINES     (HD44780_INST_FUNCTION | HD44780_FUNC_8BIT | HD44780_FUNC_2LINES)
// #define LCD_FUNCTION_4BIT_1LINE  0x20   /* 4-bit interface, single line, 5x7 dots */
// #define LCD_FUNCTION_4BIT_2LINES 0x28   /* 4-bit interface, dual line,   5x7 dots */
// #define LCD_FUNCTION_8BIT_1LINE  0x30   /* 8-bit interface, single line, 5x7 dots */
// #define LCD_FUNCTION_8BIT_2LINES 0x38   /* 8-bit interface, dual line,   5x7 dots */


#define HD44780_MODE_DEFAULT       HD44780_ENTRY_INC_NOSHIFT
#define HD44780_INIT_SEQ           0b00110000  
#define HD44780_4BIT_MODE          0b00100000  // Put LCD in 4 bit mode

extern bool lcd_wrap;

/**
 @brief    Initialize display and select type of cursor
 @param    dispAttr \b LCD_DISP_OFF display off\n
                    \b LCD_DISP_ON display on, cursor off\n
                    \b LCD_DISP_ON_CURSOR display on, cursor on\n
                    \b LCD_DISP_ON_CURSOR_BLINK display on, cursor on flashing             
 @return  none
*/
extern void lcd_init(hd4470_callback callback, uint8_t dispAttr);

/**
 @brief    Send LCD controller instruction command
 @param    cmd instruction to send to LCD controller, see HD44780 data sheet
 @return   none
*/
extern void lcd_command(uint8_t cmd);

/**
 * Clear display and set cursor to home position
 */
inline void lcd_clear(void) { lcd_command(HD44780_INST_CLEAR); }

inline void lcd_wrap_enable(bool enable) { lcd_wrap = enable; }

/**
 * Set cursor to home position
 */
inline void lcd_home(void) { lcd_command(HD44780_INST_HOME); }

inline void lcd_move_right(void) { lcd_command(HD44780_MOVE_DISPLAY_RIGHT); }
inline void lcd_move_left(void) { lcd_command(HD44780_MOVE_DISPLAY_LEFT); }

/**
 * Turn display on
 */
inline void lcd_on(void) { lcd_command(HD44780_ON); }

/**
 * Turn display on and show cursor
 */
inline void lcd_on_cursor(void) { lcd_command(HD44780_ON_CURSOR); }

/** 
 * Turn display off     
 */
inline void lcd_off(void) { lcd_command(HD44780_OFF); }


/**
 @brief    Set cursor to specified position
 
 @param    x horizontal position\n (0: left most position)
 @param    y vertical position\n   (0: first line)
 @return   none
*/
extern void lcd_gotoxy(uint8_t x, uint8_t y);

extern int lcd_getxy(void);

/**
 @brief    Display character at current cursor position
 @param    c character to be displayed                                       
 @return   none
*/
extern void lcd_putc(char c);


/**
 @brief    Display string without auto linefeed
 @param    s string to be displayed                                        
 @return   none
*/
extern void lcd_puts(const char *s);




/**
 @brief    Send data byte to LCD controller 
 
 Similar to lcd_putc(), but without interpreting LF
 @param    data byte to send to LCD controller, see HD44780 data sheet
 @return   none
 */
extern void lcd_data(uint8_t data);

#if defined(__AVR__)
/**
 @brief    Display string from program memory without auto linefeed
 @param    progmem_s string from program memory be be displayed                                        
 @return   none
 @see      lcd_puts_P
*/
extern void lcd_puts_p(const char *progmem_s);

/**
 @brief macros for automatically storing string constant in program memory
*/
#define lcd_puts_P(__s)         lcd_puts_p(PSTR(__s))

#endif
/**@}*/

