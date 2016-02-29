/**
 * @file  def.h
 * @brief Some common definitions
 *
 *
 * @author Peter Malmberg <peter.malmberg@gmail.com>
 * @date 
 *
 *
 */


#ifndef DEF_H
#define	DEF_H

//#define ARDUINO_MEGA
#define ARDUINO


/*-----------------------------------------------------------------*/
#ifndef MAXCHAR
#define MAXCHAR         255
#endif

#ifndef MAXUCHAR
#define MAXUCHAR        255
#endif

#ifndef MAXSCHAR
#define MAXSCHAR        127
#endif

#ifndef MAXINT
#define MAXINT          32767
#endif

#ifndef MAXUINT
#define MAXUINT         65535
#endif

#ifndef MAXSINT
#define MAXSINT         32767
#endif

#ifndef MAXLONG
#define MAXLONG         2147483647
#endif

#ifndef MAXULONG
#define MAXULONG        4294967295
#endif

#ifndef MAXSLONG
#define MAXSLONG        2147483647
#endif

/*-----------------------------------------------------------------*/
#ifndef FALSE
#define FALSE           0
#endif

#ifndef TRUE
#define TRUE            1
#endif

#ifndef NULL
#define NULL			(void)0
#endif

/*-----------------------------------------------------------------*/
#define NUL             0
#define SOH             1
#define STX             2
#define ETX             3
#define EOT             4
#define ENQ             5
#define ACK             6
#define BEL             7
#define BS              8
#define HT              9
#define LF              10
#define VT              11
#define FF              12
#define CR              13
#define SO              14
#define SI              15
#define DLE             16
#define DC1             17
#define DC2             18
#define DC3             19
#define DC4             20
#define NAK             21
#define SYN             22
#define ETB             23
#define CAN             24
#define EM              25
#define SUB             26
#define ESC             27
#define FS              28
#define GS              29
//#define RS            30
#define US              31

/**
 * Linux signals.
 * -----------------------------------------------------------------*/


#define SIGHUP          1       // Hangup	
#define SIGINT          2       // Interrupt	
#define SIGQUIT         3       // Quit and dump core
#define SIGILL          4       // Illegal instruction
#define SIGTRAP         5       // Trace/breakpoint trap	
#define SIGABRT         6       // Process aborted
#define SIGBUS          7       // Bus error: "access to undefined portion of memory object"
#define SIGFPE          8       // Floating point exception: "erroneous arithmetic operation"
#define SIGKILL         9       // Kill (terminate immediately)
#define SIGUSR1         10      // User-defined 1	
#define SIGSEGV         11      // Segmentation violation
#define SIGUSR2         12      // User-defined 2	
#define SIGPIPE         13      // Write to pipe with no one reading
#define SIGALRM         14      // Signal raised by alarm
#define SIGTERM         15      // Termination (request to terminate)
#define SIGCHLD         17      // Child process terminated, stopped (or continued*)
#define SIGCONT         18      // Continue if stopped
#define SIGSTOP	        19      // Stop executing temporarily	
#define SIGTSTP         20      // Terminal stop signal	
#define SIGTTIN	        21      // Background process attempting to read from tty ("in")	
#define SIGTTOU         22      // Background process attempting to write to tty ("out")	
#define SIGURG	        23      // Urgent data available on socket	
#define SIGXCPU         24      // CPU time limit exceeded	
#define SIGXFSZ         25      // File size limit exceeded	
#define SIGVTALRM       26      // Signal raised by timer counting virtual time: "virtual timer expired"	
#define SIGPROF         27      // Profiling timer expired	
#define SIGPOLL         29      // Pollable event	
#define SIGSYS          31      // Bad syscall	

/**
 * Atmel AVR specific defs.
 * -----------------------------------------------------------------*/

/**
 * Reset causes.
 */
#define POWER_ON_RESET()            (MCUSR & (1<<PORF))
#define BROWN_OUT_RESET()           (MCUSR & (1<<BORF))
#define WATCH_DOG_RESET()           (MCUSR & (1<<WDRF))
#define JTAG_RESET_RESET()          (MCUSR & (1<<JTRF))
#define EXTERNAL_RESET()            (MCUSR & (1<<EXTRF))
#define CLEAR_RESETS()              MCUSR  &= ~31              /* clearing all resets */


/**
 * Reset MCU with watchdog.
 */
#define RESET()                     wdt_enable(WDTO_500MS); while(1) {}


/**
 * Timers.
 */
#define TIMER0_DISSABLE()     TCCR0B &= 0xF8                  /* Disable timer                  */
#define TIMER0_PRES_1()       TIMER0_DISSABLE(); TCCR0B |= 1  /* Select prescaler 1/1           */
#define TIMER0_PRES_8()       TIMER0_DISSABLE(); TCCR0B |= 2  /* Select prescaler 1/8           */
#define TIMER0_PRES_64()      TIMER0_DISSABLE(); TCCR0B |= 3  /* Select prescaler 1/64          */
#define TIMER0_PRES_256()     TIMER0_DISSABLE(); TCCR0B |= 4  /* Select prescaler 1/256         */
#define TIMER0_PRES_1024()    TIMER0_DISSABLE(); TCCR0B |= 5  /* Select prescaler 1/1024        */
#define TIMER0_EXT_FE()       TIMER0_DISSABLE(); TCCR0B |= 6  /* External T0 falling edge       */
#define TIMER0_EXT_RE()       TIMER0_DISSABLE(); TCCR0B |= 7  /* External T0 rising edge        */
#define TIMER0_OVF_IE()       TIMSK0 |= (1<<TOIE0)            /* Enable overflow interrupt      */
#define TIMER0_OVF_ID()       TIMSK0 &= ~(1<<TOIE0)           /* Disable overflow interrupt     */
#define TIMER0_RELOAD(x)      TCNT0 = x                       /* Reload timer register          */

#define TIMER1_DISSABLE()     TCCR1B &= 0xF8                  /* Disable timer                  */
#define TIMER1_PRES_1()       TIMER1_DISSABLE(); TCCR1B |= 1  /* Select prescaler 1/1           */
#define TIMER1_PRES_8()       TIMER1_DISSABLE(); TCCR1B |= 2  /* Select prescaler 1/8           */
#define TIMER1_PRES_64()      TIMER1_DISSABLE(); TCCR1B |= 3  /* Select prescaler 1/64          */
#define TIMER1_PRES_256()     TIMER1_DISSABLE(); TCCR1B |= 4  /* Select prescaler 1/256         */
#define TIMER1_PRES_1024()    TIMER1_DISSABLE(); TCCR1B |= 5  /* Select prescaler 1/1024        */
#define TIMER1_EXT_FE()       TIMER1_DISSABLE(); TCCR1B |= 6  /* External T0 falling edge       */
#define TIMER1_EXT_RE()       TIMER1_DISSABLE(); TCCR1B |= 7  /* External T0 rising edge        */
#define TIMER1_OVF_IE()       TIMSK1 |= (1<<TOIE1)            /* Enable overflow interrupt      */
#define TIMER1_OVF_ID()       TIMSK1 &= ~(1<<TOIE1)           /* Disable overflow interrupt     */
#define TIMER1_RELOAD(x)      TCNT1H = (uint8_t) ((uint16_t)x>>8); TCNT1L = (uint8_t)((uint16_t)x & 0xff)  /* Reload timer register          */


/**
 * Arduino specifics.
 * -----------------------------------------------------------------*/
#ifdef ARDUINO_MEGA
#define ARD_LED_INIT()             DDRB  |= (1<<PB7)
#define ARD_LED_ON()               PORTB |= (1<<PB7)
#define ARD_LED_OFF()              PORTB &= ~(1<<PB7)
#define ARD_LED_TOGGLE()           PORTB ^= (1<<PB7)
#define ARD_IS_LED_ON()            (PINB && (1<<PB7))
#endif

#ifdef ARDUINO
#define ARD_LED_INIT()             DDRB  |= (1<<PB5)
#define ARD_LED_ON()               PORTB |= (1<<PB5)
#define ARD_LED_OFF()              PORTB &= ~(1<<PB5)
#define ARD_LED_TOGGLE()           PORTB ^= (1<<PB5)
#define ARD_IS_LED_ON()            (PINB && (1<<PB5))
#endif
/**
 * Analog
 *-----------------------------------
 * Analog in 0  -> PF0 PIN 97
 * Analog in 1  -> PF1 PIN 96
 * Analog in 2  -> PF2 PIN 95
 * Analog in 3  -> PF3 PIN 94
 * Analog in 4  -> PF4 PIN 93
 * Analog in 5  -> PF5 PIN 92
 * Analog in 6  -> PF6 PIN 91
 * Analog in 7  -> PF7 PIN 90
 * Analog in 8  -> PK0 PIN 89
 * Analog in 9  -> PK1 PIN 88
 * Analog in 10 -> PK2 PIN 87
 * Analog in 11 -> PK3 PIN 86
 * Analog in 12 -> PK4 PIN 85
 * Analog in 13 -> PK5 PIN 84
 * Analog in 14 -> PK6 PIN 83
 * Analog in 15 -> PK7 PIN 82
 *
 * PWM
 *-----------------------------------
 * PWM 0        -> PE0 PIN 2  (RX0)
 * PWM 1        -> PE1 PIN 3  (TX0)
 * PWM 2        -> PE4 PIN 6
 * PWM 3        -> PE5 PIN 7
 * PWM 4        -> PG5 PIN 1
 * PWM 5        -> PE3 PIN 5
 * PWM 6        -> PH3 PIN 15
 * PWM 7        -> PH4 PIN 16
 * PWM 8        -> PH5 PIN 17
 * PWM 9        -> PH6 PIN 18
 * PWM 10       -> PB4 PIN 23
 * PWM 11       -> PB5 PIN 24
 * PWM 12       -> PB6 PIN 25
 * PWM 13       -> PH7 PIN 26
 *
 * Communication.
 *-----------------------------------
 * RX0          -> PE0 PIN 2
 * TX0          -> PE1 PIN 3
 * RX1          -> PD2 PIN 45
 * TX1          -> PD3 PIN 46
 * RX2          -> PH0 PIN 12
 * TX2          -> PH1 PIN 13
 * RX3          -> PJ0 PIN 63
 * TX3          -> PJ1 PIN 64
 * SDA          -> PD1 PIN 44
 * SCL          -> PD0 PIN 43
 *
 *
 * Digital
 *-----------------------------------
 * D22          -> PA0 PIN 78
 * D23          -> PA1 PIN 77
 * D24          -> PA2 PIN 76
 * D25          -> PA3 PIN 75
 * D26          -> PA4 PIN 74
 * D27          -> PA5 PIN 73
 * D28          -> PA6 PIN 72
 * D29          -> PA7 PIN 71
 * D30          -> PC7 PIN 60
 * D31          -> PC6 PIN 59
 * D32          -> PC5 PIN 58
 * D33          -> PC4 PIN 57
 * D34          -> PC3 PIN 56
 * D35          -> PC2 PIN 55
 * D36          -> PC1 PIN 54
 * D37          -> PC0 PIN 53
 * D38          -> PD7 PIN 50
 * D39          -> PG2 PIN 70
 * D40          -> PG1 PIN 52
 * D41          -> PG0 PIN 51
 * D42          -> PL7 PIN 42
 * D43          -> PL6 PIN 41
 * D44          -> PL5 PIN 40
 * D45          -> PL4 PIN 39
 * D46          -> PL3 PIN 38
 * D47          -> PL2 PIN 37
 * D48          -> PL1 PIN 36
 * D49          -> PL0 PIN 35
 * D50          -> PB3 PIN 22 (MISO)
 * D51          -> PB2 PIN 21 (MOSI)
 * D52          -> PB1 PIN 20 (SCK)
 * D53          -> PB0 PIN 19
 *
 */



#endif	/* DEF_H */
