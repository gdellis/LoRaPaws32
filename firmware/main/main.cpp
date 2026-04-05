/**
 * @file main.cpp
 * @brief ESP32 Pet Tracker application entry point
 *
 * Pet tracker firmware that:
 * - Acquires GPS location via UART
 * - Transmits location data over LoRa SX1262
 * - Supports deep sleep between transmissions
 * - Button press to toggle LED and trigger transmission
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_sleep.h"

#include "board_config.h"
#include "gpio_driver.hpp"
#include "led_driver.hpp"
#include "button_handler.hpp"
#include "deep_sleep.hpp"
#include "gps.hpp"
#include "lora/sx1262.hpp"
#include "accelerometer.hpp"
#include "state_machine.hpp"

static const char* TAG = "pet-tracker";

#define SLEEP_TIMEOUT_MS 30000
#define DEEP_SLEEP_DURATION_US (10LL * 1000000)

void app_main(void) {
    ESP_LOGI(TAG, "Starting pet tracker...");
    
    // Initialize hardware components
    gpio_driver::init();
    led_driver::init();
    button_handler::init();
    
    // Initialize GPS (UART1: TX=GPIO7, RX=GPIO15 @ 115200 baud)
    gps::init(UART_NUM_1);
    
    // Initialize LoRa (SPI2: MOSI=GPIO4, MISO=GPIO5, SCLK=GPIO6, CS=GPIO10)
    lora_driver::init();
    
    // Initialize accelerometer (I2C0: SDA=GPIO2, SCL=GPIO3, INT=GPIO9)
    accelerometer::init(I2C_NUM_0, BOARD_ACCEL_INT_PIN);
    accelerometer::enable_motion_interrupt(2000); // 200mg threshold
    
    // Create and initialize state machine
    static Gps gps;
    static LoRaDriver lora(spi_host_device_t::SPI2_HOST, 
                          GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_10,
                          GPIO_NUM_0, GPIO_NUM_0, GPIO_NUM_9);
    static Accelerometer accel(I2C_NUM_0, BOARD_ACCEL_INT_PIN);
    
    static TrackerStateMachine state_machine(gps, lora, accel);
    state_machine.init();
    
    ESP_LOGI(TAG, "Pet tracker initialized, entering main loop");

    while (true) {
        state_machine.run();
        // The run() method contains the main loop, so this should never return
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Application entry point
 */
extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 Pet Tracker starting...");

    DeepSleep::enable_timer_wakeup(DEEP_SLEEP_DURATION_US);
    DeepSleep::enable_gpio_wakeup(BOARD_BUTTON_PIN, ESP_EXT1_WAKEUP_ANY_LOW);

    switch (DeepSleep::get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_TIMER:
        ESP_LOGI(TAG, "Woke up from deep sleep (timer)");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        ESP_LOGI(TAG, "Woke up from deep sleep (button)");
        break;
    default:
        ESP_LOGI(TAG, "Power-on reset");
        break;
    }

    LedDriver led(BOARD_LED_PIN);
    ButtonHandler button(BOARD_BUTTON_PIN, 200000);
    Gps gps(GPS_UART_NUM);

    if (gps.init()) {
        ESP_LOGI(TAG, "GPS initialized");
    }

    LoRaDriver lora(
        SPI2_HOST,
        BOARD_LORA_MOSI_PIN,
        BOARD_LORA_MISO_PIN,
        BOARD_LORA_SCK_PIN,
        BOARD_LORA_NSS_PIN,
        BOARD_LORA_RESET_PIN,
        BOARD_LORA_BUSY_PIN,
        BOARD_LORA_DIO1_PIN
    );

    if (lora.init() == ESP_OK) {
        ESP_LOGI(TAG, "LoRa initialized");
        lora.set_event_callback(lora_event_handler);
        lora.set_frequency(915000000);
        lora.set_tx_power(22);
        lora.set_spreading_factor(7);
    } else {
        ESP_LOGE(TAG, "LoRa initialization failed");
    }

    led.off();

    bool led_state = false;
    int64_t last_activity_time = esp_timer_get_time() / 1000;
    bool gps_fix_obtained = false;

    while (true) {
        if (button.check_pressed()) {
            last_activity_time = esp_timer_get_time() / 1000;
            led_state = !led_state;
            if (led_state) {
                led.on();
            } else {
                led.off();
            }
            ESP_LOGI(TAG, "Button pressed, LED toggled to %s", led_state ? "ON" : "OFF");
        }

        if (gps.update()) {
            if (gps.has_fix()) {
                const auto& data = gps.get_data();
                ESP_LOGI(TAG, "GPS: lat=%.6f, lon=%.6f, alt=%.1f, satellites=%d",
                         data.latitude, data.longitude, data.altitude, data.satellites);

                if (!gps_fix_obtained) {
                    gps_fix_obtained = true;
                    led.on();
                }

                TrackerPacket pkt = {};
                pkt.type = PACKET_TYPE_LOCATION;
                pkt.device_id = s_device_id;
                pkt.latitude = data.latitude;
                pkt.longitude = data.longitude;
                pkt.altitude = data.altitude;
                pkt.battery_mv = 3700;
                pkt.satellites = data.satellites;
                pkt.timestamp = esp_timer_get_time() / 1000;

                if (lora.get_mode() == LoRaMode::STANDBY) {
                    ESP_LOGI(TAG, "Sending LoRa packet...");
                    esp_err_t err = lora.send_blocking(
                        reinterpret_cast<const uint8_t*>(&pkt), sizeof(pkt), LORA_TX_TIMEOUT_MS);
                    if (err == ESP_OK) {
                        ESP_LOGI(TAG, "LoRa packet sent successfully");
                    } else {
                        ESP_LOGW(TAG, "LoRa send failed: %d", err);
                    }
                }
            }
        }

        int64_t current_time = esp_timer_get_time() / 1000;
        if (current_time - last_activity_time > SLEEP_TIMEOUT_MS) {
            ESP_LOGI(TAG, "Entering deep sleep for %llu seconds...", DEEP_SLEEP_DURATION_US / 1000000);
            led.off();
            lora.sleep();
            DeepSleep::sleep();
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
