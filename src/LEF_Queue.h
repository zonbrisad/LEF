/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   Queue functions
 *
 * @file    LEF_Queue.h
 * @author  Peter Malmberg <peter.malmberg@gmail.com>
 * @date    2016-12-08
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
 * 
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------

#include "LEF_Types.h"
#include "LEF_Config.h"

// Macros -----------------------------------------------------------------
	
// Typedefs ---------------------------------------------------------------

typedef struct {
	LEF_Event que[LEF_QUEUE_LENGTH];            // queue it self
	uint8_t head;
	volatile uint8_t cnt;                     // nr of elements in queue (i.e. not the size if the queue)
	uint8_t length;
} LEF_EventQueue;

typedef enum {
	LEF_QUEUE_OK,
	LEF_QUEUE_FULL,
	LEF_QUEUE_EMPTY
} LEF_QueueStatus;

// Variables --------------------------------------------------------------

// Functions --------------------------------------------------------------


void LEF_QueueClear(LEF_EventQueue *queue);

void LEF_QueueInit(LEF_EventQueue *queue);

void LEF_QueueSend(LEF_EventQueue *queue,  LEF_Event *event);

void LEF_QueueSendEvent(LEF_EventQueue *queue, LEF_EventId ev, void *data);

/**
 * @deprecated Use LEF_QueueNextElement instead, which allows peeking at the next element without removing it from the queue.
 */
void LEF_QueueWait(LEF_EventQueue *queue, LEF_Event *event);

/**
 * Returns the number of events in the queue.
 */
uint16_t LEF_QueueCnt(LEF_EventQueue *queue);

LEF_QueueStatus LEF_QueuePush(LEF_EventQueue* queue, LEF_Event* event);

LEF_QueueStatus LEF_QueuePop(LEF_EventQueue* queue, LEF_Event* event);

#ifdef __cplusplus
} //end brace for extern "C"
#endif


