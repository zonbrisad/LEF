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
 * @license GPLv2
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

//#undef defstrcpu
//#define defstrcpy_P
//#endif

//#define defstrcpy       strcpy

// Variables --------------------------------------------------------------

char cliBuf[LEF_CLI_BUF_LENGTH];
uint8_t cliCnt;
uint8_t cliLock;
uint8_t lef_cmds_length;

LEF_CliCmd *Cmds;

// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------

void LEF_CliInit(const LEF_CliCmd *cmds, uint8_t size) {
  cliCnt  = 0;
  cliLock = 0;
  Cmds = cmds;

	lef_cmds_length = size;
	printf("\n%s", LEF_CLI_PROMPT);
}

void LEF_CliPutChar(char ch) {
	LEF_queue_element event;

	if (ch=='\n') {
//			cliLock = 1;
	//	cliCnt=0;
		printf("\n");

		event.id = LEF_EVENT_CLI;
		event.func = 0;
		
		LEF_QueueStdSend(&event);
				
		return;
	}

	if (cliCnt >= LEF_CLI_BUF_LENGTH)
		return;

//	if (!cliLock) {
	cliBuf[cliCnt] = ch;
	cliCnt++;
	printf("%c",ch);

}


void LEF_CliPrintCommands(const LEF_CliCmd *cmdTable) {
  int i;
	char cBuf[LEF_CLI_BUF_LENGTH];

	i = 0;
  while (i < lef_cmds_length) {
		strcpy_P(cBuf, &cmdTable[i].name);
		printf("%-12s", cBuf);
		strcpy_P(cBuf, &cmdTable[i].desc);
    printf(cBuf);
		printf("\n");
    i++;
  }
}

void LEF_CliExec(void) {
	handler ptr;
	char cmd[LEF_CLI_BUF_LENGTH];
	uint8_t bIdx;
	int i;

	if (cliCnt == 0) {
		printf(LEF_CLI_PROMPT);
		return;
	}

	i = 0;
	while ((cliBuf[i] != ' ') && (i<cliCnt)) {
		i++;
	}
	
	cliBuf[i] = '\0';
	DEBUGPRINT("Command = %s\n", cliBuf);

	i = 0;
  while (i < lef_cmds_length) {
		
		strcpy_P(cmd, &Cmds[i].name);
		if ( !strncmp(cmd, cliBuf, LEF_CLI_BUF_LENGTH) ) {
			DEBUGPRINT("Found command: %s\n", cmd);
			ptr = (handler)pgm_read_word(&Cmds[i].function);
			ptr();
			goto cli_cleanup;
		}
     i++;
  }

	printf("Command \"%s\" not found\n", cliBuf);

cli_cleanup:
	
	cliLock = 0;
	cliCnt = 0;
	printf(LEF_CLI_PROMPT);
}
