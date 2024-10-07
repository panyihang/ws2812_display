#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

extern void start_cat1(void);