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
#include <string.h>

#include "LEF.h"
#include "LEF_Cli.h"

//#define DEBUGALL
#include "def.h"

// Macros -----------------------------------------------------------------


// Variables --------------------------------------------------------------

char cliBuf[LEF_CLI_BUF_LENGTH];
uint8_t cliCnt;
uint8_t cliLock;
uint8_t lef_cmds_length;

const LEF_CliCmd *Cmds;

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
	
	// handle backspace
	if (ch=='\b') {
		DEBUGPRINT("cliCnt = %d\n", cliCnt);
		if (cliCnt > 0 ) {
		  cliCnt--;
			printf("\b \b");
			return;
		}
		return;
	}

	// handle newline(enter)
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
		lefstrcpy(cBuf, cmdTable[i].name);		
		lefprintf("%-12s", cBuf);
		lefstrcpy(cBuf, cmdTable[i].desc);
    lefprintf(cBuf);
		lefprintf("\n");
    i++;
  }
}

void LEF_CliExec(void) {
	handler ptr;
	char *args;
	char cmd[LEF_CLI_BUF_LENGTH];
	int i;

	if (cliCnt == 0) {
		lefprintf(LEF_CLI_PROMPT);
		return;
	}

	i = 0;
	while ((cliBuf[i] != ' ') && (i<cliCnt)) {
		i++;
	}

	cliBuf[cliCnt] = '\0';
	cliBuf[i] = '\0';
	args=cliBuf;
	args +=  (cliCnt > i) ? (i+1) : i;
	DEBUGPRINT("Command = %s    args = %s\n", cliBuf, args);

	i = 0;
  while (i < lef_cmds_length) {
		
		lefstrcpy(cmd, Cmds[i].name);
		if ( !strncmp(cmd, cliBuf, LEF_CLI_BUF_LENGTH) ) {
			DEBUGPRINT("Found command: %s\n", cmd);
			ptr = (handler)pgm_read_word(&Cmds[i].function);
			ptr(args);
			goto cli_cleanup;
		}
		i++;
  }

	lefprintf("%s: Command not found\n", cliBuf);

cli_cleanup:
	
	cliLock = 0;
	cliCnt = 0;
	lefprintf(LEF_CLI_PROMPT);
}
