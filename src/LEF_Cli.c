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
#include "def.h"


// Macros -----------------------------------------------------------------

#define CLIBUF 32

#define CLI_PROMPT ">"

// Variables --------------------------------------------------------------


char cliBuf[CLIBUF];
uint8_t cliCnt;
uint8_t cliLock;


// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------

void LEF_CliInit(void) {
  cliCnt  = 0;
  cliLock = 0;
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

void LEF_CliExec(void) {
	DEBUGPRINT("Exec\n");

	cliLock=0;
	printf(CLI_PROMPT);
}
