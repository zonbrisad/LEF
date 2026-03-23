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

// Macros -------------------------------------------------------------------


// Prototypes ---------------------------------------------------------------


// Datatypes ----------------------------------------------------------------
typedef enum { 
    EVENT_Print1, 
    EVENT_Print4, 
    EVENT_Counter
} Events;

// Variables ----------------------------------------------------------------


// Code ---------------------------------------------------------------------

int main(int argc, char *argv[]) {
    uint64_t counter = 0;

    event_init();
    event_add_timer(EVENT_Counter, "Timer count 10ms", 30);
    event_add_timer(EVENT_Print1, "Timer print 1s", 1000);
    event_add_timer(EVENT_Print4, "Timer print 4s", 4000);

    while (true) {
        int ev = event_wait();
        // printf("E %d\n", ev);
        switch (ev) {
            case EVENT_Counter:
                counter++;
                break;
            case EVENT_Print1:
                printf("Counter val: %lu\n", counter);
                break;
            case EVENT_Print4:
                printf("Print 4\n");
                break;
            default:
                printf("Def: %d\n", ev);
        }
    }

    event_close();
    return 0;
}
