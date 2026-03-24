/**
 *---------------------------------------------------------------------------
 * @brief    Test of runing LEF in linux
 *
 * @file     main.c
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2026-03-22
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */

// Include ------------------------------------------------------------------

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "LEF_linux.h"
#include "LEF_Types.h"
#include "LEF.h"
// Macros -------------------------------------------------------------------


// Prototypes ---------------------------------------------------------------


// Datatypes ----------------------------------------------------------------
typedef enum { 
    EVENT_Print1, 
    EVENT_Timer1, 
    EVENT_Timer2, 
    EVENT_Callback,
    EVENT_Print4, 
    EVENT_Counter
} Events;

// Variables ----------------------------------------------------------------

LEF_Timer timer_1;
LEF_Timer timer_2;

// Code ---------------------------------------------------------------------

static void my_callback() {
    LEF_Timer_update(&timer_1);
    LEF_Timer_update(&timer_2);
}

int main(void) {
    LEF_Event event;

    LEF_init();
    LEF_Timer_init(&timer_1, EVENT_Timer1);
    LEF_Timer_init(&timer_2, EVENT_Timer2);
    LEF_Timer_start_repeat(&timer_1, 100);
    LEF_Timer_start_repeat(&timer_2, 500);
    
    event_init();
    LEF_add_systimer("Systick", 10, my_callback);
    
    while (true) {
        LEF_Wait(&event);
        // printf("Event id: %d\n", event.id);
        switch (event.id) {
            case EVENT_Timer1: printf("Timer 1\n"); break;
            case EVENT_Timer2: printf("Timer 2\n"); break;
        }
    }
    
    event_close();
    return 0;
}
