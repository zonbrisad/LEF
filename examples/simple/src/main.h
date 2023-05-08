/**
 *---------------------------------------------------------------------------
 * @brief    A simple LEF test
 *
 * @file     main.h
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2023-03-31
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */
        
#ifndef _MAIN_H_
#define _MAIN_H_

// Include ------------------------------------------------------------------


// Macros -------------------------------------------------------------------

#define APP_NAME        "main"
#define APP_VERSION     "0.01"
#define APP_DESCRIPTION "A simple LEF test"
#define APP_LICENSE     ""
#define APP_AUTHOR      "Peter Malmberg"
#define APP_EMAIL       "<peter.malmberg@gmail.com>"
#define APP_ORG         "PMG"
#define APP_HOME        ""
#define APP_ICON        ""

// AVR Reset causes ---------------------------------------------------------
#define IS_POWER_ON_RESET()            (MCUSR & (1<<PORF))
#define IS_BROWN_OUT_RESET()           (MCUSR & (1<<BORF))
#define IS_WATCH_DOG_RESET()           (MCUSR & (1<<WDRF))
#define IS_JTAG_RESET_RESET()          (MCUSR & (1<<JTRF))
#define IS_EXTERNAL_RESET()            (MCUSR & (1<<EXTRF))
//#define CLEAR_RESETS()                 MCUSR  &= (~31)              // clearing all reset causes

// AVR Reset MCU with watchdog ----------------------------------------------
#define AVR_RESET()                     wdt_enable(WDTO_500MS); cli(); while(1) {}

// AVR ADC ------------------------------------------------------------------
/*
#define ADC_ENABLE()     ADCSRA |= (1<<ADEN)   // Enable continuous conversion
#define ADC_DISSABLE()   ADCSRA &= ~(1<<ADEN)  // Dissable continuous conversion
#define ADC_START()      ADCSRA |= (1<<ADSC)   // Start single conversion
#define ADC_IE()         ADCSRA |= (1<<ADIE)   // Enable ADC interrupt
#define ADC_ID()         ADCSRA &= ~(1<<ADIE)  // Enable ADC interrupt

#define ADC_AUTOTRIGGER_ENABLE() 	ADCSRA |= (1<<ADATE)  // ADC auto trigger enable


//#define ADC_PRESCALER_2()   ADCSRA = (ADCSRA & ~7) | 0
#define ADC_PRESCALER_2()   ADCSRA = (ADCSRA & ~7) | 0b001
#define ADC_PRESCALER_4()   ADCSRA = (ADCSRA & ~7) | 0b010 
#define ADC_PRESCALER_8()   ADCSRA = (ADCSRA & ~7) | 0b011 
#define ADC_PRESCALER_16()  ADCSRA = (ADCSRA & ~7) | 0b100 
#define ADC_PRESCALER_32()  ADCSRA = (ADCSRA & ~7) | 0b101 
#define ADC_PRESCALER_64()  ADCSRA = (ADCSRA & ~7) | 0b110 
#define ADC_PRESCALER_128() ADCSRA = (ADCSRA & ~7) | 0b111 
#define ADC_VALUE()         (ADCL + (ADCH << 8))

//#define ADC_TRG_FREE_RUNNING() ADCSRB = (ADCSRB & ~7) | 0
#define ADC_TRG_FREE_RUNNING() ADCSRB = (ADCSRB & 0b00000111) | 0b000
#define ADC_TRG_ANALOG_COMP()  ADCSRB = (ADCSRB & 0b00000111) | 0b001
#define ADC_TRG_EXTERNAL_INT() ADCSRB = (ADCSRB & 0b00000111) | 0b010
#define ADC_TRG_TIMER0_COMPA() ADCSRB = (ADCSRB & 0b00000111) | 0b011
#define ADC_TRG_TIMER0_OVF()   ADCSRB = (ADCSRB & 0b00000111) | 0b100
#define ADC_TRG_TIMER1_COMPB() ADCSRB = (ADCSRB & 0b00000111) | 0b101
#define ADC_TRG_TIMER1_OVF()   ADCSRB = (ADCSRB & 0b00000111) | 0b110
#define ADC_TRG_TIMER1_CPT()   ADCSRB = (ADCSRB & 0b00000111) | 0b111
*/

#define ADC_REF_AREF() ADMUX = (ADMUX & ~15)            // Set voltage reference to AREF
#define ADC_REF_AVCC() ADMUX = (ADMUX & ~15) | (1 << 6) // Set voltage reference to AVcc 
#define ADC_REF_INT() ADMUX = (ADMUX & ~15) | (3 << 6)  // Set voltage reference to 1.1 V internal reference

#define ADC_MUX(mux) ADMUX = (ADMUX & 0b11110000) | (mux) 

#define ADC_IS_BUSY() (ADCSRA & ADIF) ? 0 : 1

#define ADC_WAIT()  while (ADC_IS_BUSY()) {}  // Busy wait for completion

// AVR Timer 0 (8 bit) ------------------------------------------------------

// Clock source
#define TIMER0_CLK_DISSABLE()     TCCR0B &= 0xF8              // Disable timer
#define TIMER0_CLK_PRES_1()       TCCR0B |= 1                 // Select prescaler 1/1
#define TIMER0_CLK_PRES_8()       TCCR0B |= 2                 // Select prescaler 1/8
#define TIMER0_CLK_PRES_64()      TCCR0B |= 3                 // Select prescaler 1/64
#define TIMER0_CLK_PRES_256()     TCCR0B |= 4                 // Select prescaler 1/256
#define TIMER0_CLK_PRES_1024()    TCCR0B |= 5                 // Select prescaler 1/1024
#define TIMER0_CLK_EXT_FE()       TCCR0B |= 6                 // External T0 falling edge
#define TIMER0_CLK_EXT_RE()       TCCR0B |= 7                 // External T0 rising edge

// Interrupt control
#define TIMER0_OVF_IE()           TIMSK0 |= (1<<TOIE0)        // Enable overflow interrupt
#define TIMER0_OVF_ID()           TIMSK0 &= ~(1<<TOIE0)       // Disable overflow interrupt
#define TIMER0_OCA_IE()           TIMSK0 |= (1<<OCIE0A)       // Enable output compare A interrupt
#define TIMER0_OCA_ID()           TIMSK0 &= ~(1<<OCIE0A)      // Disable output compare A interrupt
#define TIMER0_OCB_IE()           TIMSK0 |= (1<<OCIE0B)       // Enable output compare B interrupt
#define TIMER0_OCB_ID()           TIMSK0 &= ~(1<<OCIE0B)      // Disable output compare B interrupt

#define TIMER0_OCA_SET(x)         OCR0A = x                   // Set output compare A register
#define TIMER0_OCB_SET(x)         OCR0B = x                   // Set output compare B register
#define TIMER0_RELOAD(x)          TCNT0 = x                   // Reload timer register

// Waveform generation mode
#define TIMER0_WGM_NORMAL()       TCCR0A = (TCCR0A & 0x11111100) | 0x00000000
#define TIMER0_WGM_PWM()          TCCR0A = (TCCR0A & 0x11111100) | 0x00000001 // PWM, phase correct
#define TIMER0_WGM_FAST_PWM()     TCCR0A = (TCCR0A & 0x11111100) | 0x00000011 // Fast PWM

// Output modes
#define TIMER0_OM_NORMAL() TCCR0A &= 0b00111111                        // OC0A disconnected
#define TIMER0_OM_TOGGLE() TCCR0A = (TCCR0A & 0b00111111) | 0b01000000 // Toggle OC0A on compare match
#define TIMER0_OM_CLEAR()  TCCR0A = (TCCR0A & 0b00111111) | 0b10000000 // Clear OC0A on compare match
#define TIMER0_OM_SET()    TCCR0A = (TCCR0A & 0b00111111) | 0b11000000 // Set OC0A on compare match





// AVR Timer 1 (16 bit) -----------------------------------------------------

// Clock source
#define TIMER1_CLK_NONE()         TCCR1B &= 0xF8              // Disable timer
#define TIMER1_CLK_PRES_1()       TCCR1B |= 1                 // Select prescaler 1/1            
#define TIMER1_CLK_PRES_8()       TCCR1B |= 2                 // Select prescaler 1/8           
#define TIMER1_CLK_PRES_64()      TCCR1B |= 3                 // Select prescaler 1/64          
#define TIMER1_CLK_PRES_256()     TCCR1B |= 4                 // Select prescaler 1/256         
#define TIMER1_CLK_PRES_1024()    TCCR1B |= 5                 // Select prescaler 1/1024        
#define TIMER1_CLK_EXT_FE()       TCCR1B |= 6                 // External T0 falling edge       
#define TIMER1_CLK_EXT_RE()       TCCR1B |= 7                 // External T0 rising edge        

// Interrupt control
#define TIMER1_OVF_IE()           TIMSK1 |= (1<<TOIE1)        // Enable overflow interrupt      
#define TIMER1_OVF_ID()           TIMSK1 &= ~(1<<TOIE1)       // Disable overflow interrupt     
#define TIMER1_OCA_IE()           TIMSK1 |= (1<<OCIE1A)       // Enable output compare A interrupt
#define TIMER1_OCA_ID()           TIMSK1 &= ~(1<<OCIE1A)      // Disable output compare A interrupt
#define TIMER1_OCB_IE()           TIMSK1 |= (1<<OCIE1B)       // Enable output compare B interrupt
#define TIMER1_OCB_ID()           TIMSK1 &= ~(1<<OCIE1B)      // Disable output compare B interrupt
                                                              
                                                              // Set output compare A register
#define TIMER1_OCA_SET(x)         OCR1AH = (uint8_t) ((uint16_t)x>>8); OCR1AL = (uint8_t) ((uint16_t)x & 0xff)
                                                              // Set output compare B register
#define TIMER1_OCB_SET(x)         OCR1BH = (uint8_t) ((uint16_t)x>>8); OCR1BL = (uint8_t) ((uint16_t)x & 0xff)
                                                              // Reload timer register
#define TIMER1_RELOAD(x)          TCNT1H = (uint8_t) ((uint16_t)x>>8); TCNT1L = (uint8_t)((uint16_t)x & 0xff)

// Datatypes ----------------------------------------------------------------


// Variables ----------------------------------------------------------------


// Prototypes ---------------------------------------------------------------


#endif // _MAIN_H_ 
