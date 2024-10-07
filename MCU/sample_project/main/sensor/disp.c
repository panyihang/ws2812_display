#include "disp.h"

static const char *TAG = "ws2812";

struct Rel_display Rel_display[12];

led_strip_handle_t led_strip0;
led_strip_handle_t led_strip1;
led_strip_handle_t led_strip2;
led_strip_handle_t led_strip3;

void get_id_and_index(int x, int y, int *id, int *index)
{
    // 计算 id
    int row_id = x / 16;
    int col_id = y / 16;
    *id = row_id * 6 + col_id;

    // 计算 index
    int local_x = x % 16;
    int local_y = y % 16;
    *index = local_x * 16 + local_y;
}

led_strip_handle_t configure_led(int gpio)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = gpio,                   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
    return led_strip;
}

 void init_led_disp(void)
{

    led_strip0 = configure_led(GPIO_NUM_13);
    led_strip1 = configure_led(GPIO_NUM_12);
    led_strip2 = configure_led(GPIO_NUM_11);
    led_strip3 = configure_led(GPIO_NUM_10);

    Rel_display[0].rel_dataline = led_strip0;
    Rel_display[0].baseline = 512;
    Rel_display[1].rel_dataline = led_strip0;
    Rel_display[1].baseline = 256;
    Rel_display[2].rel_dataline = led_strip0;
    Rel_display[2].baseline = 0;

    Rel_display[3].rel_dataline = led_strip1;
    Rel_display[3].baseline = 0;
    Rel_display[4].rel_dataline = led_strip1;
    Rel_display[4].baseline = 256;
    Rel_display[5].rel_dataline = led_strip1;
    Rel_display[5].baseline = 512;

    Rel_display[6].rel_dataline = led_strip2;
    Rel_display[6].baseline = 512;
    Rel_display[7].rel_dataline = led_strip2;
    Rel_display[7].baseline = 256;
    Rel_display[8].rel_dataline = led_strip2;
    Rel_display[8].baseline = 0;

    Rel_display[9].rel_dataline = led_strip3;
    Rel_display[9].baseline = 0;
    Rel_display[10].rel_dataline = led_strip3;
    Rel_display[10].baseline = 256;
    Rel_display[11].rel_dataline = led_strip3;
    Rel_display[11].baseline = 512;

    for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip0, i, 0, 0, 0));
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip1, i, 0, 0, 0));
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip2, i, 0, 0, 0));
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip3, i, 0, 0, 0));
    }
}

void fulsh_led_disp(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
    int id, index;
    get_id_and_index(x, y, &id, &index);
    led_strip_set_pixel(Rel_display[id].rel_dataline, (Rel_display[id].baseline) + index, red, green, blue);
}

void update_led_disp(void)
{
    led_strip_refresh(led_strip0);
    led_strip_refresh(led_strip1);
    led_strip_refresh(led_strip2);
    led_strip_refresh(led_strip3);
}