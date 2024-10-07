#include "config.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <stdio.h>  // for snprintf
#include <string.h> // for memset

static const char *TAG = "CONFIG";
static const char *NAMESPACE = "storage";
static const char *KEY = "dispdata";

// 初始化 NVS
esp_err_t config_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(err));
    }
    return err;
}

// 读取 config（dispdata）小型变量部分
esp_err_t config_read(dispdata_t *data)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS open  small read failed: %s", esp_err_to_name(err));
        return err;
    }

    // 仅读取小型变量
    size_t required_size = sizeof(bool) * 5 + sizeof(uint8_t) * 2 + sizeof(uint16_t);
    err = nvs_get_blob(handle, KEY, data, &required_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS get_blob failed: %s", esp_err_to_name(err));
    }

    nvs_close(handle);
    return err;
}

// 保存 config（dispdata）小型变量部分
esp_err_t config_write(const dispdata_t *data)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS open small write failed: %s", esp_err_to_name(err));
        return err;
    }

    // 仅保存小型变量
    size_t data_size = sizeof(bool) * 5 + sizeof(uint8_t) * 2 + sizeof(uint16_t);
    err = nvs_set_blob(handle, KEY, data, data_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS set_blob failed: %s", esp_err_to_name(err));
    }
    else
    {
        err = nvs_commit(handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(err));
        }
    }

    nvs_close(handle);
    return err;
}

// 读取大型数组
esp_err_t config_read_large_arrays(dispdata_t *data)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS open large read failed: %s", esp_err_to_name(err));
        return err;
    }

    // 读取 log 数组
    size_t log_size = sizeof(data->log);
    err = nvs_get_blob(handle, "log", data->log, &log_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS get_blob log failed: %s", esp_err_to_name(err));
    }

    // 读取 chat_data 数组
    size_t chat_data_size = sizeof(data->chat_data);
    err = nvs_get_blob(handle, "chat_data", data->chat_data, &chat_data_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS get_blob chat_data failed: %s", esp_err_to_name(err));
    }

    // 读取 color 数组
    size_t color_size = sizeof(data->color);
    err = nvs_get_blob(handle, "color", data->color, &color_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS get_blob color failed: %s", esp_err_to_name(err));
    }

    // 读取 speed 数组
    size_t speed_size = sizeof(data->speed);
    err = nvs_get_blob(handle, "speed", data->speed, &speed_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS get_blob speed failed: %s", esp_err_to_name(err));
    }

    nvs_close(handle);
    return err;
}

// 保存大型数组
esp_err_t config_write_large_arrays(const dispdata_t *data)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS open large write failed: %s", esp_err_to_name(err));
        return err;
    }

    // 保存 log 数组
    err = nvs_set_blob(handle, "log", data->log, sizeof(data->log));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS set_blob log failed: %s", esp_err_to_name(err));
    }

    // 保存 chat_data 数组
    err = nvs_set_blob(handle, "chat_data", data->chat_data, sizeof(data->chat_data));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS set_blob chat_data failed: %s", esp_err_to_name(err));
    }

    // 保存 color 数组
    err = nvs_set_blob(handle, "color", data->color, sizeof(data->color));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS set_blob color failed: %s", esp_err_to_name(err));
    }

    // 保存 speed 数组
    err = nvs_set_blob(handle, "speed", data->speed, sizeof(data->speed));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS set_blob speed failed: %s", esp_err_to_name(err));
    }

    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(err));
    }

    nvs_close(handle);
    return err;
}

// 初始化默认的 config 数据
void config_initialize_defaults(dispdata_t *data)
{
    data->ble_enable = true;
    data->g4_enable = true;
    data->IR_enable = true;
    data->first_config = true; // 第一次初始化后置为 false
    data->hiden_enable = false;
    data->disp_screen_id = 1;
    data->disp_chat_id = 1;
    data->log_index = 0;
    memset(data->log, 0, sizeof(data->log));

    // 初始化 chat_data，填充“这是第%d条内容”
    for (int i = 0; i < 50; i++)
    {
        snprintf(data->chat_data[i], sizeof(data->chat_data[i]), "这是第%d条内容", i);
    }

    // 初始化 color 和 speed
    for (int i = 0; i < 50; i++)
    {
        data->color[i] = 0xFFFFFF; // 默认白色
        data->speed[i] = 30;       // 默认速度 30
    }
}
