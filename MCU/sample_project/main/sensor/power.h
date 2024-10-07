#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_err.h"

static int adc_raw[2][10];
static int voltage[2][10];


extern void power_init(void);
extern void power_get(float *vol_out, float *curr_out);