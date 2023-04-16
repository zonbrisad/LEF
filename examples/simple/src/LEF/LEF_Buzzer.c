/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   A basic buzzer driver.
 *
 * @file    LEF_Buzzer.c
 * @author  Peter Malmberg <peter.malmberg@gmail.com>
 * @date    2016-12-09
 * @licence GPLv2
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

// Includes ---------------------------------------------------------------

#include "LEF_Buzzer.h"
#include "LEF.h"

// Macros -----------------------------------------------------------------

// Variables --------------------------------------------------------------

// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------

//typedef LEF_Buzzer_


uint16_t buz_cnt;
LEF_Buzzer_mode buz_state;

void LEF_Buzzer_init() {
	buz_state = LEF_BUZZER_OFF;
  buz_cnt   = 0;
}

void LEF_Buzzer_set(LEF_Buzzer_mode state) {
	if (buz_state == state)
		return;
	
	buz_cnt = 0;
	buz_state = state;
}

bool LEF_Buzzer_update() {

	switch (buz_state) {
	 case LEF_BUZZER_OFF: buz_cnt = 0; return 0; break;
	 case LEF_BUZZER_ON: return 1; break;
	 case LEF_BUZZER_SHORT_BEEP:
		buz_cnt++;
		if (buz_cnt > LEF_BUZZER_SHORT_BEEP_DURATION) {
			buz_state = LEF_BUZZER_OFF;
		}
		return 1;
		break;
	 case LEF_BUZZER_BEEP:
		buz_cnt++;
		if (buz_cnt > LEF_BUZZER_BEEP_DURATION) {
			buz_state = LEF_BUZZER_OFF;
		}
		return 1;
		break;
	 default: break; //return 0;
		
	}
	return 0;
}
