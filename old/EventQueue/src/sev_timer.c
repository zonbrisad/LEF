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



void sev_timer_init(LEF_Timer *timer) {
  timer->counter = 0;
  timer->ticks   = 0;
}

void sev_timer_update(LEF_Timer *timer, event_queue *queue, LEF_EventId event) {
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


void LEF_TimerStartRepeat(LEF_Timer *timer, uint16_t ticks) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    timer->ticks   = ticks;
    timer->counter = ticks;
  }
}
void sev_timer_start_single(LEF_Timer *timer, uint16_t ticks) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
      timer->ticks   = 0;
      timer->counter = ticks;
    }
}
void LEF_TimerStop(LEF_Timer *timer, uint16_t ticks) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
        timer->ticks   = 0;
        timer->counter = 0;
      }
}






