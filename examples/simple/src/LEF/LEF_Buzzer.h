/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   A simple driver for an active buzzer.
 *
 * @file    LEF_Buzzer.h
 * @author  Peter Malmberg <peter.malmberg@gmail.com>
 * @date    2016-12-09
 * @license GPLv2
 *
 *---------------------------------------------------------------------------
 *
 * LEF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  Full license text is available on the following
 *
 * Source repository:
 * https://github.com/zonbrisad/LEF
 *
 * 1 tab = 2 spaces
 */

#ifndef LEF_BUZZER_H
#define LEF_BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------
#include "LEF.h"
	
// Macros -----------------------------------------------------------------

// Typedefs ---------------------------------------------------------------
typedef enum {
	LEF_BUZZER_OFF = 0,
	LEF_BUZZER_ON = 1,
	LEF_BUZZER_BEEP = 2,
	LEF_BUZZER_SHORT_BEEP = 3,
//	LEF_BUZZER_LONG_BEEP = 4,
} LEF_Buzzer_mode;
	
// Variables --------------------------------------------------------------

// Prototypes -------------------------------------------------------------

	void LEF_Buzzer_init();
	
	void LEF_Buzzer_set(LEF_Buzzer_mode state);

	bool LEF_Buzzer_update();
	
	
#ifdef __cplusplus
} //end brace for extern "C"
#endif
#endif

