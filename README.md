# BLE Mesh Detection Project for XIAO ESP32-S3

This project provides BLE mesh detection functionality for the XIAO ESP32-S3 board, with support for both Arduino IDE and PlatformIO development environments.

## Features

- **BLE Device Scanning**: Continuously scans for BLE devices in range
- **OUI Filtering**: Detects devices based on Organizationally Unique Identifiers (OUIs)
- **Redetection Logic**: Alerts when previously seen devices are detected again after 30 seconds
- **Dual Output**: USB Serial for debugging, UART for alerts
- **Multiple OUI Support**: Configurable list of target OUIs

## Target OUIs

The project is configured to detect devices with the following OUIs:
- `00:11:22`
- `11:22:33`
- `44:55:66`

## Pin Configuration

- **UART TX**: D4 
- **UART RX**: D5 

## Project Structure

```
meshdetect/
├── meshdetect.ino          # Arduino IDE sketch
├── platformio/
│   ├── platformio.ini      # PlatformIO configuration
│   └── src/
│       └── main.cpp        # PlatformIO source code
└── README.md               # This file
```

## Requirements

### Arduino IDE
- Arduino IDE 2.x
- ESP32 board support package
- NimBLE-Arduino library (version 2.x)

### PlatformIO
- PlatformIO IDE or CLI
- ESP32 platform
- NimBLE-Arduino library (version 2.x)

## Usage

### Arduino IDE
1. Open `meshdetect.ino` in Arduino IDE
2. Select board: "XIAO ESP32S3"
3. Select correct port
4. Upload sketch

### PlatformIO
1. Open the `platformio` directory in PlatformIO IDE
2. Build and upload the project
3. Monitor serial output

## Output

### USB Serial (Debug)
- All detected devices with RSSI values
- Target OUI matches
- Scan status and timing information

### UART (Alerts)
- Target OUI detection messages
- Redetection alerts (after 30 seconds)

## Configuration

To modify target OUIs, edit the `TARGET_OUIS` vector in the source code:

```cpp
std::vector<std::string> TARGET_OUIS = {
    "00:11:22",  // Target OUI 1
    "11:22:33",  // Target OUI 2
    "44:55:66"   // Target OUI 3
};
```

## Libraries Used

- **NimBLE-Arduino**: BLE functionality (version 2.x)
- **ESP-IDF**: ESP32 system libraries
- **Standard C++**: Vector and algorithm support

## Notes

- The project uses NimBLE 2.x API for improved performance
- UART output is buffered and flushed for reliable transmission
- Serial connection detection prevents blocking on disconnected USB
- Continuous scanning with automatic restart on scan completion 
