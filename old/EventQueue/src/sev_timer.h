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
} sev_timer;





/**
 * API
 * --------------------------------------------------------------------------
 */

/**
 * Initiate timer struct.
 *
 * @param timer  timer data struct to be initiated
 */
void sev_timer_init(sev_timer *timer);

void sev_timer_start_repeat(sev_timer *timer, uint16_t ticks);
void sev_timer_start_single(sev_timer *timer, uint16_t ticks);
void sev_timer_stop(sev_timer *timer, uint16_t ticks);
void sev_timer_update(sev_timer *timer, event_queue *queue, eventT event);





/**
 * --------------------------------------------------------------------------
 */

#endif
