/**
 * LEF - Lightweiht Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   Main LEF file
 *
 * @file    LEF.h
 * @author  Your Name <your.name@yourdomain.org>
 * @date    2016-11-30
 * @licence GPLv2
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

#ifndef LEF_H
#define LEF_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------	
#include <stdint.h>
#include <util/atomic.h>

#include "LEF_Config.h"

#include "LEF_Queue.h"
#include "LEF_Timer.h"
#include "LEF_Led.h"

// Controls --------------------------------------------------------------

#ifndef LEF_QUEUE_LENGTH
#error "LEF_QUEUE_LENGTH not defined."
#endif
	
// Macros -----------------------------------------------------------------

// Typedefs ---------------------------------------------------------------
	
// Variables --------------------------------------------------------------

	
// Functions --------------------------------------------------------------

#ifdef __cplusplus
} //end brace for extern "C"
#endif
#endif

