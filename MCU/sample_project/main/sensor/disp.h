#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "led_strip.h"
#include "esp_log.h"

#define LED_STRIP_LED_NUMBERS 768
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)
#define LV_HOR_RES_MAX 96
#define LV_VER_RES_MAX 32
#define DISP_BUF_SIZE LV_HOR_RES_MAX * LV_VER_RES_MAX * 3

struct Rel_display
{
    led_strip_handle_t rel_dataline; // rmt tx channel 0 ... 3
    int16_t baseline;                // ( index_led_nums // max_signal_display_led_nums )
};

extern void init_led_disp(void);
extern void fulsh_led_disp(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
extern void update_led_disp(void);