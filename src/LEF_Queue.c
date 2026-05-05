/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   Queue functions
 *
 * @file    LEF_Queue.c
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

// Includes ---------------------------------------------------------------
#include "LEF_Queue.h"
#include "LEF_Core.h"

// Macros -----------------------------------------------------------------

// Code -------------------------------------------------------------------

void LEF_QueueClear(LEF_EventQueue* queue) {
    queue->head = 0;
    queue->cnt = 0;
}

static void LEF_element_cpy(LEF_Event* dst, LEF_Event* src) {
    dst->id = src->id;
    dst->func = src->func;
}

LEF_EventQueue* LEF_QueueNew(uint8_t length) {
    LEF_EventQueue* queue;
    queue = (LEF_EventQueue*) malloc(sizeof(LEF_EventQueue) + sizeof(LEF_Event) * length);
    queue->length = length;
    LEF_QueueClear(queue);
    return queue;
}

LEF_QueueStatus LEF_QueuePush(LEF_EventQueue* queue, LEF_Event* event) {
    if (queue->cnt >= queue->length) { // detect initial condition and set to zero
        return LEF_QUEUE_FULL;
    }

    uint8_t tail = (queue->head + (queue->length - queue->cnt)) % queue->length;
    tail = (tail == 0) ? (queue->length - 1) : (tail - 1);
    LEF_element_cpy(&queue->que[tail], event);
    queue->cnt++;
    return LEF_QUEUE_OK;
}

LEF_QueueStatus LEF_QueuePop(LEF_EventQueue* queue, LEF_Event* event) {
    if (queue->cnt == 0)  {
        return LEF_QUEUE_EMPTY;
    }

    uint8_t head = (queue->head + (queue->length - queue->cnt)) % queue->length;
    LEF_element_cpy(event, &queue->que[head]);
    queue->cnt--;
    return LEF_QUEUE_OK;
}