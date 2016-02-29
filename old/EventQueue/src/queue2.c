
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "uart/uart.h"
#include "queue2.h"

event_queue *queue;

void queue_clear(event_queue *queue) {
  queue->head = 0;
  queue->tail = 255;
  queue->cnt  = 0;
}

void element_cpy(queue_element *dst, queue_element *src) {
  dst->source = src->source;
  dst->func   = src->func;
}

void queue_init(event_queue *queue) {
  queue_clear(queue);
}

uint8_t queue_ptr_inc(uint8_t ptr) {
  return ptr % QUEUE_SIZE;
}

void queue_send(event_queue *queue,  queue_element *qe) {

//
//  if (queue->tail==255)         // detect initial condition and set to zero
//    queue->tail=0;

  if (queue->cnt==QUEUE_SIZE)         // detect initial condition and set to zero
    return;

//  if (queue->tail==queue->head) // if queue is full return
//    return;

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    element_cpy(&queue->que[queue->head], qe);

    queue->cnt++;
    queue->head++;
    if (queue->head>=QUEUE_SIZE)
      queue->head = 0;
    }
}

void queue_wait(event_queue *queue, queue_element *qe) {

  while(queue->cnt==0) {               // wait until queue has an element
  }

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    element_cpy(qe, &queue->que[(queue->head+(QUEUE_SIZE-queue->cnt))%QUEUE_SIZE]);
    queue->cnt--;
  }

}

uint8_t queue_cnt(event_queue *queue) {
  return queue->cnt;
}


