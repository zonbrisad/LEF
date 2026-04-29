/**
 *---------------------------------------------------------------------------
 * @brief    Demo of LEF running on Raspberry Pi Pico
 *
 * @file     main.c
 * @author   Peter Malmberg <peter.malmberg@gmail.com>
 * @version  0.01
 * @date     2026-04-24
 * @license  MIT
 *
 *---------------------------------------------------------------------------
 *
 *
 */

// Include ------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "main.h"
#include "LEF.h"
#include "LEF_Cli.h"
#include "LEF_Timer.h"
#include "LEF_Led.h"

// Macros -------------------------------------------------------------------


// Prototypes ---------------------------------------------------------------


// Datatypes ----------------------------------------------------------------

typedef enum {
    LEF_EVENT_TIMER1 = 1,
    LEF_EVENT_STDIO
} PICO_Events;

// Variables ----------------------------------------------------------------
LEF_Led led_board;
LEF_Timer timer1;



// Code ---------------------------------------------------------------------

bool timer_callback(struct repeating_timer* t) {
    //printf("Timer fired!\n");
    LEF_systick();
    gpio_put(PICO_DEFAULT_LED_PIN, LEF_Led_update(&led_board));
    LEF_Timer_update(&timer1);
    // LEF_Send_msg(LEF_EVENT_STDIO, 0);
    LEF_QueuePush(&lef_std_queue, &(LEF_Event){.id = LEF_EVENT_STDIO, .func = 0});

    return true;  // return true to keep repeating
}

static void hw_init(void) {
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    struct repeating_timer timer;
    add_repeating_timer_ms(10, timer_callback, NULL, &timer);
}

static bool main_event_handler(LEF_Event* event) {
    switch (event->id) {
        case LEF_EVENT_TIMER1:
            LEF_Led_set(&led_board, LED_DOUBLE_BLINK);
            break;
        case LEF_EVENT_STDIO:
            int c;
            while ((c = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
                LEF_Cli_putc(c & 0xFF);
                LEF_Led_set(&led_board, LED_SINGLE_BLINK);
            }
            break;
        case LEF_EVENT_CLI:
            LEF_Cli_exec(event);
            break;
        default:
            break;
    }

    return false;
}

static void cmd_reset(char* args) {
    printf("Resetting system...\n");
    watchdog_reboot(0, 0, 0);
    // return true;
}

static void cmd_sysinfo(char* args) {
    LEF_print_sysinfo();
    // return true;
}

static void cmd_help(char* args) {
    LEF_Cli_print();
    // return true;
}

const LEF_CliCmd cmdTable[] = {

    LEF_CLI_LABEL("Misc"),

    {cmd_reset, "reset", "Reset the system"},
    {cmd_sysinfo, "sysinfo", "Print LEF system information"},
    LEF_CLI_CMD(cmd_help, "help", "Show help")};

int main(void) {

    LEF_init();
    LEF_Led_init(&led_board, LED_BLIP);
    LEF_Timer_init(&timer1, LEF_EVENT_TIMER1);
    LEF_Timer_start_repeat(&timer1, 400);
    LEF_CLI_INIT(cmdTable);

    hw_init();

    LEF_Run(main_event_handler, NULL);
    
    return 0;
}
