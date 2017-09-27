/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution.
 *
 *---------------------------------------------------------------------------
 * @brief   Main LEF file
 *
 * @file    LEF.h
 * @author  Your Name <your.name@yourdomain.org>
 * @date    2016-11-30
 * @license GPLv2
 *
 *---------------------------------------------------------------------------
 *
 * LEF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  Full license text is available on the following
 *
 * https://github.com/zonbrisad/LEF
 *
 * 1 tab = 2 spaces
 */

#ifndef LEF_H
#define LEF_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------	
#include <stdint.h>
	
// AVR specific includes
#ifdef __AVR__	
#include <util/atomic.h>
#include <avr/pgmspace.h>
#endif

#include "LEF_Config.h"

#include "LEF_Queue.h"
#include "LEF_Timer.h"
#include "LEF_Led.h"
#include "LEF_Buzzer.h"
#include "LEF_Cli.h"


// Controls --------------------------------------------------------------

#ifndef LEF_QUEUE_LENGTH
#error "LEF_QUEUE_LENGTH not defined."
#endif
	
// Macros -----------------------------------------------------------------

#define LEF_ATOMIC_BLOCK() ATOMIC_BLOCK(ATOMIC_FORCEON)
	
//#define LEF_ATOMIC_BLOCK()

	
// default lefprintf 
//#define lefprintf(...)  printf( __VA_ARGS__)
//
//#ifdef DEF_PLATFORM_AVR   // if avr GCC use printf_P to store format strings in flash instead of RAM
//#undef lefprintf
//#define lefprintf(fmt, ...)  printf_P(PSTR(fmt), ##__VA_ARGS__)
//#endif
//
	
// Critical Section -------------------------------------------------------

#define LEF_EnterCritical()  \
		asm volatile ( "in    __tmp_reg__, __SREG__" :: );  \
        asm volatile ( "cli" :: );                \
        asm volatile ( "push  __tmp_reg__" :: )

#define LEF_ExitCritical() \
		asm volatile ( "pop   __tmp_reg__" :: );        \
        asm volatile ( "out   __SREG__, __tmp_reg__" :: )

#define LEF_DisableInterrupts()  asm volatile ( "cli" :: );
#define LEF_EnableInterrupts()   asm volatile ( "sei" :: );

// Architectural specifics ------------------------------------------------
#define LEF_portNOP  asm volatile ( "nop" );
	
	
	
// Typedefs ---------------------------------------------------------------
	
// Variables --------------------------------------------------------------

	
// Functions --------------------------------------------------------------


void LEF_Init(void);

#ifdef __cplusplus
} //end brace for extern "C"
#endif
#endif

