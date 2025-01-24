#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <MQTT.h>
#include "WiFiManager.h"
#include "secrets.h"
#include <ArduinoJson.h>

class MQTTManager
{
private:
    WiFiManager &wifiManager;
    WiFiClient wifiClient;
    MQTTClient mqttClient;
    const char *broker;
    int port;
    const char *clientId;
    bool connected;
    unsigned long lastReconnectAttempt;
    unsigned long reconnectIntervalMs = 5000;
    int connectionAttempts = 10;
    unsigned long connectionAttemptDelayMs = 2000;
    unsigned long keepAliveIntervalMs = 60;
    String subscribedTopic = "";
    uint8_t subscribedQos = 0;
    std::function<void(MQTTClient &, const String &, JsonDocument &)> jsonMessageHandler = nullptr;
    void handleJsonMessage(String &topic, String &payload);

public:
    MQTTManager(WiFiManager &wifiManager);

    bool begin();
    void update();
    bool isConnected() const;
    bool publish(const char *topic, const char *payload);
    String getBroker() const;
    int getPort() const;
    String getClientId() const;
    bool subscribe(const char *topic, uint8_t qos = 2);
    bool unsubscribe(const char *topic);
    void onJsonMessage(void (*callback)(MQTTClient &, const String &, JsonDocument &));

    MQTTClient &getClient() { return mqttClient; }
};

#endif