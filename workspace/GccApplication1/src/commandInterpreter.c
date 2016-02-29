

#include "stdio.h"
#include "string.h"
#include "avr/pgmspace.h"
#include "uart/uart.h"

#include "commandInterpreter.h"

const char cBuf[32];


void printCommands(const commandStruct *cmdTable) {
  int i;
  handler ptr;

  i = 0;
  ptr = (handler)pgm_read_word(&cmdTable[i].function);
  while (ptr!=NULL) {
    strcpy_P(cBuf,&cmdTable[i].name);
    printf_P(PSTR("%-12s"), cBuf);
    printf_P(&cmdTable[i].desc);  
    printf_P(PSTR("\n"));
    i++;
    ptr = (handler)pgm_read_word(&cmdTable[i].function);
  }
}


char getChar() {
	int ch;
	do {
		ch = uart_getc();
	} while (ch==UART_NO_DATA);
	return ch;
}

void commandInterpreter(const commandStruct *cmdTable) {
  int ch;
  char cBuf[16];
  int bIdx;
  int i;

  handler ptr;
  while (1) {

    while (1) {   
      printf_P(PSTR("->"));                 // print the command prompt

      bIdx = 0;
      do {                                  // read a command from serial port
        ch = getChar();
        uart_putc(ch);
        cBuf[bIdx] = (char)ch;
        bIdx++;
      } while ((ch!=0x0d) && (ch!=0x0a));   // check for newline and carriage return
      
      bIdx--;
      cBuf[bIdx] = '\0';                // null terminate string
      
      //printf_P(PSTR("Entered Command = %s\n"), cBuf);
      
      uart_putc('\n');
     
      if (!strcmp_P(cBuf, PSTR("help"))) {
        printCommands(cmdTable);
      }

      i = 0;
      //cmdId = pgm_read_word(&cmdTable[i].id);                      // identify command
      ptr = (handler)pgm_read_word(&cmdTable[i].function);
      while (ptr!=NULL) {
        if (!strcmp_P(cBuf, &cmdTable[i].name)) {                  // command found,break search
          ptr = (handler)pgm_read_word(&cmdTable[i].function);
          if (ptr!=NULL) {
            //printf_P(PSTR("Function found\n"));
            ptr();
          }
          break;  
        } else {
          i++;
          //cmdId = pgm_read_word(&cmdTable[i].id);     
          ptr = (handler)pgm_read_word(&cmdTable[i].function);
        }
      }
    }
  }
}

