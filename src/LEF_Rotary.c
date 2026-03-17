/**
 *---------------------------------------------------------------------------
 * @brief    Rotary encoder (experimental)
 *
 * @file     LEF_Rotary.c
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2023-04-14
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */

#include <stdlib.h>

#include "LEF_Rotary.h"


void LEF_Rotary_init(LEF_Rotary *rotary, LEF_EventId id) {
	rotary->id = id;
	rotary->state = 0;
	rotary->cnt = 0;
	rotary->clk = 0xff;
	rotary->dt = 0xff;
}

void LEF_Rotary_update(LEF_Rotary *rotary, uint8_t clk, uint8_t data) {

	// store rotary state
	rotary->clk = (rotary->clk << 1);
	if (clk)
		rotary->clk |= 1;
	else
		rotary->clk &= ~1;

	// event.func = rotary->clk;
	if ((rotary->clk & 0x3) == 2) {
		LEF_Send_msg(rotary->id, (data) ? 0 : 1);
	}

}

void LEF_Rotary_update_alt(LEF_Rotary *rotary, uint8_t clk , uint8_t data);
void LEF_Rotary_update_alt(LEF_Rotary *rotary, uint8_t clk , uint8_t data) {

	// store rotary state
	rotary->clk = (rotary->clk << 1);
	if (clk)
		rotary->clk |= 1;
	else
		rotary->clk &= ~1;

	// event.func = rotary->clk;
	if ((rotary->clk & 0x3) == 2) {
    	LEF_Send_msg(rotary->id, (data) ? 0 : 1);
	}


}
