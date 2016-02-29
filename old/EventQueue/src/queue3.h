/**
 * @file  queue.h
 * @brief Abstract datatype representing an event queue
 *
 *
 * @author Peter Malmberg <peter.malmberg@gmail.com>
 * @date 
 *
 *
 */



#ifndef QUEUE_H
#define QUEUE_H


/**
 * Defs.
 * -----------------------------------------------------------------------
 */

/**
 * Queue size Max 250 is allowed. Above numbers are reserved.
 */
#define QUEUE_SIZE              10

#define QUEUE_OK                0
#define QUEUE_FULL              1

#define BUTTON_NO_EVENT         0
#define BUTTON_PRESS_EVENT      1
#define BUTTON_RELEASE_EVENT    2
#define BUTTON_LONG_PRESS_EVENT 3

// nr of ticks that counts as a longpress event
#define BUTTON_LONG_PRESS_TIMEOUTE_TICK  200

typedef struct {
  uint8_t source;
  uint8_t func;
} queue_element;

typedef struct {
  queue_element que[QUEUE_SIZE];            // queue it self
  uint8_t head;
  volatile uint8_t cnt;                     // nr of elements in queue (i.e. not the size if the queue)
} event_queue;


typedef struct {
  volatile uint8_t *port;
  uint8_t bit;
  uint8_t state;
} button;


/**
 * API
 * -----------------------------------------------------------------------
 */


void queue_clear(event_queue *queue);

void queue_init(event_queue *queue);

void queue_send(event_queue *queue,  queue_element *qe);

void queue_wait(event_queue *queue, queue_element *qe);

/**
 * Returns the number of events in the queue.
 */
uint8_t queue_cnt(event_queue *queue);




void button_init(button *b, volatile uint8_t *port);

uint8_t button_update(button *b);




/**
 * -----------------------------------------------------------------------
 */

#endif
