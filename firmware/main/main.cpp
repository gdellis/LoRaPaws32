#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "gpio_driver.hpp"
#include "led_driver.hpp"
#include "button_handler.hpp"

static const char* TAG = "pet-tracker";

#define LED_PIN GPIO_NUM_8
#define BUTTON_PIN GPIO_NUM_9

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 Pet Tracker starting...");

    LedDriver led(LED_PIN);
    ButtonHandler button(BUTTON_PIN, 200000);

    led.off();

    bool led_state = false;

    while (true) {
        if (button.check_pressed()) {
            led_state = !led_state;
            if (led_state) {
                led.on();
            } else {
                led.off();
            }
            ESP_LOGI(TAG, "Button pressed, LED toggled to %s", led_state ? "ON" : "OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
