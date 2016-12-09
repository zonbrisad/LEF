/**
 * @file  commanInterpreter.h
 * @brief A simple command interpreter for Atmel AVR MCU
 *
 *
 * @author Peter Malmberg
 * @date 
 *
 *
 */

/*--------------------------------------------------------------------------*/
/* Example 

const PROGMEM commandStruct cmdTable[] = {
  {testDisplay,       "lcdtest",      "Test lcd"},
  {printInfo,         "info",         "Print basic info"},
  {printTemperature,  "temperature",  "Read temperature cont."},
  {Moist,             "moist",        "Read moist cont."},
  {TimerPotTest,      "timer",        "Test timer pot"},
  {NULL,              NULL,            NULL}
};
*/






#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include "queue.h"


/**
 * Defs
 * --------------------------------------------------------------------------
 */


#define CMD_PROMPT       "->"

typedef void (*handler)(void);

typedef struct {
  handler function;
  char name[12];
  char desc[32];
  
} commandStruct;




/**
 * API
 * --------------------------------------------------------------------------
 */



void commandInterpreter(const commandStruct *cmdTable);


void sev_cmd_init(commandStruct *cmdTable);
void sev_cmd_update(event_queue *queue, LEF_EventId event);
void sev_cmd_exec(LEF_EventId event);

/*--------------------------------------------------------------------------*/

#endif
