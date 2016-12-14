/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   A simple Command Line Interface for LEF.
 *
 * @file    LEF_Cli.c
 * @author  Peter Malmberg <peter.malmberg@gmail.com>
 * @date    2016-12-09
 * @lisence GPLv2
 *
 *---------------------------------------------------------------------------
 *
 * LEF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  Full license text is available on the following
 *
 * Source repository:
 * https://github.com/zonbrisad/LEF
 *
 * 1 tab = 2 spaces
 */

// Includes ---------------------------------------------------------------
#include <stdio.h>
#include <avr/pgmspace.h>

#include "LEF.h"
#include "LEF_Cli.h"
#include "def.h"


// Macros -----------------------------------------------------------------



// Variables --------------------------------------------------------------


char cliBuf[CLIBUF];
uint8_t cliCnt;
uint8_t cliLock;

LEF_CliCmd *Cmds;

// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------

void LEF_CliInit(LEF_CliCmd *cmds) {
  cliCnt  = 0;
  cliLock = 0;
  Cmds = cmds;
  printf("\n%s",CLI_PROMPT);
}

void LEF_CliPutChar(char ch) {
	LEF_queue_element qe;

	if (!cliLock) {
		cliBuf[cliCnt] = ch;
		cliCnt++;

		//putc(ch);
		printf("%c",ch);
		qe.id = 250;
		if (ch=='\n') {
			cliLock = 1;
			LEF_QueueSend(&StdQueue, &qe);
		}
	}
}

char cBuf[32];
uint8_t bIdx;


void printCommands(const LEF_CliCmd *cmdTable);
void printCommands(const LEF_CliCmd *cmdTable) {
  int i;
  handler ptr;
  i = 0;
  ptr = (handler)pgm_read_word(&cmdTable[i].function);
  while (ptr!=NULL) {
	ptr();
    strcpy_P(cBuf,&cmdTable[i].name);
    printf_P(PSTR("%-12s"), cBuf);
    printf_P(&cmdTable[i].desc);
    printf_P(PSTR("\n"));
    i++;
    ptr = (handler)pgm_read_word(&cmdTable[i].function);
  }
}

void LEF_CliExec(void) {
	DEBUGPRINT("Exec\n");
	printCommands(Cmds);
	cliLock=0;
	printf(CLI_PROMPT);
}
