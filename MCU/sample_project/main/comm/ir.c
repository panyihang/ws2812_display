#include "ir.h"
#include "../ui/ui_events.h"

static const char *TAG = "IR_DECODER";

static uint16_t s_nec_code_address;
static uint16_t s_nec_code_command;

#define IR_SOURCE 0x02
#define EXAMPLE_IR_RESOLUTION_HZ 1000000 // 1MHz resolution, 1 tick = 1us
#define EXAMPLE_IR_RX_GPIO_NUM 41
#define EXAMPLE_IR_NEC_DECODE_MARGIN 200 // Tolerance for parsing RMT symbols into bit stream

#define NEC_LEADING_CODE_DURATION_0 9000
#define NEC_LEADING_CODE_DURATION_1 4500
#define NEC_PAYLOAD_ZERO_DURATION_0 560
#define NEC_PAYLOAD_ZERO_DURATION_1 560
#define NEC_PAYLOAD_ONE_DURATION_0 560
#define NEC_PAYLOAD_ONE_DURATION_1 1690
#define NEC_REPEAT_CODE_DURATION_0 9000
#define NEC_REPEAT_CODE_DURATION_1 2250

static inline bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + EXAMPLE_IR_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - EXAMPLE_IR_NEC_DECODE_MARGIN));
}

static bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

static bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

static bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols)
{
    rmt_symbol_word_t *cur = rmt_nec_symbols;
    uint16_t address = 0;
    uint16_t command = 0;
    bool valid_leading_code = nec_check_in_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                              nec_check_in_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);
    if (!valid_leading_code)
    {
        return false;
    }
    cur++;
    for (int i = 0; i < 16; i++)
    {
        if (nec_parse_logic1(cur))
        {
            address |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            address &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }
    for (int i = 0; i < 16; i++)
    {
        if (nec_parse_logic1(cur))
        {
            command |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            command &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }
    // save address and command
    s_nec_code_address = address;
    s_nec_code_command = command;
    return true;
}

static bool nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_REPEAT_CODE_DURATION_1);
}

static uint8_t current_number = 0;
static bool waiting_for_ok = false;
static TimerHandle_t number_timer = NULL;
uint8_t shutdown_count = 20;

// Reset the current number input
void reset_number_input()
{
    current_number = 0;
    waiting_for_ok = false;
    if (number_timer != NULL)
    {
        xTimerStop(number_timer, 0);
    }
}
// Timer callback
void number_timer_callback(TimerHandle_t xTimer)
{
    reset_number_input();
}

// Handle number input
void handle_number_input(uint8_t number)
{
    if (waiting_for_ok)
    {
        current_number = current_number * 10 + number;
    }
    else
    {
        current_number = number;
        waiting_for_ok = true;
    }

    char number_str[16]; // 定义一个足够大的字符数组来存储转换后的字符串

    snprintf(number_str, sizeof(number_str), "%d", current_number); // 将数字转换为字符串

    disp_msg((const char *)number_str, IR_SOURCE); // 调用disp_msg函数

    ESP_LOGI(TAG, "Current Number Input: %d", current_number);

    if (number_timer != NULL)
    {
        xTimerReset(number_timer, 0);
    }
    else
    {
        number_timer = xTimerCreate("Number Timer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, number_timer_callback);
        xTimerStart(number_timer, 0);
    }

    if (current_number > 50)
    {
        ESP_LOGE(TAG, "Invalid Number Input: %d. Resetting input.", current_number);
        disp_msg("ERROR", IR_SOURCE);
        reset_number_input(); // Invalid input, reset
    }
}

// Handle OK button
void handle_ok_button()
{
    if (waiting_for_ok)
    {
        set_disp_chat_id(current_number, IR_SOURCE);
        reset_number_input();
        disp_msg("OK", IR_SOURCE);
    }
    else
    {

        ESP_LOGE(TAG, "OK button pressed without a valid number input.");
    }
}

// Handle least/next buttons
void handle_least_next_button(bool next)
{
    uint8_t chat_id = get_disp_chat_id();
    if (next)
    {
        if (chat_id < 50)
        {
            chat_id++;
        }
        else
        {
            ESP_LOGE(TAG, "Next button pressed but Chat ID is already at maximum (50).");
        }
    }
    else
    {
        if (chat_id > 0)
        {
            chat_id--;
        }
        else
        {
            ESP_LOGE(TAG, "Least button pressed but Chat ID is already at minimum (0).");
        }
    }
    char number_str[16];                                     // 定义一个足够大的字符数组来存储转换后的字符串
    snprintf(number_str, sizeof(number_str), "%d", chat_id); // 将数字转换为字符串
    disp_msg((const char *)number_str, IR_SOURCE);           // 调用disp_msg函数
    set_disp_chat_id(chat_id, IR_SOURCE);
}

// Handle up/down buttons
void handle_up_down_button(bool up)
{
    uint16_t speed = get_speed(get_disp_chat_id());

    if (up)
    {
        speed += 5; // Increase speed by 1
    }
    else
    {
        if (speed > 5)
        {
            speed -= 5; // Decrease speed by 1
        }
        else
        {
            ESP_LOGE(TAG, "Down button pressed but Speed is already at minimum.");
            return;
        }
    }

    set_speed(get_disp_chat_id(), speed, IR_SOURCE);
    char number_str[16];
    snprintf(number_str, sizeof(number_str), "s %d", speed); // 将数字转换为字符串
    disp_msg((const char *)number_str, IR_SOURCE);           // 调用disp_msg函数
}

void handle_power_button()
{
    set_hiden_enable(!get_hiden_enable(), IR_SOURCE);
    disp_msg("Hiden", IR_SOURCE);
}

// Handle IR command based on received code
void handle_ir_command(uint16_t command)
{
    switch (command)
    {
    case 0xE916:
        handle_number_input(0);
        break;
    case 0xF30C:
        handle_number_input(1);
        break;
    case 0xE718:
        handle_number_input(2);
        break;
    case 0xA15E:
        handle_number_input(3);
        break;
    case 0xF708:
        handle_number_input(4);
        break;
    case 0xE31C:
        handle_number_input(5);
        break;
    case 0xA55A:
        handle_number_input(6);
        break;
    case 0xBD42:
        handle_number_input(7);
        break;
    case 0xAD52:
        handle_number_input(8);
        break;
    case 0xB54A:
        handle_number_input(9);
        break;
    case 0xEA15:
        handle_ok_button();
        break;
    case 0xF807:
        handle_least_next_button(false);
        break; // Least
    case 0xF609:
        handle_least_next_button(true);
        break; // Next
    case 0xBF40:
        handle_up_down_button(true);
        break; // Up
    case 0xE619:
        handle_up_down_button(false);
        break; // Down
    case 0xBA45:
        handle_power_button();
        shutdown_count = 20;
        break; // power
    case 0xBC43:
        break; // reset
        
    default:
        ESP_LOGE(TAG, "Unknown command: 0x%04X", command);
        break;
    }
}


static void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num)
{
    // decode RMT symbols

    // 0-9 E916 F30C E718 A15E F708 E31C A55A BD42 AD52 B54A
    // power BA45
    // least next  F807 F609
    // up down（speed) BF40 E619
    // ok EA15

    switch (symbol_num)
    {
    case 34: // NEC normal frame
        if (nec_parse_frame(rmt_nec_symbols))
        {
            printf("Address=%04X, Command=%04X\r\n\r\n", s_nec_code_address, s_nec_code_command);
            if (s_nec_code_address == 0xFF00)
            {
                handle_ir_command(s_nec_code_command);
            }
        }
        break;
    case 2: // NEC repeat frame
        if (nec_parse_frame_repeat(rmt_nec_symbols))
        {
            if (s_nec_code_address == 0xFF00 && s_nec_code_command == 0xBA45)
            {
                if (get_hiden_enable())
                {
                    shutdown_count--;
                    char number_str[16];
                    snprintf(number_str, sizeof(number_str), "P %d", shutdown_count); // 将数字转换为字符串
                    disp_msg((const char *)number_str, IR_SOURCE);
                    if (shutdown_count == 0)
                    {
                        esp_restart();
                    }
                }
            }
            printf("Address=%04X, Command=%04X, repeat\r\n\r\n", s_nec_code_address, s_nec_code_command);
        }
        break;
    default:
        printf("Unknown NEC frame\r\n\r\n");
        break;
    }
}

static bool example_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    // send the received RMT symbols to the parser task
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}
static void ir_receive_task(void *arg)
{
    ESP_LOGI(TAG, "create RMT RX channel");
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = EXAMPLE_IR_RESOLUTION_HZ,
        .mem_block_symbols = 64, // amount of RMT symbols that the channel can store at a time
        .gpio_num = EXAMPLE_IR_RX_GPIO_NUM,
    };
    rmt_channel_handle_t rx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    ESP_LOGI(TAG, "register RX done callback");
    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = example_rmt_rx_done_callback,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));

    // The following timing requirement is based on the NEC protocol
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,     // The shortest duration for NEC signal is 560us, 1250ns < 560us, valid signal won't be treated as noise
        .signal_range_max_ns = 12000000, // The longest duration for NEC signal is 9000us, 12000000ns > 9000us, the receive won't stop early
    };

    ESP_LOGI(TAG, "install IR NEC encoder");
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = EXAMPLE_IR_RESOLUTION_HZ,
    };
    rmt_encoder_handle_t nec_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));

    ESP_LOGI(TAG, "enable RMT RX channel");
    ESP_ERROR_CHECK(rmt_enable(rx_channel));

    // Save the received RMT symbols
    rmt_symbol_word_t raw_symbols[64]; // 64 symbols should be sufficient for a standard NEC frame
    rmt_rx_done_event_data_t rx_data;

    // Start receiving
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));

    while (1)
    {
        // Wait for RX done signal
        if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            // Parse the received symbols and print the result
            example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);
            // Start receiving again
            ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
        }
        else
        {
            // Handle timeout or any other task-specific operations here
        }

        // Add a small delay to yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Cleanup code if needed
    vTaskDelete(NULL); // Delete the task when finished
}

void ir_init(void)
{
    // Create the IR receive task on CPU1
    xTaskCreatePinnedToCore(ir_receive_task, "IR Receive Task", 4096, NULL, 0, NULL, 0);
}