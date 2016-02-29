/**
 * --------------------------------------------------------------------------
 * @file  sev_timer.c
 * @brief Software timer library.
 *
 *
 * @author Peter Malmberg <peter.malmberg@gmail.com>
 * @license LGPL
 * @date
 * --------------------------------------------------------------------------
 *
 */



#include <stdio.h>
#include <string.h>
#include <util/atomic.h>



#include "queue.h"
#include "sev_timer.h"






/**
 * Prototypes
 * --------------------------------------------------------------------------
 */


/**
 * Variables
 * --------------------------------------------------------------------------
 */



/**
 * Code
 * --------------------------------------------------------------------------
 */



void sev_timer_init(sev_timer *timer) {
  timer->counter = 0;
  timer->ticks   = 0;
}

void sev_timer_update(sev_timer *timer, event_queue *queue, eventT event) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
  if (timer->counter>0) {
    timer->counter--;
    if (timer->counter==0) {
      queue_send_event(queue, event, NULL);
      if (timer->ticks>0)
        timer->counter = timer->ticks;
    }
  }
  }
}


void sev_timer_start_repeat(sev_timer *timer, uint16_t ticks) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    timer->ticks   = ticks;
    timer->counter = ticks;
  }
}
void sev_timer_start_single(sev_timer *timer, uint16_t ticks) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
      timer->ticks   = 0;
      timer->counter = ticks;
    }
}
void sev_timer_stop(sev_timer *timer, uint16_t ticks) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
        timer->ticks   = 0;
        timer->counter = 0;
      }
}






