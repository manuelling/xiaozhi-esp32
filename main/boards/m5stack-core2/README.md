# M5Stack Core2 Voice Assistant Configuration

## Board Overview
- **Device**: M5Stack Core2 (ESP32-D0WDQ6-V3)
- **Processor**: Dual-core Xtensa 240MHz
- **Memory**: 16MB Flash, 8MB PSRAM, 520KB SRAM
- **Battery**: 500mAh lithium-ion
- **Connectivity**: WiFi 2.4GHz IEEE 802.11b/g/n

## Hardware Components

### Audio System
- **Microphone**: SPM1423 PDM microphone
  - I2S Input Pin: GPIO34
  - Clock Pin: GPIO0
  - I2S Interface: Master Clock (GPIO0), WS/LRCK (GPIO25), BCLK (GPIO4), DIN (GPIO34)

- **Speaker**: 1W speaker with NS4168 Class-D amplifier
  - I2S Output Pin: GPIO26
  - I2S Interface: BCLK (GPIO4), LRCK (GPIO25), DOUT (GPIO26)

### Display System
- **LCD Controller**: ILI9342C
- **Resolution**: 320×240 pixels
- **Interface**: SPI
  - MOSI: GPIO23
  - MISO: GPIO38
  - SCK: GPIO18
  - CS: GPIO5
  - DC: GPIO15
  - RST: GPIO1
  - Backlight: GPIO12

### Touch Screen
- **Controller**: FT6336U capacitive touch
- **I2C Address**: 0x38
- **I2C Interface**:
  - SDA: GPIO21
  - SCL: GPIO22

### Power Management
- **IC**: AXP192
- **I2C Address**: 0x34
- **I2C Interface**: SDA (GPIO21), SCL (GPIO22)
- **Functions**:
  - Battery charging and management
  - Backlight brightness control
  - Power supply regulation
  - System shutdown capability

## GPIO Pin Mapping

| Function | Pin | Type |
|----------|-----|------|
| **Audio** | | |
| I2S MCLK | GPIO0 | Output |
| I2S BCLK | GPIO4 | Output |
| I2S LRCK | GPIO25 | Output |
| I2S DIN (Mic) | GPIO34 | Input |
| I2S DOUT (Speaker) | GPIO26 | Output |
| **Display SPI** | | |
| MOSI | GPIO23 | Output |
| MISO | GPIO38 | Input |
| SCK | GPIO18 | Output |
| CS | GPIO5 | Output |
| DC (Data/Command) | GPIO15 | Output |
| RST (Reset) | GPIO1 | Output |
| Backlight | GPIO12 | Output (PWM) |
| **Touch/Power I2C** | | |
| SDA | GPIO21 | Bi-directional |
| SCL | GPIO22 | Bi-directional |
| **Serial** | | |
| TX | GPIO1 | Output |
| RX | GPIO3 | Input |

## Configuration Files

### config.h
Board-specific GPIO definitions and feature flags

### config.json
Board metadata including pin mappings, memory info, and feature list

### m5stack_core2.cc
Board initialization code including:
- AXP192 power management setup
- FT6336U touch screen initialization
- I2S audio configuration
- Display initialization

## Setup Instructions

1. **WiFi Configuration**
   - SSID: `ASUS_MJ`
   - Password: `5544332211`

2. **Compiler Target**
   - Set ESP-IDF target to `esp32`
   - Use sdkconfig.m5core2 for device-specific settings

3. **Build Commands**
   ```bash
   idf.py -p COM9 -b 921600 build
   idf.py -p COM9 -b 921600 flash
   idf.py -p COM9 -b 115200 monitor
   ```

## Features
- ✅ Full audio input (microphone) and output (speaker) via I2S
- ✅ 320×240 color LCD display with touch support
- ✅ WiFi connectivity for cloud services
- ✅ Battery monitoring and power management
- ✅ Color-coded logging (GREEN/AMBER/RED) for debugging
- ✅ Compatible with xiaozhi-esp32 framework

## Logging
All system messages are color-coded for easy debugging:
- 🟢 **GREEN** (`🟢`): Normal operation/success
- 🟡 **AMBER** (`⚠️`): Warnings
- 🔴 **RED** (`🔴`): Errors

## Development Notes
- The Core2 uses I2C bus 1 (GPIO21 SDA, GPIO22 SCL) for both touch and power management
- Audio is configured for 16kHz sample rate (standard for ASR)
- Display backlight is controlled via PWM through AXP192
- Battery level can be read from the AXP192 ADC
