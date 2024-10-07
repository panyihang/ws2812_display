#include "asr.h"
#include "../ui/ui_events.h"

static const char *TAG = "ASR";

#define ASR_SOURCE 0x04

#define EX_UART_NUM UART_NUM_1
#define ASR_RX GPIO_NUM_40
#define ASR_TX GPIO_NUM_39

#define PATTERN_CHR_NUM (3)
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;
static int setupflags = 0;
static int checkflags = 0;

void process_uart_data(uint8_t *data, size_t length)
{
    char command[3] = {0}; // Assuming 2 characters commands like "C1", "CH", etc.
    if (length >= 2)
    {
        strncpy(command, (char *)data, 2); // Copy the first two characters
    }
    else
    {
        ESP_LOGI(TAG, "Received data with insufficient length: %.*s", length, data);
        return;
    }

    // Process based on the exact command string
    if (strcmp(command, "C1") == 0)
    {
        uint8_t index = get_disp_chat_id();
        if (index > 0)
        {
            set_disp_chat_id(index - 1, ASR_SOURCE);
        }
        char number_str[8];                                    // 定义一个足够大的字符数组来存储转换后的字符串
        snprintf(number_str, sizeof(number_str), "%d", index); // 将数字转换为字符串
        disp_msg((const char *)number_str, ASR_SOURCE);        // 调用disp_msg函数
        ESP_LOGI(TAG, "Processing C1 command");
    }
    else if (strcmp(command, "C2") == 0)
    {
        uint8_t index = get_disp_chat_id();
        if (index < 49)
        {
            set_disp_chat_id(index + 1, ASR_SOURCE);
        }
        char number_str[8];                                    // 定义一个足够大的字符数组来存储转换后的字符串
        snprintf(number_str, sizeof(number_str), "%d", index); // 将数字转换为字符串
        disp_msg((const char *)number_str, ASR_SOURCE);        // 调用disp_msg函数
        ESP_LOGI(TAG, "Processing C2 command");
    }
    else if (strcmp(command, "CH") == 0)
    {
        set_hiden_enable(!get_hiden_enable(), ASR_SOURCE);
        disp_msg("Hiden", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing CH command");
        // Add functionality for CH command here
    }
    else if (strcmp(command, "C3") == 0)
    {

        uint8_t index = get_disp_chat_id();
        uint8_t speed = get_speed(index);
        if (speed > 5 && speed < 250)
        {
            speed += 3;
            set_speed(index, speed, ASR_SOURCE);
            char number_str[16];
            snprintf(number_str, sizeof(number_str), "s %d", speed); // 将数字转换为字符串
            disp_msg((const char *)number_str, ASR_SOURCE);          // 调用disp_msg函数
        }

        ESP_LOGI(TAG, "Processing C3 command");
    }
    else if (strcmp(command, "C4") == 0)
    {
        uint8_t index = get_disp_chat_id();
        uint8_t speed = get_speed(index);
        if (speed > 5 && speed < 250)
        {
            speed -= 3;
            set_speed(index, speed, ASR_SOURCE);
            char number_str[16];
            snprintf(number_str, sizeof(number_str), "s %d", speed); // 将数字转换为字符串
            disp_msg((const char *)number_str, ASR_SOURCE);          // 调用disp_msg函数
        }
        ESP_LOGI(TAG, "Processing C4 command");
        // Add functionality for C4 command here
    }
    else if (strcmp(command, "D0") == 0)
    {
        set_disp_chat_id(0, ASR_SOURCE);
        disp_msg("0", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D0 command");
        // Add functionality for D0 command here
    }
    else if (strcmp(command, "D1") == 0)
    {
        set_disp_chat_id(1, ASR_SOURCE);
        disp_msg("1", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D1 command");
        // Add functionality for D1 command here
    }
    else if (strcmp(command, "D2") == 0)
    {
        set_disp_chat_id(2, ASR_SOURCE);
        disp_msg("2", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D2 command");
        // Add functionality for D2 command here
    }
    else if (strcmp(command, "D3") == 0)
    {
        set_disp_chat_id(3, ASR_SOURCE);
        disp_msg("3", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D3 command");
        // Add functionality for D3 command here
    }
    else if (strcmp(command, "D4") == 0)
    {
        set_disp_chat_id(4, ASR_SOURCE);
        disp_msg("4", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D4 command");
        // Add functionality for D4 command here
    }
    else if (strcmp(command, "D5") == 0)
    {
        set_disp_chat_id(5, ASR_SOURCE);
        disp_msg("5", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D5 command");
        // Add functionality for D5 command here
    }
    else if (strcmp(command, "D6") == 0)
    {
        set_disp_chat_id(6, ASR_SOURCE);
        disp_msg("6", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D6 command");
        // Add functionality for D6 command here
    }
    else if (strcmp(command, "D7") == 0)
    {
        set_disp_chat_id(7, ASR_SOURCE);
        disp_msg("7", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D7 command");
        // Add functionality for D7 command here
    }
    else if (strcmp(command, "D8") == 0)
    {
        set_disp_chat_id(8, ASR_SOURCE);
        disp_msg("8", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D8 command");
        // Add functionality for D8 command here
    }
    else if (strcmp(command, "D9") == 0)
    {
        set_disp_chat_id(9, ASR_SOURCE);
        disp_msg("9", ASR_SOURCE);
        ESP_LOGI(TAG, "Processing D9 command");
        // Add functionality for D9 command here
    }
    else if (strcmp(command, "RS") == 0)
    {
        esp_restart();
        ESP_LOGI(TAG, "Processing RST (Reset) command");
        // Add functionality for reset command here
    }
    else
    {
        ESP_LOGW(TAG, "Unknown command: %.*s", length, data);
    }
}

static void asr_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE);
    for (;;)
    {
        if (xQueueReceive(uart0_queue, (void *)&event, (TickType_t)portMAX_DELAY))
        {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
            switch (event.type)
            {
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                // 调用处理数据的函数
                process_uart_data(dtmp, event.size);
                // 例外情况：回传接收到的数据
                break;
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart0_queue);
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart0_queue);
                break;
            // Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            // Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            // Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            // UART_PATTERN_DET
            case UART_PATTERN_DET:
                uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);
                int pos = uart_pattern_pop_pos(EX_UART_NUM);
                ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                if (pos == -1)
                {
                    uart_flush_input(EX_UART_NUM);
                }
                else
                {
                    uart_read_bytes(EX_UART_NUM, dtmp, pos, 100 / portTICK_PERIOD_MS);
                    uint8_t pat[PATTERN_CHR_NUM + 1];
                    memset(pat, 0, sizeof(pat));
                    uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                    ESP_LOGI(TAG, "read data: %s", dtmp);
                    ESP_LOGI(TAG, "read pat : %s", pat);
                }
                break;
            // Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

// 主函数
void ASR_init()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install UART driver, and get the queue.
    uart_param_config(EX_UART_NUM, &uart_config);
    uart_set_pin(EX_UART_NUM, ASR_TX, ASR_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    // Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
    // Reset the pattern queue length to record at most 20 pattern positions.

    uart_pattern_queue_reset(EX_UART_NUM, 20);
    ESP_LOGI(TAG, "uart ok");

    // Create a task to handler UART event from ISR
    xTaskCreatePinnedToCore(asr_task, "asr_task", 4096, NULL, 3, NULL, 1);
}
