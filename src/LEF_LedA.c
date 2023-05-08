/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   LED controll library.
 *
 * @file    LEF_Led.c
 * @author  Peter Malmberg <peter.malmberg@gmail.com>
 * @date    2016-11-30
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

#include "LEF.h"
#include "LEF_LedA.h"

// Macros -----------------------------------------------------------------

// Variables --------------------------------------------------------------

// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------

void LEF_LedA_init(LEF_LedA *led, LEDA_STATES mode) {
	led->mode = LED_OFF;
	led->cnt = 0;
	LEF_LedA_set(led, mode);
}

uint8_t LEF_LedA_update(LEF_LedA *led) {
	uint8_t limit;
	limit = 0;
	switch (led->mode) {
		case LEDA_OFF: return 0; break;
		case LEDA_ON:  return 255; break;
		case LEDA_FAST_BLINK:
			limit = LEDA_FAST_BLINK_DURATION - LEDA_BLINK_DURATION;
		case LEDA_BLINK:
			limit += LEDA_BLINK_DURATION;
			led->cnt++;

			if (led->cnt>limit) {
				led->cnt = - limit;
			}

			if (led->cnt<0)
				return 255;
			else
				return 0;

			break;
		case LEDA_SINGLE_BLINK:
		  led->cnt++;
		  if (led->cnt>LEDA_SINGLE_BLINK)
			  led->mode = LEDA_OFF;
		  return 255;

	 case LEDA_SOFT:
//		led->cnt++;
		led->cnt += 3;
//	if (led->cnt>0) {
		if (led->cnt > 255)
			led->cnt=-255;
//		}
		if (led->cnt >=0)
			return led->cnt;
		else
			return -led->cnt;
		break;

				
		default: return 0;
	}
}

void LEF_LedA_set(LEF_LedA *led, LEDA_STATES state) {
	led->mode = state;
	switch (state) {
	 case LEDA_BLINK:
	 case LEDA_FAST_BLINK:
		led->cnt = - LEDA_BLINK_DURATION;
		break;
	 case LEDA_SINGLE_BLINK:
		led->cnt = 0;
		break;
	 default:break;
	}
}
