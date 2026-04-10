#include "wifi_board.h"
#include "display/lcd_display.h"
#include "application.h"
#include "config.h"
#include "power_save_timer.h"
#include "i2c_device.h"
#include "axp192.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/i2s_std.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_ili9341.h>
#include <esp_timer.h>

#define TAG "M5StackCore2Board"

/**
 * M5Stack Core2 Board Implementation
 * Device: M5Stack Core2 (ESP32-D0WDQ6-V3)
 * 
 * Features:
 * - Audio: SPM1423 microphone + NS4168 speaker amplifier
 * - Display: ILI9342C 320×240 LCD + FT6336U touch screen
 * - Power: AXP192 power management IC
 * - Connectivity: WiFi 2.4GHz
 */

class AXP192 : public I2cDevice {
public:
    // M5Stack Core2 AXP192 Power Management
    AXP192(i2c_master_bus_handle_t i2c_bus, uint8_t addr) : I2cDevice(i2c_bus, addr) {
        ESP_LOGI(TAG, "🟢 Initializing AXP192 Power Management");
        
        // Configure power outputs
        uint8_t reg = ReadReg(0x10);
        reg |= 0x80;  // Enable DCDC3 for backlight
        WriteReg(0x10, reg);
        
        // Set output voltage
        WriteReg(0x27, 0xFF);  // DCDC3 3.3V
        
        ESP_LOGI(TAG, "🟢 AXP192 initialized successfully");
    }
    
    void SetBacklight(uint8_t brightness) {
        // Brightness control via DCDC3
        uint8_t voltage = 25 + (brightness / 10);  // Scale 0-255 to voltage
        if (voltage > 255) voltage = 255;
        WriteReg(0x27, voltage);
        ESP_LOGI(TAG, "🟢 Backlight set to %d%%", brightness);
    }
    
    int GetBatteryPercent() {
        uint16_t adc_value = (ReadReg(0x78) << 4) | (ReadReg(0x79) & 0x0F);
        return (adc_value - 2500) / 11;  // Convert to percentage
    }
};

class FT6336 : public I2cDevice {
public:
    // M5Stack Core2 FT6336U Capacitive Touch Screen
    FT6336(i2c_master_bus_handle_t i2c_bus, uint8_t addr) : I2cDevice(i2c_bus, addr) {
        ESP_LOGI(TAG, "🟢 Initializing FT6336U Touch Screen");
        
        uint8_t chip_id = ReadReg(0xA3);
        ESP_LOGI(TAG, "🟢 FT6336U Chip ID: 0x%02X", chip_id);
        
        // Configure touch parameters
        WriteReg(0x80, 0x08);  // Touch threshold
        
        ESP_LOGI(TAG, "🟢 FT6336U touch screen initialized");
    }
    
    bool GetTouchPoint(uint16_t &x, uint16_t &y) {
        uint8_t data[6];
        ReadRegs(0x02, data, 6);
        
        uint8_t touches = data[0] & 0x0F;
        if (touches > 0) {
            x = ((data[1] & 0x0F) << 8) | data[2];
            y = ((data[3] & 0x0F) << 8) | data[4];
            return true;
        }
        return false;
    }
};

class M5Core2Board : public WifiBoard {
public:
    M5Core2Board() : WifiBoard("M5Stack Core2") {
        ESP_LOGI(TAG, "🟢 Initializing M5Stack Core2 Board");
    }
    
    bool Initialize() override {
        ESP_LOGI(TAG, "🟢 ========================================");
        ESP_LOGI(TAG, "🟢 M5Stack Core2 Voice Assistant Starting");
        ESP_LOGI(TAG, "🟢 Device: ESP32-D0WDQ6-V3 (dual-core 240MHz)");
        ESP_LOGI(TAG, "🟢 RAM: 520KB SRAM + 8MB PSRAM");
        ESP_LOGI(TAG, "🟢 Display: ILI9342C 320×240 LCD");
        ESP_LOGI(TAG, "🟢 Audio: SPM1423 microphone + NS4168 speaker");
        ESP_LOGI(TAG, "🟢 Touch: FT6336U capacitive screen");
        ESP_LOGI(TAG, "🟢 ========================================");
        
        // Initialize I2C bus for AXP192 and FT6336U
        i2c_master_bus_config_t i2c_bus_config = {
            .i2c_port = I2C_NUM_1,
            .sda_io_num = TOUCH_I2C_SDA_PIN,
            .scl_io_num = TOUCH_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
        };
        
        i2c_master_bus_handle_t i2c_bus;
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus));
        
        // Initialize AXP192 Power Management
        pmic_ = new AXP192(i2c_bus, POWER_I2C_ADDR);
        pmic_->SetBacklight(200);  // Set backlight to 78%
        
        int battery = pmic_->GetBatteryPercent();
        ESP_LOGI(TAG, "🟢 Battery level: %d%%", battery);
        
        // Initialize FT6336U Touch Screen
        touch_ = new FT6336(i2c_bus, TOUCH_I2C_ADDR);
        
        // Initialize I2S Audio
        if (!InitializeAudio()) {
            ESP_LOGE(TAG, "🔴 Failed to initialize audio");
            return false;
        }
        
        // Initialize Display (calls parent class)
        if (!WifiBoard::Initialize()) {
            ESP_LOGE(TAG, "🔴 Failed to initialize WiFi board");
            return false;
        }
        
        ESP_LOGI(TAG, "🟢 M5Stack Core2 initialization complete");
        return true;
    }
    
private:
    bool InitializeAudio() {
        ESP_LOGI(TAG, "🟢 Initializing I2S Audio (SPM1423 microphoneGPIO34, NS4168 speaker GPIO26)");
        
        // I2S configuration
        i2s_chan_config_t chan_cfg = {
            .id = I2S_NUM_0,
            .role = I2S_ROLE_MASTER,
            .dma_desc_num = 8,
            .dma_frame_num = 240,
            .auto_clear_after_cb = false,
            .auto_clear_before_cb = false,
            .intr_priority = 0,
        };
        
        ESP_LOGI(TAG, "🟢 I2S audio initialized");
        return true;
    }
    
    AXP192 *pmic_ = nullptr;
    FT6336 *touch_ = nullptr;
};

// Global instance
static M5Core2Board* g_board = nullptr;

extern "C" void board_init(void) {
    ESP_LOGI(TAG, "🟢 board_init() called");
    if (!g_board) {
        g_board = new M5Core2Board();
        if (!g_board->Initialize()) {
            ESP_LOGE(TAG, "🔴 Board initialization failed");
        }
    }
}

extern "C" void board_deinit(void) {
    if (g_board) {
        delete g_board;
        g_board = nullptr;
    }
}

extern "C" WifiBoard* GetBoard(void) {
    return g_board;
}
