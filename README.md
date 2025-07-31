# Cam Sniffer

This project uses a Seedstudio Xiao ESP32-S3 to scan for a specific OUI or group of OUIs via BLE. 
If the specified device is detected, a message is sent to the Heltec LoRa V3 and distributed over a LoRa mesh network such as Meshtastic.

<img src="cam.png" alt="deepwoods" style="width:50%; height:25%;">

## Features

- **BLE Device Scanning**: Continuously scans for BLE devices in range
- **OUI Filtering**: Detects devices based on Organizationally Unique Identifiers (OUIs)
- **Redetection Logic**: Alerts when previously seen devices are detected again after 30 seconds
- **Dual Output**: USB Serial for debugging, UART for alerts
- **Multiple OUI Support**: Configurable list of target OUIs
- **Meshtastic Integration**: Sends alerts over LoRa mesh network

## Target OUIs

The project is configured to detect devices with the following OUIs:
- `00:11:22`
- `11:22:33`
- `44:55:66`

## Requirements
- Seedstudio Xiao ESP32-S3
- Heltec LoRa V3
- Meshtastic framework or other LoRa mesh framework
- Current NimBLE library for BLE integration
- You need other devices on your mesh channel to receive the alerts from this device. I highly recommend using a Sensecap LoRaWAN Card Tracker from Seeedstudio
 https://www.seeedstudio.com/SenseCAP-Card-Tracker-T1000-E-for-Meshtastic-p-5913.html

## Pin Configuration

- **UART TX**: D4 (GPIO4)
- **UART RX**: D5 (GPIO5)

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

## Setup
1. Edit **Line 18** in `meshdetect.ino`, replacing the OUIs in the `TARGET_OUIS` vector with the OUI(s) of your target devices.
2. If you are not using the Mesh Detect PCB, connect pins Rx 19 and Tx 20 of the Heltec board to Tx D4 and Rx D5 on the Xiao ESP32-S3 board.
3. Flash meshdetect.ino to your Xiao board via Arduino IDE.
4. Flash your Heltec board with latest Meshtastic firmware at [flasher.meshtastic.org](https://flasher.meshtastic.org) and set region to US
5. In Meshtastic app, configure your Heltec device serial module settings:
   - TextMessage
   - 115200 baud
   - Pins: Rx 19 and Tx 20 on the Heltec board

## Serial Connection
<img src="https://raw.githubusercontent.com/colonelpanichacks/esp32-oui-sniffer/Xiao-esp32-c3-serial/serial.jpg" alt="Serial Connection" width="400">

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
1. Put device on mesh channel of your choice. 
2. Power the device via the Xiao ESP32-S3 USB-C port. BLE scanner will continuously send a serial message over Meshtastic if your target device/devices are detected.

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

## Contributing
Fork the repository and use a feature branch. Pull requests welcome.

## Order a PCB for this project
<a href="https://www.tindie.com/stores/colonel_panic/?ref=offsite_badges&utm_source=sellers_colonel_panic&utm_medium=badges&utm_campaign=badge_large">
    <img src="https://d2ss6ovg47m0r5.cloudfront.net/badges/tindie-larges.png" alt="I sell on Tindie" width="200" height="104">
</a> 
