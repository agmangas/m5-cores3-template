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
#include "UDPManager.h"
#include "MFRC522_I2C.h"

// If this is true the RFID module must be connected to the M5Stack
const bool ENABLE_RFID = false;

const float DISPLAY_TEXT_SIZE = 2;
const char *TOPIC_SUBSCRIBE = "test/json/#";
const char *TOPIC_PUBLISH = "test/publish/m5test";
const unsigned long PRINT_INTERVAL_MS = 3000;

WiFiManager wifiManager;
MQTTManager mqttManager(wifiManager);
UDPManager udpManager(wifiManager);

// 0x28 is the default address of the MFRC522 module
// If ENABLE_RFID is true, the RFID module should be connected to Port A of the M5Stack
MFRC522 mfrc522(0x28);

unsigned long now = 0;
String publishedValue = "Hello World";
JsonDocument doc;
char uidString[MFRC522::MAX_RFID_UID_SIZE];

void onJsonMessage(MQTTClient &mqttClient, const String &topic, JsonDocument &doc)
{
    Serial.printf("Received message on topic: %s\n", topic.c_str());
    Serial.printf("Message: %s\n", doc.as<String>().c_str());

    if (doc["value"].is<String>())
    {
        Serial.printf("Received value: %s\n", doc["value"].as<String>().c_str());
        publishedValue = doc["value"].as<String>();
    }
}

void printRFIDCard()
{
    if (!ENABLE_RFID)
    {
        return;
    }

    if (!mfrc522.getCardUidAsString(uidString, sizeof(uidString)))
    {
        return;
    }

    if (strlen(uidString) == 0)
    {
        return;
    }

    M5.Display.printf("Card: %s\n", uidString);
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

    if (udpManager.isInitialized())
    {
        M5.Display.printf("UDP: %d\n", udpManager.getLocalPort());
    }
    else
    {
        M5.Display.printf("UDP: Uninitialized\n");
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

    M5.Display.printf("Connecting to MQTT...\n");
    mqttManager.begin();
    mqttManager.onJsonMessage(onJsonMessage);
    mqttManager.subscribe(TOPIC_SUBSCRIBE);

    M5.Display.printf("Initializing UDP...\n");
    udpManager.begin();
}

void loop()
{
    M5.update();
    wifiManager.update();
    mqttManager.update();
    udpManager.update();

    if (millis() - now >= PRINT_INTERVAL_MS)
    {
        updateDisplay();
        now = millis();
        doc.clear();
        doc["value"] = publishedValue;
        doc["now"] = now;
        String payload;
        serializeJson(doc, payload);
        mqttManager.publish(TOPIC_PUBLISH, payload.c_str());
    }

    if (udpManager.isInitialized() && udpManager.parsePacket() > 0)
    {
        String message = udpManager.readString();
        M5.Display.printf("UDP msg: %s\n", message.c_str());
    }
}
