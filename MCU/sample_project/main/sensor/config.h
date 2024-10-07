#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"  // 添加 esp_err_t

// 定义dispdata结构体
typedef struct {
    bool ble_enable;
    bool g4_enable;
    bool IR_enable;
    bool first_config;
    bool hiden_enable;
    uint8_t disp_screen_id;
    uint8_t disp_chat_id;
    uint16_t log_index;
    uint8_t log[1000];
    char chat_data[50][100];
    uint32_t color[50];  
    uint8_t speed[50];  
} dispdata_t;

// 函数声明
esp_err_t config_init(void);
esp_err_t config_read(dispdata_t *data);
esp_err_t config_write(const dispdata_t *data);
void config_initialize_defaults(dispdata_t *data);

// 用于读取和保存大型数组
esp_err_t config_read_large_arrays(dispdata_t *data);
esp_err_t config_write_large_arrays(const dispdata_t *data);

#endif // CONFIG_H
