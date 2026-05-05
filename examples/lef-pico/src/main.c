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

#include "main.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "LEF.h"
#include "LEF_Cli.h"
#include "LEF_Led.h"
#include "LEF_Timer.h"
#include "hardware/adc.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

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
    LEF_QueuePush(lef_std_queue, &(LEF_Event){.id = LEF_EVENT_STDIO, .func = 0});

    return true;  // return true to keep repeating
}



static float read_onboard_temperature() {
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    return  tempC; // default to Celsius if unit is unrecognized
}

static void cmd_temperature() {
    float temperature = read_onboard_temperature();
    printf("Temperature: %.02f C\n", temperature);
}

static void cmd_reset_usb(char* args) {
    printf("Booting into USB storage mode\n");
    reset_usb_boot(0, 0);
}
static void cmd_reset(char* args) {
    printf("Resetting system...\n");
    watchdog_reboot(0, 0, 0);
}

static void cmd_sysinfo(char* args) {
    LEF_print_sysinfo();
}

static void cmd_help(char* args) {
    LEF_Cli_print();
}

const LEF_CliCmd cmdTable[] = {

    LEF_CLI_LABEL("Misc"),
    {cmd_temperature, "temp", "Read onboard temperature"},
    {cmd_reset, "reset", "Reset the system"},
    {cmd_reset_usb, "reset_usb", "Reset into USB storage mode"},
    {cmd_sysinfo, "info", "Print LEF system information"},
    LEF_CLI_CMD(cmd_help, "help", "Show help")
};

static void hw_init(void) {
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

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

int main(void) {

    LEF_init();
    LEF_Led_init(&led_board, LED_BLIP);
    LEF_Timer_init(&timer1, LEF_EVENT_TIMER1);
    LEF_Timer_start_repeat(&timer1, 400);
    LEF_CLI_INIT(cmdTable);

    hw_init();

    printf("\n\nStarting LEF Pico demo\n\n");

    LEF_Run(main_event_handler, NULL);
    
    return 0;
}
