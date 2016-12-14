/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   A simple Command Line Interface for LEF.
 *
 * @file    LEF_Cli.h
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

#ifndef LEF_CLI_H
#define LEF_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------

// Macros -----------------------------------------------------------------

#define CLIBUF 32
#define CLI_PROMPT ">"

#define LEF_CLI_CMD_LENGTH  12
#define LEF_CLI_DESC_LENGTH 32

// Typedefs ---------------------------------------------------------------

typedef void (*handler)(void);

typedef struct {
  handler function;
  char name[LEF_CLI_CMD_LENGTH];
  char desc[LEF_CLI_DESC_LENGTH];

} LEF_CliCmd;

// Variables --------------------------------------------------------------

// Prototypes -------------------------------------------------------------

void LEF_CliInit(LEF_CliCmd *cmds);

void LEF_CliPutChar(char ch);

void LEF_CliExec(void);

#ifdef __cplusplus
} //end brace for extern "C"
#endif
#endif

