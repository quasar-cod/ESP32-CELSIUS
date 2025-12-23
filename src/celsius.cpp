#include <Arduino.h>
#include <NimBLEDevice.h>
#include <string>
#include <vector>

// (TARGET_ADDRESS removed — use MAC_WHITELIST to control which devices are processed)

// Optional: list MACs to explicitly allow. If non-empty, only these will be processed.
// Edit to add the addresses you want to target (lowercase, colon-separated).
static const std::vector<std::string> MAC_WHITELIST = {
    "f7:86:17:6f:ad:57",
};

// List MACs to ignore when whitelist is empty (lowercase, colon-separated).
static const std::vector<std::string> MAC_BLACKLIST = {
    "2b:ed:64:24:44:d5",
    "38:01:95:1c:47:33",
    "1e:f6:22:94:c3:3e",
    "e2:da:33:12:4b:79",
    "66:aa:b9:10:ea:87"
};

int scanTime = 5; // Scan duration in seconds (Scan will restart automatically)
NimBLEScan* pBLEScan;

// ----------------------------------------------------------------------
// 1. DATA CALLBACKS (AdvertisedDeviceCallbacks):
//    Handles processing data when a device is found.
// ----------------------------------------------------------------------
class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
public:
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) override {
        if (advertisedDevice == nullptr) return;

        // 1. Filter by MAC Address (case-insensitive string compare)
        std::string advAddr = advertisedDevice->getAddress().toString();
        auto toLower = [](std::string s){ for (auto &c: s) c = tolower(c); return s; };

        std::string advLower = toLower(advAddr);

        // If a whitelist is configured, only process those MACs
        if (!MAC_WHITELIST.empty()) {
            bool allowed = false;
            for (const auto &w : MAC_WHITELIST) if (advLower == toLower(w)) { allowed = true; break; }
            if (!allowed) return; // not in whitelist
        } else {
            // Ignore blacklisted MACs when whitelist is empty
            for (const auto &b : MAC_BLACKLIST) if (advLower == toLower(b)) return;
        }

        // Debug: print summary for every (allowed) discovered device
        size_t payloadLenDbg = advertisedDevice->getPayloadLength();
        std::string nameDbg = advertisedDevice->getName();
        Serial.printf("Found device: %s  RSSI: %d dBm  PayloadLen: %u  Name: %s\n",
                      advertisedDevice->getAddress().toString().c_str(),
                      advertisedDevice->getRSSI(), (unsigned)payloadLenDbg,
                      nameDbg.c_str());

        uint8_t* payloadDbg = advertisedDevice->getPayload();
        if (payloadDbg != nullptr && payloadLenDbg > 0) {
            Serial.print("  Payload: ");
            for (size_t i = 0; i < payloadLenDbg && i < 32; ++i) Serial.printf("%02X ", payloadDbg[i]);
            Serial.println();
        } else {
            Serial.println("  Payload: <none>");
        }

        // At this point the device is allowed (whitelist/blacklist checks passed)
        {
            
            Serial.println("--- SwitchBot Meter (T/H) FOUND & DECODING ---");
            Serial.printf("  Name: %s\n", advertisedDevice->getName().c_str());
            Serial.printf("  MAC: %s\n", advertisedDevice->getAddress().toString().c_str());
            Serial.printf("  RSSI: %d dBm\n", advertisedDevice->getRSSI());

            uint8_t* payload = advertisedDevice->getPayload();
            size_t payloadLen = advertisedDevice->getPayloadLength();

            if (payload == nullptr || payloadLen == 0) {
                Serial.println("  ERROR: No payload to decode.");
                return;
            }

            // Parse AD structures in the payload to find Service Data (type 0x16)
            // looking for UUID 0xFD3D (little-endian bytes: 0x3D 0xFD).
            size_t idx = 0;
            bool decoded = false;
            while (idx + 1 < payloadLen) {
                uint8_t len = payload[idx];
                if (len == 0) break;
                if (idx + len >= payloadLen) break;
                uint8_t type = payload[idx + 1];
                if (type == 0x16 && len >= 3) { // Service Data - 16-bit UUID
                    // UUID is at idx+2 (2 bytes, little-endian)
                    uint16_t uuid = payload[idx + 2] | (payload[idx + 3] << 8);
                    if (uuid == 0xFD3D) {
                        // service data payload starts at idx+4, length = len-3
                        uint8_t* sdata = payload + idx + 4;
                        size_t sLen = len - 3;
                        if (sLen >= 3 && sdata[0] == 0x69) { // 0x69 = SwitchBot Meter
                            // Typical layout (heuristic): [0]=model, [1]=frameType, [2]=battery,
                            // [3],[4]=temperature (various endian/scales), [5]=humidity
                            int battery = -1;
                            float temperature = NAN;
                            float humidity = NAN;
                            if (sLen >= 3) battery = sdata[2];

                            // try multiple temperature interpretations and choose a reasonable one
                            auto inRange = [](float v){ return v > -40.0f && v < 85.0f; };
                            if (sLen >= 5) {
                                // Observed SwitchBot Meter format:
                                // sdata[4] = integer part offset by +128
                                // sdata[3] = fractional tenths (0-9)
                                int temp_int = ((int)sdata[4]) - 128;
                                float temp_frac = ((float)sdata[3]) / 10.0f;
                                if (temp_int >= 0) temperature = (float)temp_int + temp_frac;
                                else temperature = (float)temp_int - temp_frac;
                                // sanity check: if out of expected range, fall back to 16-bit interpretations
                                if (!inRange(temperature)) {
                                    int16_t t_be = (int16_t)((sdata[3] << 8) | sdata[4]);
                                    float t_be_f = (float)t_be / 10.0f;
                                    int8_t t_i8 = (int8_t)sdata[3];
                                    float t_i8_f = (float)t_i8;
                                    if (inRange(t_be_f)) temperature = t_be_f;
                                    else if (inRange(t_i8_f)) temperature = t_i8_f;
                                }
                            } else if (sLen >= 4) {
                                int8_t t_i8 = (int8_t)sdata[3];
                                temperature = (float)t_i8;
                            }

                            if (sLen >= 6) humidity = (float)sdata[5];

                            // Print chosen decoding (fall back to raw print if missing)
                            Serial.printf("  Battery: %d%%\n", (battery >= 0) ? battery : -1);
                            if (!isnan(temperature)) Serial.printf("  Temperature: %.1f°C\n", temperature);
                            else Serial.println("  Temperature: <unknown>");
                            if (!isnan(humidity)) Serial.printf("  Humidity: %.1f%%\n", humidity);
                            else Serial.println("  Humidity: <unknown>");

                            Serial.println("------------------------------------");
                            decoded = true;
                            break;
                        }
                    }
                }
                idx += (size_t)len + 1;
            }
            if (!decoded) {
                Serial.println("  No SwitchBot FD3D service data found to decode.");
                Serial.println("------------------------------------");
            }
        }
    }
};
AdvertisedDeviceCallbacks advCallbacks;


// ----------------------------------------------------------------------
// 2. SCAN CONTROLLER:
//    Use the `start(duration, true)` overload for continuous scanning
//    to avoid relying on scan-complete callback signatures.
// ----------------------------------------------------------------------


void setup() {
    Serial.begin(115200);
    delay(1000); 
    Serial.println("Starting BLE Scan for SwitchBot Meter...");

    // Initialize the BLE stack
    NimBLEDevice::init("");

    pBLEScan = NimBLEDevice::getScan();
    
    // Assign the advertised device callback handler
    pBLEScan->setAdvertisedDeviceCallbacks(&advCallbacks, true);
    
    // Configure scan settings
    // Use active scan to request scan responses and improve discovery reliability
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(100);
    
    Serial.printf("Starting scan for %d seconds (continuous mode)...\n", scanTime);
    pBLEScan->start(scanTime, true);
}

void loop() {
    // The main loop can be empty since the BLE scanning and decoding is handled 
    // asynchronously by the callback functions.
}