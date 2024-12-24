/*
 * M5Unified is a unified library that supports multiple M5Stack devices
 * (like M5Stack, M5Core2, M5StickC, etc.) with a common API.
 * Unlike board-specific libraries (e.g., M5Stack.h, M5Core2.h),
 * M5Unified allows the same code to work across different M5 devices
 * without modification, making code more portable and maintainable.
 */
#include <M5Unified.h>
#include "WiFiManager.h"
#include "MQTTManager.h"

const float DISPLAY_TEXT_SIZE = 2;
const unsigned long LOOP_DELAY_MS = 1000;

WiFiManager wifiManager;
MQTTManager mqttManager(wifiManager);

void updateDisplay()
{
    M5.Display.clear();
    M5.Display.setCursor(0, 20);

    float batteryLevel = M5.Power.getBatteryLevel();
    bool isCharging = M5.Power.isCharging();

    M5.Display.printf("Battery: %.1f%%\n", batteryLevel);
    M5.Display.printf("Charging: %s\n", isCharging ? F("Yes") : F("No"));

    if (wifiManager.isConnected())
    {
        M5.Display.printf("IP: %s\n", wifiManager.getIP().c_str());
        M5.Display.printf("SSID: %s\n", wifiManager.getSSID().c_str());
    }
    else
    {
        M5.Display.printf("WiFi: Disconnected\n");
    }

    if (mqttManager.isConnected())
    {
        M5.Display.printf("MQTT: OK\n");
    }
    else
    {
        M5.Display.printf("MQTT: Disconnected\n");
    }

    Serial.printf("Battery: %.1f%%\n", batteryLevel);
    Serial.printf("Charging: %s\n", isCharging ? F("Yes") : F("No"));
    Serial.printf("WiFi: %s\n", wifiManager.isConnected() ? F("OK") : F("Disconnected"));
    Serial.printf("MQTT: %s\n", mqttManager.isConnected() ? F("OK") : F("Disconnected"));
}

void setup()
{
    Serial.println(F("Initializing..."));

    // M5.config() provides platform-specific initialization settings
    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Display.setTextSize(DISPLAY_TEXT_SIZE);

    M5.Display.print(F("Connecting to WiFi..."));
    wifiManager.begin();

    if (wifiManager.isConnected())
    {
        M5.Display.print(F("Connecting to MQTT..."));
        mqttManager.begin();
    }
}

void loop()
{
    M5.update();
    wifiManager.update();
    mqttManager.update();
    updateDisplay();
    delay(LOOP_DELAY_MS);
}
