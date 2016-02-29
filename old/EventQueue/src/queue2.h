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


/*--------------------------------------------------------------------------*/

/**
 * Queue size Max 250 is allowed. Above numbers are reserved.
 */
#define QUEUE_SIZE      10

#define QUEUE_OK        0
#define QUEUE_FULL      1


typedef struct {
  uint8_t source;
  uint8_t func;
} queue_element;

typedef struct {
  queue_element que[QUEUE_SIZE];  // queue it self
  uint8_t head;
  uint8_t tail;
  volatile uint8_t cnt;                    // nr of elements in queue (i.e. not the size if the queue)
} event_queue;



/*--------------------------------------------------------------------------*/


void queue_clear();

void queue_init();

void queue_send(queue_element *qe);

void queue_wait(queue_element *qe);

/**
 * Returns the number of events in the queue.
 */
uint8_t queue_cnt();



/*--------------------------------------------------------------------------*/

#endif
