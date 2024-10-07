
#include "4g.h"
#include "../ui/ui_events.h"
#include "power.h"

#define TAG "4G_MQTT"

#define BUF_SIZE 1024
#define TIMEOUT_MS 5000 // 超时时间设置为5秒

#define UART_NUM UART_NUM_2
#define BUF_SIZE 1024
#define TXD_PIN 38
#define RXD_PIN 37

// MQTT配置
const char *mqtt_broker = "119.3.158.32";
const char *mqtt_topic = "devices/telemetry/control/%!(EXTRA string=70d611ae-0ae2-7adf-806d-6895a274475f)";

const char *mqtt_user = "1b3a6aa3-2642-0ed6-2ca";
const char *mqtt_password = "d7f8665";
const char *mqtt_client_id = "mqtt_70d611ae-0ae";

// 标志位，表示是否需要重新执行mqtt_communication
bool mqtt_error_flag = false;

void uart_init()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
}

// 发送AT命令
void send_at_command(const char *cmd)
{
    uart_write_bytes(UART_NUM_2, cmd, strlen(cmd));
}

// 读取响应并解析
int read_response(char *data, int max_len)
{
    int len = uart_read_bytes(UART_NUM_2, data, max_len - 1, 20 / portTICK_PERIOD_MS);
    if (len > 0)
    {
        data[len] = '\0'; // 添加字符串结束符
        ESP_LOGI(TAG, "Recv str: %s", data);
    }
    return len;
}

// 轮询等待特定的响应，正确返回true，超时或错误返回false
bool wait_for_response(const char *expected_response, int timeout_ms, const char *error_response)
{
    TickType_t start_time = xTaskGetTickCount();
    while (xTaskGetTickCount() - start_time < timeout_ms / portTICK_PERIOD_MS)
    {
        char data[BUF_SIZE];
        if (read_response(data, sizeof(data)) > 0)
        {
            if (strstr(data, expected_response))
            {
                return true; // 收到期望的响应
            }
            // 检查是否有错误响应，并且错误响应非空
            if (error_response != NULL && strlen(error_response) > 0 && strstr(data, error_response))
            {
                return false; // 收到错误响应
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // 延时100ms
    }
    return false; // 超时未收到期望的响应
}

// 重启模块并等待RDY
void restart_module_and_wait_for_rdy()
{
    send_at_command("AT+CFUN=1,1\r\n");
    if (wait_for_response("+NITZ", TIMEOUT_MS,""))
    {
        ESP_LOGI(TAG, "Module restarted and ready.");
    }
    else
    {
        ESP_LOGI(TAG, "Module did not respond with RDY or timeout.");
    }
}
// 检测SIM卡状态
void check_sim_card()
{
    send_at_command("AT+CPIN?\r\n");
    if (wait_for_response("READY", TIMEOUT_MS,""))
    {
        ESP_LOGI(TAG, "SIM card is ready.");
    }
    else
    {
        ESP_LOGI(TAG, "SIM card is not ready or timeout.");
    }
}

// 检测是否附着上数据网络
void check_data_network_attachment()
{
    send_at_command("AT+CGATT?\r\n");
    if (wait_for_response("+CGATT: 1", TIMEOUT_MS,""))
    {
        ESP_LOGI(TAG, "Attached to data network.");
    }
    else
    {
        ESP_LOGI(TAG, "Not attached to data network or timeout.");
    }
}

// 配置网络
void configure_network()
{
    char cmd[BUF_SIZE];
    sprintf(cmd, "AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n", mqtt_client_id, mqtt_user, mqtt_password);
    send_at_command(cmd);

    if (wait_for_response("OK", TIMEOUT_MS,""))
    {
        ESP_LOGI(TAG, "Network configured.");
    }
}

// 启动MQTT连接
void start_mqtt_connection()
{
    send_at_command("AT+MIPSTART=\"119.3.158.32\",\"1883\"\r\n");
    if (wait_for_response("CONNECT OK", TIMEOUT_MS,""))
    {
        ESP_LOGI(TAG, "MQTT connection started.");
    }
}

// 连接MQTT服务器
void connect_mqtt_server()
{
    send_at_command("AT+MCONNECT=1,120\r\n");
    if (wait_for_response("CONNACK OK", TIMEOUT_MS,""))
    {
        ESP_LOGI(TAG, "Connected to MQTT server.");
    }
}

// 订阅MQTT主题
void subscribe_mqtt_topic(const char *topic)
{
    char cmd[BUF_SIZE];
    sprintf(cmd, "AT+MSUB=\"%s\",0\r\n", topic);
    send_at_command(cmd);
    if (wait_for_response("SUBACK", TIMEOUT_MS,""))
    {
        ESP_LOGI(TAG, "Subscribed to MQTT topic.");
    }
}

void publish_mqtt_long_message(const char *topic, int qos, cJSON *payload)
{
    char cmd[BUF_SIZE];
    sprintf(cmd, "AT+MPUBEX=%s,%d,0\r\n", topic, qos);
    send_at_command(cmd);

    if (qos == 0)
    {
        if (wait_for_response(">", TIMEOUT_MS, "+CME ERROR: 767"))
        {
            ESP_LOGI(TAG, "Start send long msg.");
            char *json_str = cJSON_Print(payload);
            send_at_command(json_str);
            free(json_str);
            send_at_command("\x1A"); // 发送结束符0x1A

            // 检查响应是否为OK，且检查是否有错误响应
            if (wait_for_response("OK", TIMEOUT_MS, "+CME ERROR: 767"))
            {
                ESP_LOGI(TAG, "Long message published successfully.");
            }
            else
            {
                ESP_LOGE(TAG, "Failed to publish long message, encountered +CME ERROR: 767.");
                mqtt_error_flag = true; // 设置标志位以便后续处理
            }
        }
        else
        {
            mqtt_error_flag = true;
            ESP_LOGE(TAG, "Failed to initiate long message publishing.");
        }
    }
}

// 发布MQTT消息
void publish_mqtt_message(const char *topic, int qos, const char *payload)
{
    char cmd[BUF_SIZE];
    sprintf(cmd, "AT+MPUB=\"%s\",%d,0,\"%s\"\r\n", topic, qos, payload);
    send_at_command(cmd);
    if (qos == 0)
    {
        wait_for_response("OK", TIMEOUT_MS,"");
    }
    else if (qos == 1)
    {
        wait_for_response("PUBACK", TIMEOUT_MS,"");
    }
    else if (qos == 2)
    {
        wait_for_response("PUBCOMP", TIMEOUT_MS,"");
    }
}

// 完整的4G MQTT通信流程
void mqtt_communication()
{
    restart_module_and_wait_for_rdy();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    check_sim_card();
    check_data_network_attachment();
    configure_network();
    start_mqtt_connection();
    connect_mqtt_server();
}




void send_json_to_server()
{

    // 创建cJSON对象
    cJSON *root = cJSON_CreateObject();
    uint8_t disp_index = get_disp_chat_id();
    char data[100];
    get_chat_data(disp_index, data);

    cJSON_AddStringToObject(root, "disp_msg", data);
    cJSON_AddNumberToObject(root, "disp_index", disp_index);
    cJSON_AddNumberToObject(root, "SPEED", get_speed(disp_index));
    cJSON_AddNumberToObject(root, "Color", get_color(disp_index));
    cJSON_AddBoolToObject(root, "hiden", get_hiden_enable());

    cJSON_AddBoolToObject(root, "BLE_Enable", 1);
    cJSON_AddBoolToObject(root, "IR_Enable", get_IR_enable());
    cJSON_AddBoolToObject(root, "4G_Enable", get_g4_enable());

    cJSON_AddBoolToObject(root, "ASR_Enable", 1);

    float v, c;
    power_get(&v, &c);

    cJSON_AddNumberToObject(root, "V", v);
    cJSON_AddNumberToObject(root, "A", c);
    cJSON_AddNumberToObject(root, "POWER", v * c);

    // 发布MQTT长消息
    publish_mqtt_long_message("devices/telemetry", 0, root);

    // 如果遇到错误则执行mqtt_communication函数
    if (mqtt_error_flag)
    {
        ESP_LOGI(TAG, "Attempting to reconnect using mqtt_communication due to error.");
        mqtt_communication();
        mqtt_error_flag = false; // 重置标志位
    }

    // 删除cJSON对象
    cJSON_Delete(root);
}

static void mqtt_task()
{
    uart_init();

    mqtt_communication();

    // subscribe_mqtt_topic("devices/telemetry/control/70d611ae-0ae2-7adf-806d-6895a274475f");
    // subscribe_mqtt_topic("devices/attributes/set/70d611ae-0ae2-7adf-806d-6895a274475f/8216256");
    subscribe_mqtt_topic(mqtt_topic);

    while (1)
    {
        char data[BUF_SIZE];
        int len = read_response(data, sizeof(data));
        read_response(data, sizeof(data));
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf("free_heap_size = %ldn", esp_get_free_heap_size()); 
        send_json_to_server(mqtt_topic);
    }
}

void start_cat1(void)
{
    xTaskCreatePinnedToCore(mqtt_task, "mqtt_task", 10240, NULL, 4, NULL, 1);
}