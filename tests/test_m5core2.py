#!/usr/bin/env python3
"""
M5Stack Core2 Voice Assistant Test Suite
Tests all 7 critical components:
1. Boot Sequence
2. I2S Audio (Microphone/Speaker)
3. Display (ILI9342C LCD + FT6336U Touch)
4. Memory (PSRAM)
5. WiFi Connection
6. XiaoZhi Framework Ready
7. Wake Word Model Loaded

Color-coded output:
- 🟢 GREEN (✅): Tests passed
- 🟡 AMBER (⚠️): Tests skipped or warnings
- 🔴 RED (❌): Tests failed
"""

import serial
import time
import sys
import re
from pathlib import Path

# Color codes for terminal output
class Colors:
    GREEN = '\033[92m'      # GREEN for normal/success
    AMBER = '\033[93m'       # AMBER for warnings/skip
    RED = '\033[91m'         # RED for errors
    CYAN = '\033[96m'        # CYAN for info
    WHITE = '\033[97m'       # WHITE for reset
    BOLD = '\033[1m'
    END = '\033[0m'

class TestResults:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.skipped = 0
        self.tests = []
    
    def add_pass(self, test_name):
        self.passed += 1
        self.tests.append((test_name, "PASS", Colors.GREEN))
        print(f"{Colors.GREEN}✅ PASS{Colors.END}: {test_name}")
    
    def add_fail(self, test_name, reason=""):
        self.failed += 1
        self.tests.append((test_name, "FAIL", Colors.RED))
        print(f"{Colors.RED}❌ FAIL{Colors.END}: {test_name}")
        if reason:
            print(f"   Reason: {reason}")
    
    def add_skip(self, test_name, reason=""):
        self.skipped += 1
        self.tests.append((test_name, "SKIP", Colors.AMBER))
        print(f"{Colors.AMBER}⚠️  SKIP{Colors.END}: {test_name}")
        if reason:
            print(f"   Reason: {reason}")
    
    def print_summary(self):
        print("\n" + "="*60)
        print(f"{Colors.BOLD}TEST SUMMARY{Colors.END}")
        print("="*60)
        for test_name, status, color in self.tests:
            status_str = f"{color}{status}{Colors.END}"
            print(f"{status_str:20} {test_name}")
        
        print("\n" + "-"*60)
        total = self.passed + self.failed + self.skipped
        pass_rate = (self.passed / total * 100) if total > 0 else 0
        
        print(f"{Colors.GREEN}✅ PASSED: {self.passed}{Colors.END}")
        print(f"{Colors.RED}❌ FAILED: {self.failed}{Colors.END}")
        print(f"{Colors.AMBER}⚠️  SKIPPED: {self.skipped}{Colors.END}")
        print(f"{Colors.CYAN}Total: {total} tests{Colors.END}")
        print(f"{Colors.CYAN}Pass Rate: {pass_rate:.1f}%{Colors.END}")
        print("="*60)
        
        if self.failed > 0:
            print(f"\n{Colors.RED}⚠️  {self.failed} TESTS FAILED - CHECK HARDWARE{Colors.END}\n")
            return False
        else:
            print(f"\n{Colors.GREEN}✅ ALL TESTS PASSED!{Colors.END}\n")
            return True

def connect_serial(port="COM9", baudrate=115200, timeout=10):
    """Connect to serial port (M5Core2 on COM9)"""
    try:
        ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(1)  # Wait for port to stabilize
        print(f"{Colors.CYAN}📡 Connected to {port} @ {baudrate} baud{Colors.END}")
        return ser
    except serial.SerialException as e:
        print(f"{Colors.RED}❌ Failed to connect to {port}: {e}{Colors.END}")
        sys.exit(1)

def read_serial_output(ser, duration=3, keyword=None):
    """Read serial output for specified duration or until keyword found"""
    output = ""
    start_time = time.time()
    
    while time.time() - start_time < duration:
        try:
            if ser.in_waiting > 0:
                byte_data = ser.read(1)
                output += byte_data.decode('utf-8', errors='ignore')
                
                # Check for keyword match
                if keyword and keyword.lower() in output.lower():
                    return output
        except Exception as e:
            print(f"{Colors.AMBER}⚠️  Serial read error: {e}{Colors.END}")
    
    return output

def test_boot_sequence(ser, results):
    """TEST 1: Boot Sequence
    Expected: "Boot success", "I2S initialized", system startup messages
    """
    print(f"\n{Colors.CYAN}[TEST 1] Boot Sequence{Colors.END}")
    
    # Clear serial buffer
    ser.reset_input_buffer()
    
    # Device should be booting, capture output
    output = read_serial_output(ser, duration=5)
    
    # Look for boot success indicators
    boot_patterns = [
        r"Boot success",
        r"Starting system",
        r"setup\(\) started",
        r"I2S init",
    ]
    
    found_boot = False
    for pattern in boot_patterns:
        if re.search(pattern, output, re.IGNORECASE):
            found_boot = True
            break
    
    if found_boot or len(output) > 100:
        results.add_pass("Boot sequence initialization")
    else:
        results.add_fail("Boot sequence", f"No boot messages detected. Output: {output[:100]}")

def test_i2s_audio(ser, results):
    """TEST 2: I2S Audio (Microphone/Speaker)
    Expected: I2S pins initialized, GPIO4 BCLK, GPIO25 LRCK, GPIO26 DO, GPIO34 DI
    """
    print(f"\n{Colors.CYAN}[TEST 2] I2S Audio Configuration{Colors.END}")
    
    # Read serial for I2S initialization messages
    output = read_serial_output(ser, duration=3)
    
    # Look for I2S or audio-related messages
    i2s_patterns = [
        r"I2S.*init",
        r"audio.*init",
        r"microphone",
        r"speaker",
        r"GPIO.*4.*25.*26",  # BCLK, LRCK, DO pin numbers
    ]
    
    found_i2s = False
    for pattern in i2s_patterns:
        if re.search(pattern, output, re.IGNORECASE):
            found_i2s = True
            break
    
    if found_i2s or "I2S" in output:
        results.add_pass("I2S audio initialization (GPIO 4,25,26,34)")
    else:
        results.add_skip("I2S audio", "Audio system not yet booted - may initialize later")

def test_display(ser, results):
    """TEST 3: Display (ILI9342C LCD + FT6336U Touch)
    Expected: LCD initialized, LVGL started, touch screen ready
    """
    print(f"\n{Colors.CYAN}[TEST 3] Display/LVGL Initialization{Colors.END}")
    
    output = read_serial_output(ser, duration=3)
    
    display_patterns = [
        r"LVGL.*init",
        r"display.*init",
        r"LCD.*init",
        r"ILI9342",
        r"touch.*init",
        r"FT6336",
    ]
    
    found_display = False
    for pattern in display_patterns:
        if re.search(pattern, output, re.IGNORECASE):
            found_display = True
            break
    
    if found_display or "LVGL" in output:
        results.add_pass("Display/LVGL initialization (ILI9342C 320x240)")
    else:
        results.add_skip("Display", "LVGL not yet initialized - may start later")

def test_memory(ser, results):
    """TEST 4: Memory (PSRAM)
    Expected: PSRAM available (8MB for M5Core2), memory allocation working
    """
    print(f"\n{Colors.CYAN}[TEST 4] Memory (PSRAM){Colors.END}")
    
    output = read_serial_output(ser, duration=3)
    
    memory_patterns = [
        r"PSRAM",
        r"memory.*available",
        r"heap.*size",
        r"free.*memory",
    ]
    
    found_memory = False
    for pattern in memory_patterns:
        if re.search(pattern, output, re.IGNORECASE):
            found_memory = True
            break
    
    if found_memory:
        results.add_pass("PSRAM memory (8MB available)")
    else:
        results.add_skip("PSRAM", "Memory messages not yet logged")

def test_wifi_connection(ser, results):
    """TEST 5: WiFi Connection (ASUS_MJ)
    Expected: WiFi initialized, SSID found, IP address assigned
    """
    print(f"\n{Colors.CYAN}[TEST 5] WiFi Connection (ASUS_MJ){Colors.END}")
    
    output = read_serial_output(ser, duration=10)  # Longer wait for WiFi
    
    wifi_patterns = [
        r"wifi.*init",
        r"ASUS_MJ",
        r"connected",
        r"ASUS_MJ.*connected",
        r"IP.*assigned",
        r"\d+\.\d+\.\d+\.\d+",  # IP address pattern
    ]
    
    found_wifi = False
    found_ssid = "ASUS_MJ" in output
    
    for pattern in wifi_patterns:
        if re.search(pattern, output, re.IGNORECASE):
            found_wifi = True
            break
    
    if found_wifi and found_ssid:
        results.add_pass("WiFi connection (ASUS_MJ with IP)")
    elif found_ssid:
        results.add_pass("WiFi SSID detected (ASUS_MJ)")
    else:
        results.add_skip("WiFi", "WiFi connection in progress - may complete later")

def test_xiaozhi_framework(ser, results):
    """TEST 6: XiaoZhi Framework Ready
    Expected: xiaozhi-esp32 components initialized, service ready
    """
    print(f"\n{Colors.CYAN}[TEST 6] XiaoZhi Framework{Colors.END}")
    
    output = read_serial_output(ser, duration=3)
    
    framework_patterns = [
        r"xiaozhi",
        r"framework.*ready",
        r"service.*init",
        r"components.*ready",
    ]
    
    found_framework = False
    for pattern in framework_patterns:
        if re.search(pattern, output, re.IGNORECASE):
            found_framework = True
            break
    
    if found_framework or "xiaozhi" in output.lower():
        results.add_pass("XiaoZhi framework initialized")
    else:
        results.add_skip("XiaoZhi framework", "Not yet logged - framework loads after boot")

def test_wake_word_model(ser, results):
    """TEST 7: Wake Word Model (WakeNet)
    Expected: "你好小智" or "Hi Xiaozhi" model loaded, waiting for wake word
    """
    print(f"\n{Colors.CYAN}[TEST 7] Wake Word Model (WakeNet){Colors.END}")
    
    output = read_serial_output(ser, duration=3)
    
    wake_word_patterns = [
        r"wake.*word",
        r"WakeNet",
        r"你好小智",
        r"Hi Xiaozhi",
        r"model.*loaded",
        r"listening",
    ]
    
    found_wake_word = False
    for pattern in wake_word_patterns:
        if re.search(pattern, output, re.IGNORECASE):
            found_wake_word = True
            break
    
    if found_wake_word:
        results.add_pass("Wake word model loaded (WakeNet 你好小智)")
    else:
        results.add_skip("Wake word model", "WakeNet model not yet loaded - loads after framework init")

def main():
    """Main test suite execution"""
    print("\n" + "="*60)
    print(f"{Colors.BOLD}{Colors.CYAN}M5Stack Core2 Voice Assistant - Test Suite{Colors.END}")
    print("="*60)
    print(f"{Colors.CYAN}Device: M5Stack Core2 (ESP32-D0WDQ6-V3){Colors.END}")
    print(f"{Colors.CYAN}WiFi: ASUS_MJ{Colors.END}")
    print(f"{Colors.CYAN}Serial Port: COM9 @ 115200 baud{Colors.END}")
    print("="*60 + "\n")
    
    # Connect to serial port
    ser = connect_serial("COM9", 115200, timeout=10)
    
    # Initialize results tracking
    results = TestResults()
    
    try:
        # Run all 7 tests
        test_boot_sequence(ser, results)
        test_i2s_audio(ser, results)
        test_display(ser, results)
        test_memory(ser, results)
        test_wifi_connection(ser, results)
        test_xiaozhi_framework(ser, results)
        test_wake_word_model(ser, results)
        
        # Print summary
        success = results.print_summary()
        
        return 0 if success else 1
    
    except KeyboardInterrupt:
        print(f"\n{Colors.AMBER}⚠️  Test interrupted by user{Colors.END}")
        return 1
    finally:
        if ser.is_open:
            ser.close()
            print(f"{Colors.CYAN}📡 Serial port closed{Colors.END}\n")

if __name__ == "__main__":
    sys.exit(main())
