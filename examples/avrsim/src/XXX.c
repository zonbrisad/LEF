/**
 * LEF - Lightweight Event Framework
 *
 * This file is part of LEF distribution
 *
 *---------------------------------------------------------------------------
 * @brief   A test module
 *
 * @file    XXX.c
 * @author  Peter Malmberg <peter.malmberg@gmail.com>
 * @date    2016-12-20
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


#include "LEF.h"
#include "XXX.h"

#include "def.h"

// Macros -----------------------------------------------------------------

// Variables --------------------------------------------------------------

// Prototypes -------------------------------------------------------------

// Code -------------------------------------------------------------------


//#define WEAK(a) __attribute__((weak, alias(a)))

void WEAKA("weakFunction")  weakTest(void);

void weakFunction(void) {
  printf("My weak default function\n");
}
