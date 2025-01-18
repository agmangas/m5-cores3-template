#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <ArduinoMqttClient.h>
#include "WiFiManager.h"
#include "secrets.h"
#include <ArduinoJson.h>

class MQTTManager
{
private:
    WiFiManager &wifiManager;
    WiFiClient wifiClient;
    MqttClient mqttClient;
    const char *broker;
    int port;
    const char *clientId;
    bool connected;
    unsigned long lastReconnectAttempt;
    unsigned long reconnectIntervalMs = 5000;
    int connectionAttempts = 3;
    unsigned long connectionAttemptDelayMs = 1000;
    unsigned long keepAliveIntervalMs = 60;
    std::function<void(MqttClient &, const String &, JsonDocument &)> jsonMessageHandler = nullptr;
    void handleJsonMessage(int messageSize);

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
    void onJsonMessage(void (*callback)(MqttClient &, const String &, JsonDocument &));

    MqttClient &getClient() { return mqttClient; }
};

#endif