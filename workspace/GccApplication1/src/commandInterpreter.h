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
  {printCommands,     "help",         "This help info"},
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


/**
 * typedefs
 * --------------------------------------------------------------------------
 */

typedef void (*handler)(void);

typedef struct {
  handler function;
  char name[12];
  char desc[32];
} commandStruct;


/**
 * functions
 * --------------------------------------------------------------------------
 */

void commandInterpreter(const commandStruct *cmdTable);

void printCommands(const commandStruct *cmdTable);



#endif
