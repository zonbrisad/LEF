/**
 * @file  sev_timer.h
 * @brief Software timer library.
 *
 *
 * @author Peter Malmberg <peter.malmberg@gmail.com>
 * @license LGPL
 * @date 
 *
 *---------------------------------------------------------------------------
 */



#ifndef SEV_TIMER_H
#define SEV_TIMER_H


/**
 * Defs
 * --------------------------------------------------------------------------
 */

typedef struct {
  uint16_t counter;
  uint16_t ticks;
} LEF_Timer;





/**
 * API
 * --------------------------------------------------------------------------
 */

/**
 * Initiate timer struct.
 *
 * @param timer  timer data struct to be initiated
 */
void sev_timer_init(LEF_Timer *timer);

void LEF_TimerStartRepeat(LEF_Timer *timer, uint16_t ticks);
void sev_timer_start_single(LEF_Timer *timer, uint16_t ticks);
void LEF_TimerStop(LEF_Timer *timer, uint16_t ticks);
void sev_timer_update(LEF_Timer *timer, event_queue *queue, LEF_EventId event);





/**
 * --------------------------------------------------------------------------
 */

#endif
