/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   LEF configuration file
 *
 * @file    LEF_config.h
 * @author  Your Name <your.name@yourdomain.org>
 * @date    2016-11-30
 * @license GPLv2
 *
 *---------------------------------------------------------------------------
 *
 * LEF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  Full license text is available on the following
 *
 * https://github.com/zonbrisad/LEF
 *
 * 1 tab = 2 spaces
 */

#ifndef LEF_CONFIG_H
#define LEF_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif




// StdQue settings --------------------------------------------------------

#define LEF_QUEUE_LENGTH 10  /**< Length of LEF std queue */

	
// Cli settings -----------------------------------------------------------

#define CLIBUF 32
#define CLI_PROMPT ">"

#define LEF_CLI_CMD_LENGTH  12
#define LEF_CLI_DESC_LENGTH 32


// Buzzer settings --------------------------------------------------------

#define LEF_BUZZER_BEEP_DURATION 40
#define LEF_BUZZER_SHORT_BEEP_DURATION 10

	

#ifdef __cplusplus
} //end brace for extern "C"
#endif
#endif

