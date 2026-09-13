#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include <esp_log.h>
#include <esp_timer.h>
#include <stdio.h>

#define SMA_WINDOW_SIZE 16

const gpio_num_t readingPin = GPIO_NUM_4;
const gpio_num_t ledPin = GPIO_NUM_18;
adc_channel_t channel;
adc_oneshot_unit_handle_t adc;
int smaAverage = 0;
int turnLightOnAt = 1500;
int turnLightOffAt = 1700;
int smaCurrentSum = 0;
int smaValues[16] = {0};
bool smaValuesFull = false;

void setupAdc() {
  adc_unit_t unit;

  ESP_ERROR_CHECK(adc_oneshot_io_to_channel(readingPin, &unit, &channel));
  ESP_LOGI("Main App: ", "Unit: %d, channel: %d", unit, channel);

  const adc_oneshot_unit_init_cfg_t adcConfig = {.unit_id = unit};
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&adcConfig, &adc));

  adc_oneshot_chan_cfg_t chanConfig = {.atten = ADC_ATTEN_DB_12,
                                       .bitwidth = ADC_BITWIDTH_12};

  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, channel, &chanConfig));
}

void setupPin() {
  gpio_reset_pin(ledPin);
  gpio_config_t pinConfig = {.pin_bit_mask = (1ULL << ledPin),
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};

  gpio_config(&pinConfig);
  gpio_set_level(ledPin, 0);
}

int getRaw() {
  int raw = 0;
  ESP_ERROR_CHECK(adc_oneshot_read(adc, channel, &raw));

  return raw;
}

void smaUpdate(int newValue) {
  static uint8_t i = 0;
  smaCurrentSum -= smaValues[i];
  smaValues[i] = newValue;
  smaCurrentSum += newValue;
  smaAverage = smaCurrentSum / SMA_WINDOW_SIZE;

  if (!smaValuesFull && i >= (SMA_WINDOW_SIZE - 1)) {
    smaValuesFull = true;
  }

  i++;

  if (i >= SMA_WINDOW_SIZE) {
    i = 0;
  }
}

void logStatus(int raw) {
  if (!smaValuesFull) {
    ESP_LOGI(
        "Main app: ",
        "Latest measurement is %d. Collecting more data to calculate average.",
        raw);
    return;
  }
  ESP_LOGI("Main app: ",
           "Latest measurement is %d, filtered value is %d, the diff is %d",
           raw, smaAverage, raw - smaAverage);
};

void app_main(void) {

  setupAdc();
  setupPin();

  int64_t lastShotAt = 0;

  while (1) {
    if (esp_timer_get_time() - lastShotAt > 5000) {
      int value = getRaw();
      logStatus(value);
      smaUpdate(value);
      lastShotAt = esp_timer_get_time();

      if (smaAverage < turnLightOnAt) {
        gpio_set_level(ledPin, 1);
      }

      if (smaAverage > turnLightOffAt) {
        gpio_set_level(ledPin, 0);
      }

      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}
