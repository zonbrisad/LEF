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

// Variables --------------------------------------------------------------

// Prototypes -------------------------------------------------------------

// void LEF_element_cpy(LEF_Event* dst, LEF_Event* src);
// uint8_t queue_ptr_inc(uint8_t ptr);

// Code -------------------------------------------------------------------

void LEF_QueueClear(LEF_EventQueue* queue) {
    queue->head = 0;
    queue->cnt = 0;
}

static void LEF_element_cpy(LEF_Event* dst, LEF_Event* src) {
    dst->id = src->id;
    dst->func = src->func;
}

void LEF_QueueInit(LEF_EventQueue* queue) { 
    queue->length = LEF_QUEUE_LENGTH;
    LEF_QueueClear(queue); 
}

LEF_EventQueue* LEF_QueueNew(uint8_t length) {
    LEF_EventQueue* queue;
    queue = (LEF_EventQueue*)malloc(sizeof(LEF_EventQueue));
    // LEF_QueueInit(queue, length);
    return queue;
}

static uint8_t queue_ptr_inc(uint8_t ptr) { 
    return ptr % LEF_QUEUE_LENGTH; 
}

void LEF_QueueSend(LEF_EventQueue* queue, LEF_Event* event) {
    //
    //  if (queue->tail==255)         // detect initial condition and set to
    //  zero
    //    queue->tail=0;

    if (queue->cnt == queue->length)  // detect initial condition and set to zero
        return;

    //  if (queue->tail==queue->head) // if queue is full return
    //    return;

    // LEF_ATOMIC_BLOCK() {
    LEF_ATOMIC_BLOCK_START();
    LEF_element_cpy(&queue->que[queue->head], event);
    queue->cnt++;
    queue->head++;
    if (queue->head >= queue->length) 
        queue->head = 0;
    // }
    LEF_ATOMIC_BLOCK_END();
}

void LEF_QueueWait(LEF_EventQueue* queue, LEF_Event* event) {
    while (queue->cnt == 0) {  // wait until queue has an element
        LEF_portNOP;
    }

    // LEF_ATOMIC_BLOCK() {
    LEF_ATOMIC_BLOCK_START();
    LEF_element_cpy(event,
                    &queue->que[(queue->head + (queue->length - queue->cnt)) %
                                queue->length]);
    queue->cnt--;
    // }
    LEF_ATOMIC_BLOCK_END();
}

/**
 * @todo add support for extra data
 */
void LEF_QueueSendEvent(LEF_EventQueue* queue, LEF_EventId ev, void* data) {
    LEF_Event event;
    (void)data;
    event.id = ev;
    event.func = 0;
    LEF_QueueSend(queue, &event);
}

uint16_t LEF_QueueCnt(LEF_EventQueue* queue) { return queue->cnt; }

// void LEF_QueueNextElement(LEF_EventQueue* queue, LEF_Event* dst) {
//     if (queue->cnt == 0) return;

//     LEF_element_cpy(
//         dst, &queue->que[(queue->head + (queue->length - queue->cnt) + pos) %
//                          queue->length]);
// }

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