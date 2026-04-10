#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_heap_caps.h>

#include "application.h"

#define TAG "M5Core2-Main"

// Color codes for terminal output (if CONFIG_LOG_COLORS enabled)
#define GREEN   "\033[92m"
#define AMBER   "\033[93m"
#define RED     "\033[91m"
#define RESET   "\033[0m"

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, GREEN "🟢 =================================================" RESET);
    ESP_LOGI(TAG, GREEN "🟢 M5Stack Core2 Voice Assistant - Booting..." RESET);
    ESP_LOGI(TAG, GREEN "🟢 Device: ESP32-D0WDQ6-V3 (240MHz, dual-core)" RESET);
    ESP_LOGI(TAG, GREEN "🟢 Target: xiaozhi-esp32 + Qwen + Cantonese ASR" RESET);
    ESP_LOGI(TAG, GREEN "🟢 =================================================" RESET);
    
    // Initialize NVS flash for WiFi configuration
    ESP_LOGI(TAG, GREEN "🟢 Initializing NVS Flash..." RESET);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, AMBER "⚠️  Erasing NVS flash to fix corruption" RESET);
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, GREEN "🟢 NVS Flash initialized successfully" RESET);
    
    // Log memory information
    ESP_LOGI(TAG, GREEN "🟢 Memory Information:" RESET);
    ESP_LOGI(TAG, GREEN "   - Flash: 16MB" RESET);
    ESP_LOGI(TAG, GREEN "   - PSRAM: 8MB (available)" RESET);
    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, GREEN "   - Heap Free: %u bytes" RESET, free_heap);
    
    // Log WiFi configuration
    ESP_LOGI(TAG, GREEN "🟢 WiFi Configuration:" RESET);
    ESP_LOGI(TAG, GREEN "   - SSID: ASUS_MJ" RESET);
    ESP_LOGI(TAG, GREEN "   - Security: WPA2-PSK" RESET);
    
    // Initialize and run the application
    ESP_LOGI(TAG, GREEN "🟢 Starting xiaozhi-esp32 Application..." RESET);
    auto& app = Application::GetInstance();
    app.Initialize();
    
    ESP_LOGI(TAG, GREEN "🟢 Entering Main Event Loop" RESET);
    app.Run();  // This function runs the main event loop and never returns
    
    // If we reach here, something went wrong
    ESP_LOGE(TAG, RED "🔴 Application terminated unexpectedly!" RESET);
}
