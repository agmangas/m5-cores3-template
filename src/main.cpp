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
#include "MFRC522_I2C.h"

// If this is true the RFID module must be connected to the M5Stack
const bool ENABLE_RFID = true;

const float DISPLAY_TEXT_SIZE = 2;
const unsigned long LOOP_DELAY_MS = 1000;
const char *TEST_MQTT_TOPIC = "test/json/#";

WiFiManager wifiManager;
MQTTManager mqttManager(wifiManager);

// 0x28 is the default address of the MFRC522 module
// If ENABLE_RFID is true, the RFID module should be connected to Port A of the M5Stack
MFRC522 mfrc522(0x28);

void onJsonMessage(MqttClient &mqttClient, const String &topic, JsonDocument &doc)
{
    Serial.printf("Received message on topic: %s\n", topic.c_str());
    Serial.printf("Message: %s\n", doc.as<String>().c_str());
}

void printRFIDCard()
{
    if (!ENABLE_RFID)
    {
        return;
    }

    String uid = mfrc522.getCardUidAsString();

    if (uid.isEmpty())
    {
        return;
    }

    M5.Display.printf("Card: %s\n", uid.c_str());
}

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
        M5.Display.printf("MQTT: %s:%d\n", mqttManager.getBroker().c_str(), mqttManager.getPort());
    }
    else
    {
        M5.Display.printf("MQTT: Disconnected\n");
    }

    Serial.printf("Battery: %.1f%%\n", batteryLevel);
    Serial.printf("Charging: %s\n", isCharging ? F("Yes") : F("No"));
    Serial.printf("WiFi: %s\n", wifiManager.isConnected() ? F("OK") : F("Disconnected"));
    Serial.printf("MQTT: %s\n", mqttManager.isConnected() ? F("OK") : F("Disconnected"));

    printRFIDCard();
}

void setup()
{
    Serial.begin(115200);
    Serial.println(F("Initializing..."));

    // M5.config() provides platform-specific initialization settings
    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Display.setTextSize(DISPLAY_TEXT_SIZE);

    M5.Display.printf("Init I2C...\n");
    Wire.begin();

    if (ENABLE_RFID)
    {
        M5.Display.printf("Init MFRC522...\n");
        mfrc522.PCD_Init();
    }

    M5.Display.printf("Connecting to WiFi...\n");
    wifiManager.begin();

    if (wifiManager.isConnected())
    {
        M5.Display.printf("Connecting to MQTT...\n");
        mqttManager.begin();
        mqttManager.subscribe(TEST_MQTT_TOPIC);
        mqttManager.onJsonMessage(onJsonMessage);
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
