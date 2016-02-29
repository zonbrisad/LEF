
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "uart/uart.h"
#include "queue.h"


void queue_clear(event_queue *queue) {
  queue->head = 0;
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

/**
 * @todo add support for extra data
 */
void queue_send_event(event_queue *queue, eventT ev, void *data) {
  queue_element qe;
  qe.source = ev;
  queue_send(queue, &qe);
}


uint8_t queue_cnt(event_queue *queue) {
  return queue->cnt;
}


void button_init(button *b, volatile uint8_t *port) {
  *port |= (1<<b->bit);
  b->state = 0;
}

uint8_t button_pressed(button *b) {
  if (b->state==2)
    return 1;

  return 0;
}

uint8_t button_update(button *b) {
  uint8_t p;

  p = *b->port & (1<<b->bit);       // read the button pin

  //printf("pin %x\n", p);
  if (!p) {                         // button is being pressed
    if (b->state<250)
      b->state++;
    if (b->state==2)
      return BUTTON_PRESS_EVENT;

    if (b->state==BUTTON_LONG_PRESS_TIMEOUTE_TICK)
      return BUTTON_LONG_PRESS_EVENT;
  } else {                          // button is being released

    //if  (b->state>1)

    if  (b->state==1) {
      b->state = 0;
      return BUTTON_RELEASE_EVENT;
    }
    if  (b->state>1)
      b->state = 1;

  }

  return BUTTON_NO_EVENT;
}

void button_que_update(event_queue *queue, button *b, uint8_t ev_base) {
  uint8_t ev;
  static queue_element qel;

  ev = button_update(b);
  qel.source = ev + ev_base;
  if (ev!=BUTTON_NO_EVENT)
    queue_send(queue, &qel);
}


