#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <ArduinoMqttClient.h>
#include "WiFiManager.h"
#include "secrets.h"

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

public:
    MQTTManager(WiFiManager &wifiManager);

    bool begin();
    void update();
    bool isConnected() const;
    bool publish(const char *topic, const char *payload);

    MqttClient &getClient() { return mqttClient; }
};

#endif