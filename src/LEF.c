/**
 * LEF - Lightweiht Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   Main LEF file
 *
 * @file    LEF.c
 * @author  Your Name <your.name@yourdomain.org>
 * @date    2016-11-30
 * @licence GPLv2
 *
 *---------------------------------------------------------------------------
 *
 * LEF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  Full license text is available on the following
 *
 * https://github.com/zonbrisad/LEF
 *
 * 
 */

// Includes ---------------------------------------------------------------

#include "LEF.h"

#ifdef LEF_SYSTEM_LINUX
#include "LEF_linux.h"
#endif

#include <stdio.h>

// Macros -----------------------------------------------------------------

// Variables --------------------------------------------------------------

LEF_EventQueue lef_std_queue;

// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------


// initiate LEF standard queue
void LEF_init(void) {
	LEF_QueueInit(&lef_std_queue);
}


void LEF_Send(LEF_Event *event) {
	return LEF_QueueSend(&lef_std_queue, event);
}

void LEF_Send_msg(LEF_EventId id, LEF_EventFunc func) {
	LEF_Event event;
	event.id = id;
	event.func = func;
	LEF_Send(&event);
}

void LEF_Clear(void) {
	return LEF_QueueClear(&lef_std_queue);
}

uint16_t LEF_Count(void) {
  return LEF_QueueCnt(&lef_std_queue);
}

void LEF_Print_event(LEF_Event *event) {
	printf("Event id: %3d   Event func: %3d\n", event->id, event->func);
}

#ifdef LEF_SYSTEM_LINUX

void LEF_Wait(LEF_Event* event) {
    while (true) {
        if (LEF_QueueCnt(&lef_std_queue) > 0) {
            LEF_QueueWait(&lef_std_queue, event);
            return;
        }
        event_wait();
    }
}

/**
 * @brief Add system(Linux) timer
 *
 * @param name Name of timer
 * @param invervall Timer intervall in ms
 * @param callback Callback function for timer
 */
void LEF_add_systimer(char* name, size_t intervall, LEF_Callback callback) {
    event_add_timer_callback(0, name, intervall, callback);
}

#else 

void LEF_Wait(LEF_Event *event) {
	return LEF_QueueWait(&StdQueue, event);
}

#endif