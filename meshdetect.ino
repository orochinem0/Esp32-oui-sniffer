// Disable NimBLE logs (at compile time)
#define NIMBLE_DEBUG_DISABLED

// ================================
// Include Libraries
// ================================
#include <NimBLEDevice.h>
#include <esp_log.h>
#include <vector>
#include <algorithm>

// ================================
// Pin Definitions
// ================================
#define SERIAL1_RX_PIN  D5 // GPIO5
#define SERIAL1_TX_PIN  D4  // GPIO4

// ================================
// OUI Configuration
// ================================
// Add your desired OUIs here (first 3 bytes of MAC address)
// Format: "XX:XX:XX" (case insensitive)
std::vector<std::string> TARGET_OUIS = {
    "00:11:22",  // Target OUI 1
    "11:22:33",  // Target OUI 2
    "44:55:66"   // Target OUI 3
    // Add more OUIs as needed
};

// ================================
// Serial Configuration
// ================================
// Initialize Serial Ports
void initializeSerial() {
    // Initialize USB Serial
    Serial.begin(115200);
    // Removed blocking wait
    Serial.println("USB Serial started.");

    // Initialize Serial1 for UART (TX: D5, RX: D4)
    Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX_PIN, SERIAL1_TX_PIN);
    Serial.println("Serial1 started.");
}

// Function to check if USB Serial is connected
bool isSerialConnected() {
    return Serial && Serial.peek() >= 0;
}

// ================================
// Device Information Struct
// ================================
struct DeviceInfo {
    std::string macAddress;
    int rssi;
    unsigned long firstSeen;
    unsigned long lastSeen;
};

// ================================
// Global Variables
// ================================
// Track devices to avoid duplicates and for redetection
std::vector<DeviceInfo> devices;

// ================================
// Helper Functions
// ================================
// Function to check if a MAC address matches any of our target OUIs
bool isTargetOUI(const std::string& macAddress) {
    // Convert MAC to uppercase for comparison
    std::string upperMac = macAddress;
    std::transform(upperMac.begin(), upperMac.end(), upperMac.begin(), ::toupper);
    
    // Extract first 8 characters (XX:XX:XX format)
    if (upperMac.length() < 8) return false;
    std::string macOUI = upperMac.substr(0, 8);
    
    // Check against all target OUIs
    for (const auto& targetOUI : TARGET_OUIS) {
        std::string upperTargetOUI = targetOUI;
        std::transform(upperTargetOUI.begin(), upperTargetOUI.end(), upperTargetOUI.begin(), ::toupper);
        
        if (macOUI == upperTargetOUI) {
            return true;
        }
    }
    return false;
}

// ================================
// BLE Scan Callbacks Class (Updated for NimBLE 2.x)
// ================================
class MyScanCallbacks : public NimBLEScanCallbacks {
public:
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        // Extract MAC address and RSSI
        std::string mac = advertisedDevice->getAddress().toString();
        int rssi = advertisedDevice->getRSSI();
        unsigned long currentMillis = millis();

        // Log detected device to USB Serial if connected
        if (isSerialConnected()) {
            Serial.print("Detected device: ");
            Serial.print(mac.c_str());
            Serial.print(" RSSI: ");
            Serial.println(rssi);
        }

        // OUI Filtering: Check if MAC matches any of our target OUIs
        if (isTargetOUI(mac)) {
            if (isSerialConnected()) {
                Serial.println("Target OUI match found!");
            }

            // Check if the device is already known
            bool known = false;
            for (auto& dev : devices) {
                if (dev.macAddress == mac) {
                    known = true;

                    // Debugging: Time since last seen
                    if (isSerialConnected()) {
                        Serial.print("Device already known. Time since last seen: ");
                        Serial.print(currentMillis - dev.lastSeen);
                        Serial.println(" ms");
                    }

                    // If redetected after 30 seconds, send redetection message
                    if (currentMillis - dev.lastSeen > 30000) {
                        Serial1.print("Redetection: Device ");
                        Serial1.println(mac.c_str());
                        Serial1.flush(); // Ensure UART buffer is sent
                        if (isSerialConnected()) {
                            Serial.println("Redetection message sent to Serial1.");
                        }
                    }

                    // Update the lastSeen timestamp
                    dev.lastSeen = currentMillis;
                    break;
                }
            }

            // If new, add to list and send alert
            if (!known) {
                DeviceInfo newDev = { mac, rssi, currentMillis, currentMillis };
                devices.push_back(newDev);

                // Send detection message to Serial1
                Serial1.print("Device Detected: ");
                Serial1.println(mac.c_str());
                Serial1.flush(); // Ensure UART buffer is sent
                if (isSerialConnected()) {
                    Serial.println("Sent message to Serial1.");
                }
            }
        }
    }

    void onScanEnd(const NimBLEScanResults& results, int reason) override {
        if (isSerialConnected()) {
            Serial.print("Scan ended. Reason: ");
            Serial.print(reason);
            Serial.print(", Found devices: ");
            Serial.println(results.getCount());
        }
        // Restart scanning automatically
        NimBLEDevice::getScan()->start(0);
    }

    void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice) override {
        // This is called immediately when a device is first discovered
        // We'll handle everything in onResult for simplicity
    }
};

// ================================
// Setup Function
// ================================
void setup() {
    // Short delay for boot stability
    delay(2000);

    // Silence all ESP-IDF logs
    esp_log_level_set("*", ESP_LOG_NONE);

    // Initialize Serial Ports
    initializeSerial();

    // Print configured OUIs
    if (isSerialConnected()) {
        Serial.println("Configured target OUIs:");
        for (const auto& oui : TARGET_OUIS) {
            Serial.print("  - ");
            Serial.println(oui.c_str());
        }
        Serial.println();
    }

    // Initialize NimBLE
    if (!NimBLEDevice::init("")) {
        if (isSerialConnected()) {
            Serial.println("Failed to initialize NimBLE!");
        }
        return;
    }
    
    if (isSerialConnected()) {
        Serial.println("NimBLE initialized.");
    }

    // 5-second pause before starting BLE scan
    if (isSerialConnected()) {
        Serial.println("Pausing for 5 seconds before starting BLE scan...");
    }
    delay(5000);

    // Start BLE Scanning
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(new MyScanCallbacks());
    pScan->setActiveScan(true); // Enable active scanning for detailed data
    pScan->setInterval(1349);   // Scan interval
    pScan->setWindow(449);      // Scan window
    
    // Start continuous scanning (0 = scan forever)
    if (pScan->start(0)) {
        if (isSerialConnected()) {
            Serial.println("BLE scan started (continuous).");
        }
    } else {
        if (isSerialConnected()) {
            Serial.println("Failed to start BLE scan!");
        }
    }
}

// ================================
// Loop Function
// ================================
void loop() {
    // Nothing to process in the loop; BLE scanning runs in the background
    delay(1000); // Small delay to prevent watchdog issues
}
