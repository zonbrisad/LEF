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
 * 
 */

// Includes ---------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "LEF.h"
#include "LEF_Cli.h"
#include "LEF_Config.h"

//#define DEBUGALL
//#include "def.h"

// Macros -----------------------------------------------------------------


// Variables --------------------------------------------------------------

char cliBuf[LEF_CLI_BUF_LENGTH];
uint8_t cli_cnt;
uint8_t cli_lock;
uint8_t lef_cmds_length;
volatile uint8_t cli_wait_key_pressed;

const LEF_CliCmd *lef_cmds; //*Cmds;

// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------
#define NO_DATA     0x0100
#define ARROW_UP    0x1000
#define ARROW_DOWN  0x1001
#define ARROW_LEFT  0x1002
#define ARROW_RIGHT 0x1003

/**
 * Filter ANSI escape sequences from input characters. This function maintains an internal state to detect and filter out ANSI sequences, while allowing normal characters to pass through. It also translates arrow key sequences into specific codes.
 */
uint16_t ANSI_Filter(const char ch);
uint16_t ANSI_Filter(const char ch) {
	static uint8_t pos = 0;

	switch (pos) {
		case 0:
			if (ch == 0x1b) { // ESC
				pos = 1;
				return NO_DATA; // Filter out ESC
			}
			return ch; // Normal character
		case 1:
			if (ch == '[') {
				pos = 2;
				return NO_DATA; // Filter out [
			} else {
				pos = 0; // Not an ANSI sequence, reset state
				return ch; // Return the character that was after ESC
			}
		default:
			if (isalpha(ch)) {
				pos = 0; // Reset state after processing ANSI sequence
				switch (toupper(ch)) {
					case 'A': return ARROW_UP;
					case 'B': return ARROW_DOWN;
					//case 'C': return ARROW_RIGHT;
					//case 'D': return ARROW_LEFT;
					default: return NO_DATA; // Filter out the final character of ANSI sequence
				}
			}
			if (pos > 5)
				pos = 0; // Reset state if sequence is too long (should not happen)
				// return 0; // Filter out characters in ANSI sequence
			else
				pos++; // Move to next character in ANSI sequence				
	}
	return ch; // Return character just in case
}


void LEF_Cli_WaitKeyPressed(void) {
	cli_wait_key_pressed = 1;
	printf("Waiting for key press...\n");
}

void LEF_Cli_init(const LEF_CliCmd *cmds, uint8_t size) {
  cli_cnt  = 0;
  cli_lock = 0;
  lef_cmds = cmds;
  cli_wait_key_pressed = 0;

	lef_cmds_length = size;
	printf("\n%s", LEF_CLI_PROMPT);
}


void LEF_Cli_putc(const char ch) {
	LEF_Event event;
	uint16_t chf = ANSI_Filter(ch);

	if (cli_wait_key_pressed) {
		event.id = LEF_EVENT_CLI;
		event.func = 1;
		LEF_QueueStdSend(&event);
		//cli_wait_key_pressed = 0;
		return;
	}
	
	if (chf == NO_DATA) {
		return; // Filtered out character, do nothing
	}

	if (chf >= ARROW_UP && chf <= ARROW_RIGHT) {
		// Handle arrow keys if needed
		// For now, just ignore them
		return;
	}

	// handle backspace
	if (chf=='\b') {
//		LDEBUGPRINT("cliCnt = %d\n", cliCnt);
		if (cli_cnt > 0 ) {
		  cli_cnt--;
			printf("\b \b");
			return;
		}
		return;
	}

	// handle newline(enter)
	if (chf=='\n') {
//			cliLock = 1;
	//	cliCnt=0;
		printf("\n");

		event.id = LEF_EVENT_CLI;
		event.func = 0;
		
		LEF_QueueStdSend(&event);
				
		return;
	}

	if (cli_cnt >= LEF_CLI_BUF_LENGTH)
		return;

//	if (!cliLock) {
	cliBuf[cli_cnt] = chf;
	cli_cnt++;
	printf("%c",chf);
}


void LEF_Cli_print(void) {
	char cBuf[LEF_CLI_BUF_LENGTH];
	char dBuf[LEF_CLI_BUF_LENGTH];
	cmd_handler ptr;

  	for(int i=0; i<lef_cmds_length; i++) {

//		lefstrcpy(cBuf, cmdTable[i].name);
//		lefstrcpy(dBuf, cmdTable[i].desc);
//		ptr = (handler)pgm_read_word(&cmdTable[i].function);

		lefstrcpy(cBuf, lef_cmds[i].name);
		lefstrcpy(dBuf, lef_cmds[i].desc);

		ptr = (cmd_handler)pgm_read_word(&lef_cmds[i].function);
		if (ptr != NULL)
			lefprintf("  %-12s", cBuf);
	
	//		
	// Change in future?
	// use lefprintf("%s", dBuf) instead of lefprintf(dBuf) to avoid format string vulnerability warning from compiler
	// 
	#pragma GCC diagnostic ignored "-Wformat-security"
    lefprintf(dBuf);
	#pragma GCC diagnostic pop
    
    lefprintf("\n");
  }
}

void LEF_Cli_exec(void) {
	cmd_handler ptr;
	char *args;
	char cmd[LEF_CLI_BUF_LENGTH];
	int i = 0;

	if (cli_wait_key_pressed) {
		cli_wait_key_pressed = 0;
		printf("Key pressed\n");
		return;
	}

	if (cli_cnt == 0) {
		lefprintf(LEF_CLI_PROMPT);
		return;
	}

	while ((cliBuf[i] != ' ') && (i<cli_cnt)) {
		i++;
	}

	cliBuf[cli_cnt] = '\0';
	cliBuf[i] = '\0';
	args=cliBuf;
	args +=  (cli_cnt > i) ? (i+1) : i;
//	LDEBUGPRINT("Command = %s    args = %s\n", cliBuf, args);

	i = 0;
  	while (i < lef_cmds_length) {
		
		lefstrcpy(cmd, lef_cmds[i].name);
		if ( !strncmp(cmd, cliBuf, LEF_CLI_BUF_LENGTH) ) {
//			LDEBUGPRINT("Found command: %s\n", cmd);
			ptr = (cmd_handler)pgm_read_word(&lef_cmds[i].function);
			ptr(args);
			goto cli_cleanup;
		}
		i++;
  }

	lefprintf("%s: Command not found\n", cliBuf);

cli_cleanup:
	
	cli_lock = 0;
	cli_cnt = 0;
	lefprintf(LEF_CLI_PROMPT);
}
