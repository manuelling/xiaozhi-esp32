#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// M5Stack Core2 Board configuration
// Device: M5Stack Core2 (ESP32-D0WDQ6-V3)
// Audio: SPM1423 Microphone (GPIO34), NS4168 Amplifier + 1W Speaker (GPIO26)
// Display: ILI9342C 320×240 LCD with FT6336U Touch Screen
// Connectivity: WiFi 2.4GHz, 500mAh Battery

#include <driver/gpio.h>

// ==================== AUDIO CONFIGURATION ====================
#define AUDIO_INPUT_REFERENCE    true
#define AUDIO_INPUT_SAMPLE_RATE  16000    // 16kHz for ASR
#define AUDIO_OUTPUT_SAMPLE_RATE 16000    // 16kHz for TTS

// I2S Audio Pins (M5Core2 specific)
#define AUDIO_I2S_GPIO_MCLK  GPIO_NUM_0       // Master Clock for microphone
#define AUDIO_I2S_GPIO_WS    GPIO_NUM_25      // LRCK (Left/Right Channel Clock)
#define AUDIO_I2S_GPIO_BCLK  GPIO_NUM_4       // Bit Clock
#define AUDIO_I2S_GPIO_DIN   GPIO_NUM_34      // Data In from SPM1423 microphone
#define AUDIO_I2S_GPIO_DOUT  GPIO_NUM_26      // Data Out to NS4168 speaker

// I2C Audio Codec (if using external codec, not applicable for Core2)
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_NC
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_NC
#define AUDIO_CODEC_ADDR         0x00          // Not used

// ==================== BUTTON CONFIGURATION ====================
#define BUILTIN_LED_GPIO        GPIO_NUM_NC   // Core2 has RGB LED on AXP192
#define BOOT_BUTTON_GPIO        GPIO_NUM_37   // Optional boot button
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

// ==================== DISPLAY CONFIGURATION ====================
// Display: ILI9342C 320×240 resolution
// Interface: SPI (GPIO23 MOSI, GPIO38 MISO, GPIO18 SCK, GPIO5 CS, GPIO15 DC)
// Backlight: GPIO12

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240

// Display SPI Pins
#define DISPLAY_SDA_PIN GPIO_NUM_NC   // Not using I2C display
#define DISPLAY_SCL_PIN GPIO_NUM_NC

// Display orientation
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY  false

// Display offsets
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

// Backlight control
#define DISPLAY_BACKLIGHT_PIN           GPIO_NUM_12
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

// ==================== TOUCH SCREEN CONFIGURATION ====================
// Touch Controller: FT6336U (0x38 address)
// Interface: I2C (GPIO21 SDA, GPIO22 SCL)

#define TOUCH_I2C_SDA_PIN  GPIO_NUM_21
#define TOUCH_I2C_SCL_PIN  GPIO_NUM_22
#define TOUCH_I2C_ADDR     0x38
#define TOUCH_CONTROLLER   FT6336U

// ==================== POWER MANAGEMENT ====================
// AXP192 Power Management IC
#define POWER_I2C_SDA_PIN  GPIO_NUM_21
#define POWER_I2C_SCL_PIN  GPIO_NUM_22
#define POWER_I2C_ADDR     0x34          // AXP192 I2C address

// ==================== CAMERA PINS ====================
// M5Core2 does not have built-in camera
#define CAMERA_PIN_PWDN   GPIO_NUM_NC
#define CAMERA_PIN_RESET  GPIO_NUM_NC
#define CAMERA_PIN_XCLK   GPIO_NUM_NC
#define CAMERA_PIN_SIOD   GPIO_NUM_NC
#define CAMERA_PIN_SIOC   GPIO_NUM_NC
#define CAMERA_PIN_D0     GPIO_NUM_NC
#define CAMERA_PIN_D1     GPIO_NUM_NC
#define CAMERA_PIN_D2     GPIO_NUM_NC
#define CAMERA_PIN_D3     GPIO_NUM_NC
#define CAMERA_PIN_D4     GPIO_NUM_NC
#define CAMERA_PIN_D5     GPIO_NUM_NC
#define CAMERA_PIN_D6     GPIO_NUM_NC
#define CAMERA_PIN_D7     GPIO_NUM_NC
#define CAMERA_PIN_VSYNC  GPIO_NUM_NC
#define CAMERA_PIN_HREF   GPIO_NUM_NC
#define CAMERA_PIN_PCLK   GPIO_NUM_NC

// ==================== SDCARD CONFIGURATION ====================
// M5Core2 has SDCard slot (optional)
#define SDCARD_MOSI_PIN GPIO_NUM_NC    // Using SPI shared with display
#define SDCARD_MISO_PIN GPIO_NUM_NC
#define SDCARD_SCK_PIN  GPIO_NUM_NC
#define SDCARD_CS_PIN   GPIO_NUM_NC

// ==================== LOGGING CONFIGURATION ====================
// Color-coded logging for debugging
#define LOG_COLOR_ENABLED  true
#define LOG_LEVEL_DEFAULT  ESP_LOG_INFO

#endif  // _BOARD_CONFIG_H_
