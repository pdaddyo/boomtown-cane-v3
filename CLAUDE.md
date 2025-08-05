# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the Boomtown LED Cane v3 - a Persistence of Vision (POV) dance cane with ESP32-S3, featuring 144 high-speed LEDs, gyroscope-based motion sensing, and a touchscreen interface. The project supports both hardware execution and SDL2-based desktop emulation.

## Build Commands

### ESP32 Hardware Build
```bash
# Build for ESP32-S3 hardware (default target)
pio run

# Build and upload to device
pio run -t upload

# Monitor serial output
pio device monitor

# Clean build artifacts
pio run -t clean
```

### SDL2 Emulator Build
```bash
# Build and run macOS emulator
pio run -e mac_64 -t upload

# Build and run Windows emulator  
pio run -e windows_64 -t upload

# Alternative execute command
pio run -e mac_64 -t execute
```

### Testing
```bash
# Run unit tests
pio test

# Run tests for specific environment
pio test -e lolin_s3_mini_1_69
```

## Architecture Overview

### Hardware Abstraction Layer (HAL)
The project uses a HAL pattern to support both ESP32 hardware and SDL2 emulation:
- `/hal/esp32/` - ESP32-specific implementations
- `/hal/sdl2/` - SDL2 emulator implementations for desktop testing
- Conditional compilation in `src/main.cpp` switches between hardware and emulator

### Core Components
1. **LED Control**: 
   - 144 APA102 LEDs (72 per side) controlled via `lib/Hardware/leds.h`
   - Patterns defined in `src/patterns.h` (rainbow, confetti, sinelon, etc.)
   - FastLED library for LED manipulation

2. **Motion Sensing**:
   - QMI8658C gyroscope via I2C (`lib/Hardware/gyro.h`)
   - Wave cadence prediction for synchronized effects

3. **User Interface**:
   - LVGL-based touchscreen UI designed in Squareline Studio
   - UI code generated in `src/ui/` directory
   - 1.69" touch LCD display (240x280 resolution)

4. **Storage**:
   - 16MB flash with custom partition scheme (`partitions.csv`)
   - FAT filesystem for image/asset storage
   - OTA update support

### Key Libraries
- **LVGL v8.3.1** - Graphics and UI framework
- **LovyanGFX v1.1.16** - Display driver
- **FastLED v3.9.16** - LED control
- **Qmi8658c** - Gyroscope driver
- **Timber v1.0.0** - Logging

### Development Workflow

#### UI Development
1. Design UI in Squareline Studio (`squareline-studio/` directory)
2. Export generated code to `src/ui/`
3. Test with SDL2 emulator before deploying to hardware

#### Hardware Configuration
- Board: ESP32-S3-DevKitC-1 with 16MB flash
- Upload speed: 921600 baud
- Monitor speed: 115200 baud
- Debug tool: esp-builtin

#### Platform-Specific Notes
- macOS: SDL2 installed via Homebrew, paths auto-detected
- Windows: Requires MSYS64/MinGW64 with SDL2
- Build flags handle platform differences automatically