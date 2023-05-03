/**
 *---------------------------------------------------------------------------
 * @brief    A button class
 *
 * @file     LEF_Button.c
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

#include "LEF_Pot.h"


LEF_Pot *LEF_Pot_new(void) {
  return malloc(sizeof(LEF_Pot));
}

void LEF_Pot_init(LEF_Pot *pot, LEF_EventId id) {
	pot->id = id;
	pot->state = 0;
}

void LEF_Pot_update(LEF_Pot *pot, uint16_t newState) {
	int diff;
	LEF_queue_element qe;
	qe.id = pot->id;

	diff = pot->state - newState;
	

	if ((((diff) < 0) ? -(diff) : (diff)) > 2) {
		qe.func=1;
		LEF_QueueStdSend(&qe);
		pot->state = newState;
	}

}

uint16_t LEF_Pot_state(LEF_Pot *pot) {
  return pot->state;
}

void LEF_Pot_free(LEF_Pot *pot) {  
  free(pot);
}

