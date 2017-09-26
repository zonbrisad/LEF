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

#include "LEF.h"
#include "LEF_Cli.h"

#define DEBUGALL
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
  defprintf("\n%s",CLI_PROMPT);
}

void LEF_CliPutChar(char ch) {
	LEF_queue_element qe;

	if (!cliLock) {
		cliBuf[cliCnt] = ch;
		cliCnt++;

		//putc(ch);
		defprintf("%c",ch);
		qe.id = 250;
		if (ch=='\n') {
			cliLock = 1;
			LEF_QueueStdSend(&qe);
		}
	}
}



void LEF_CliPrintCommands(const LEF_CliCmd *cmdTable) {
  int i;
  handler ptr;
  i = 0;
	char cBuf[32];
	uint8_t bIdx;

	DEBUGPRINT("A\n");
  ptr = (handler)pgm_read_word(&cmdTable[i].function);
  while (ptr!=NULL) {
		DEBUGPRINT("X\n");
	  //ptr();
    defstrcpy(cBuf,&cmdTable[i].name);
    defprintf("%-12s", cBuf);
    printf(&cmdTable[i].desc);
    defprintf("\n");
    i++;
    ptr = (handler)pgm_read_word(&cmdTable[i].function);
  }
}

void LEF_CliExec(void) {
	DEBUGPRINT("Exec\n");
	LEF_CliPrintCommands(Cmds);
	cliLock=0;
	printf(CLI_PROMPT);
}
