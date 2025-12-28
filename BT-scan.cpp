// Disabled duplicate example to avoid multiple definitions (kept for reference)
#if 0
#include <Arduino.h>
#include <NimBLEDevice.h>

int         scanTime = 5 * 1000; // In milliseconds, 0 = scan forever
NimBLEScan* pBLEScan;

bool active = false;

class ScanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
public:
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) override {
        if (advertisedDevice == nullptr) return;
        Serial.printf("Advertised Device Result: %s \n", advertisedDevice->toString().c_str());
    }
} scanCallbacks;

void setup() {
    Serial.begin(115200);
    Serial.println("Scanning...");

    NimBLEDevice::init("active-passive-scan");
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(&scanCallbacks, true);
    pBLEScan->setActiveScan(active);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(100);
    pBLEScan->start(scanTime);
}
#endif
